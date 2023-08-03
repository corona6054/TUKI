#include "../includes/kernel.h"

// usleep(5 * 1000000);

t_instruction* inicializar_instruccion(InstructionType codigo, char* str1, char* str2){
	t_instruction* instr = malloc(sizeof(t_instruction));

	instr -> instruccion = codigo;

	int tam1 = 0;
	while(str1[tam1] != NULL)
		tam1++;
	instr->string1 = malloc(tam1);
	strcpy(instr->string1, str1);

	int tam2 = 0;
	while(str1[tam1] != NULL)
		tam2++;
	instr->string2 = malloc(tam2);
	strcpy(instr->string2, str2);

	return instr;
}

t_list* instruccion_prueba(){
	t_list* lista_prueba = list_create();

	list_add(lista_prueba, inicializar_instruccion(SET, "AX", "AAAA"));
	list_add(lista_prueba, inicializar_instruccion(SET, "BX", "BBBB"));
	list_add(lista_prueba, inicializar_instruccion(SET, "CX", "CCCC"));
	list_add(lista_prueba, inicializar_instruccion(SET, "DX", "DDDD"));
	list_add(lista_prueba, inicializar_instruccion(SET, "EAX", "AAAAAAAA"));
	list_add(lista_prueba, inicializar_instruccion(SET, "EBX", "BBBBBBBB"));
	list_add(lista_prueba, inicializar_instruccion(SET, "ECX", "CCCCCCCC"));
	list_add(lista_prueba, inicializar_instruccion(SET, "EDX", "DDDDDDDD"));
	list_add(lista_prueba, inicializar_instruccion(SET, "RAX", "AAAAAAAAAAAAAAAA"));
	list_add(lista_prueba, inicializar_instruccion(SET, "RBX", "BBBBBBBBBBBBBBBB"));
	list_add(lista_prueba, inicializar_instruccion(SET, "RCX", "CCCCCCCCCCCCCCCC"));
	list_add(lista_prueba, inicializar_instruccion(SET, "RDX", "DDDDDDDDDDDDDDDD"));
	list_add(lista_prueba, inicializar_instruccion(YIELD, "v", "v"));
	list_add(lista_prueba, inicializar_instruccion(SET, "RAX", "TCHOUAMENIAFUERA"));
	list_add(lista_prueba, inicializar_instruccion(SET, "RBX", "SEGUNDO_FRANCIA_"));
	list_add(lista_prueba, inicializar_instruccion(EXIT, "v", "v"));

	return lista_prueba;
}

int main(void){

	levantar_modulo();

	/*
	t_list* lista_instrucciones_prueba = instruccion_prueba();

	t_pcb*  pcb = crear_pcb(lista_instrucciones_prueba, 25);

	log_info(logger, "Por agregar  PCB a cola NEW");
	agregar_pcb_a(procesosNew, pcb, &mutex_new);
	sem_post(&cont_new);

	log_info(logger, "Por iniciar los planificadores");
	*/
	planificadores();

	//time_t tiempo_actual = time(NULL);
	//log_info(logger, "Tiempo actual: %lf", tiempo_actual);

	while(1);
	return 0;
}


// SUBPROGRAMAS
void levantar_modulo(){
	logger = iniciar_logger();
	config = iniciar_config();
	levantar_config();

	incializar_listas();
	incializar_semaforos();
	inicializar_recurso_nulo();

	
	establecer_conexiones();
	return;
}

void incializar_listas(){
	procesosNew = queue_create();
	procesosReady = queue_create();
	procesosExec = queue_create();
	procesosBlocked = queue_create();
	procesosExit = queue_create();

	pcb_en_ejecucion = malloc(sizeof(t_pcb));

	return;
}

void incializar_semaforos(){
	// COLAS
	pthread_mutex_init(&mutex_new, NULL);
	pthread_mutex_init(&mutex_ready, NULL);
	pthread_mutex_init(&mutex_blocked, NULL);
	pthread_mutex_init(&mutex_exec, NULL);
	pthread_mutex_init(&mutex_exit, NULL);
	// FIN COLAS
	
	pthread_mutex_init(&mutex_pid_a_asignar, NULL); // para var global pid
	
	//para no agregar mas procesos a READY de los q permite el grado max de multi
	sem_init(&cont_grado_max_multiprog, 0, config_kernel.grado_max_multi); 

	//para postear cuando uno entra a exit y poder matar su consola
	sem_init(&cont_new, 0, 0); 
	sem_init(&cont_ready, 0, 0);
	sem_init(&cont_exec, 0, 0);
	sem_init(&cont_exit, 0, 0);

	sem_init(&bin1_envio_cde, 0, 1);
	sem_init(&bin2_recibir_cde, 0, 0);

	sem_init(&necesito_enviar_cde, 0, 0);

	// Para ver si el procesador esta libre para mandar un proceso a ejecucion
	sem_init(&cont_procesador_libre, 0, 1);
}

void finalizar_modulo(){
	log_destroy(logger);
	config_destroy(config);
	return;
}


// UTILS INICIAR MODULO -----------------------------------------------------------------
t_log* iniciar_logger(void)
{
	t_log* nuevo_logger;
	nuevo_logger = log_create("kernel.log", "Kernel", 1, LOG_LEVEL_INFO);

	if (nuevo_logger == NULL)
	{
		printf("Error al crear el logger\n");
		exit(1);
	}

	return nuevo_logger;
}

t_config* iniciar_config(void)
{
	t_config* nuevo_config;

	nuevo_config = config_create("kernel.config");

	if (nuevo_config == NULL){
		printf("Error al crear el nuevo config\n");
		exit(2);
	}

	return nuevo_config;
}

void levantar_config(){

	config_kernel.ip_memoria = config_get_string_value(config,"IP_MEMORIA");
	config_kernel.puerto_memoria = config_get_int_value(config,"PUERTO_MEMORIA");

	config_kernel.ip_cpu = config_get_string_value(config,"IP_CPU");
	config_kernel.puerto_cpu = config_get_int_value(config,"PUERTO_CPU");

	config_kernel.ip_file_system = config_get_string_value(config,"IP_FILESYSTEM");
	config_kernel.puerto_file_system = config_get_int_value(config,"PUERTO_FILESYSTEM");

	config_kernel.puerto_escucha = config_get_int_value(config,"PUERTO_ESCUCHA");
	config_kernel.algoritmo = config_get_string_value(config,"ALGORITMO_PLANIFICACION");
	config_kernel.est_inicial = config_get_int_value(config,"ESTIMACION_INICIAL");
	config_kernel.alpha_hrrn = config_get_int_value(config,"HRRN_ALFA");
	config_kernel.grado_max_multi = config_get_int_value(config,"GRADO_MAX_MULTIPROGRAMACION");

	char **recursos = config_get_array_value(config,"RECURSOS");
	char **instancias = config_get_array_value(config,"INSTANCIAS_RECURSOS");

	config_kernel.recursos = list_create();

	int a = 0;
	while(recursos[a]!= NULL){
		int num = strtol(instancias[a],NULL,10);
		t_recurso* recurso = inicializar_recurso(recursos[a], num);
		list_add(config_kernel.recursos, recurso);
		a++;
	}
}
// FIN UTILS INICIAR MODULO -------------------------------------------------------------


// UTILS CONEXIONES ---------------------------------------------------------------------
void manejar_conexion_con_memoria(){

}

void manejar_conexion_con_cpu(){
	pthread_t p1;
	pthread_create(&p1, NULL, (void*) enviar_cde_a_cpu, NULL);
	pthread_detach(p1);

	pthread_t p2;
	pthread_create(&p2, NULL, (void*) recibir_cde_de_cpu, NULL);
	pthread_detach(p2);
}

void manejar_conexion_con_file_system(){
	
}

void establecer_conexiones()
{
	/*
	socket_memoria = crear_conexion(config_kernel.ip_memoria, config_kernel.puerto_memoria);
	if (socket_memoria >= 0){
		log_info(logger,"Conectado con memoria");
		//realizar handshake;
		
		pthread_t conexion_memoria;
		pthread_create(&conexion_memoria, NULL, (void *) manejar_conexion_con_memoria, NULL);
		pthread_detach(conexion_memoria);
	}
	else
		log_info(logger,"Error al conectar con memoria");
	*/

	socket_cpu = crear_conexion(config_kernel.ip_cpu, config_kernel.puerto_cpu);
	if (socket_cpu >= 0){			
		log_info(logger,"Conectado con cpu");

		enviar_handshake(socket_cpu);
		if (recibir_handshake(socket_cpu) == HANDSHAKE){
			log_info(logger, "Handshake con CPU realizado.");

			manejar_conexion_con_cpu();

		}else
			log_info(logger, "Ocurio un error al realizar el handshake con CPU.");
	}
	else
		log_info(logger,"Error al conectar con cpu");

	/*
	socket_file_system = crear_conexion(config_kernel.ip_file_system, config_kernel.puerto_file_system);
	if (socket_file_system >= 0){
		log_info(logger,"Conectado con file system");
		pthread_t conexion_file_system;
		pthread_create(&conexion_file_system, NULL, (void *) manejar_conexion_con_file_system, NULL);
		pthread_detach(conexion_file_system);
	}else
		log_info(logger,"Error al conectar con file system");
	*/

	
	server_fd = iniciar_servidor(logger, config_kernel.puerto_escucha);
	
	pthread_t manejo_consolas;
	pthread_create(&manejo_consolas, NULL, (void *) esperar_consolas, NULL);
	pthread_detach(manejo_consolas);
}

void esperar_consolas(){
	//thread para esperar clientes
	while(1){
		int socket = esperar_cliente(server_fd, logger);

		if (recibir_handshake(socket) == HANDSHAKE){
			enviar_handshake(socket);
			log_info(logger, "Handshake con Consola realizado.");
			
			pthread_t admitir_consolas;
			pthread_create(&admitir_consolas, NULL, (void *) recepcionar_proceso, (void *) &socket);
			pthread_detach(admitir_consolas);
		}else
			log_info(logger, "Ocurio un error al realizar el handshake con Consola.");

	}
}

void recibir_cde_de_cpu(){
	while(1){
		sem_wait(&bin2_recibir_cde);
		log_info(logger, "A punto de recibir buffer");;

		t_buffer* buffer = recibir_buffer(socket_cpu);

		log_info(logger, "Buffer Recibido");
		pthread_mutex_lock(&mutex_exec);
		pcb_en_ejecucion->cde = buffer_read_cde(buffer);
		pthread_mutex_unlock(&mutex_exec);
		log_info(logger, "Buffer leido");
	
		destruir_buffer_nuestro(buffer);
		log_info(logger, "Destrui buffer recibido");

		log_info(logger, "Proceso en ejecucion: %d", pcb_en_ejecucion->cde->pid);
		evaluar_instruccion(); // aca posteo necesito_enviar_cde
		sem_post(&bin1_envio_cde);
	}
}

void enviar_cde_a_cpu(){
	while(1){
		sem_wait(&necesito_enviar_cde);
		sem_wait(&bin1_envio_cde);
		t_buffer* buffer = crear_buffer_nuestro();
		buffer_write_cde(buffer, *(pcb_en_ejecucion->cde));
		enviar_buffer(buffer, socket_cpu);
		log_info(logger, "Envie cde a cpu");

		destruir_buffer_nuestro(buffer);
		log_info(logger, "Destrui buffer enviado");
		
		destruir_cde(pcb_en_ejecucion->cde);
		sem_post(&bin2_recibir_cde);
	}
}
// FIN UTILS CONEXIONES -----------------------------------------------------------------


// UTILS MOVIMIENTO PCB SEGUROS (MUTEX) -------------------------------------------------
t_pcb* retirar_pcb_de(t_queue* cola, pthread_mutex_t* mutex){
	t_pcb* pcb;
	pthread_mutex_lock(mutex);
	pcb = queue_pop(cola);
	pthread_mutex_unlock(mutex);
	return pcb;
}

void agregar_pcb_a(t_queue* cola, t_pcb* pcb_a_agregar, pthread_mutex_t* mutex){
	pthread_mutex_lock(mutex);
	queue_push(cola, pcb_a_agregar);
	pthread_mutex_unlock(mutex);
}
// FIN UTILS MOVIMIENTO PCB SEGUROS (MUTEX) ---------------------------------------------


// ------------------------------------ TRANSICIONES ------------------------------------
// TRANSICIONES CORTO PLAZO -------------------------------------------------------------
void enviar_de_ready_a_exec(){
	while(1){
		sem_wait(&cont_ready);
		sem_wait(&cont_procesador_libre);

		pthread_mutex_lock(&mutex_exec);
		pcb_en_ejecucion = retirar_pcb_de_ready_segun_algoritmo();
		pthread_mutex_unlock(&mutex_exec);

		sem_post(&cont_exec);
		log_info(logger, "PID: %d - Estado anterior: READY - Estado actual: EXEC", pcb_en_ejecucion->cde->pid); //OBLIGATORIO
		
		// Primer post de ese semaforo ya que se envia por primera vez
		sem_post(&necesito_enviar_cde);
	}
}

void enviar_de_exec_a_psedudoblock(char* razon){ // SOLO PARA WAIT, por ARCHIVOS y por (IO -> va a ser un hilo aparte)
	// razones: "nombre de recurso" || "nombre de archivo" |||||| io va aparte
	sem_wait(&cont_exec);
	
	log_info(logger, "PID: %d - Estado anterior: EXEC -	Estado actual: BLOCK", pcb_en_ejecucion->cde->pid); //OBLIGATORIO
	log_info("PID: %d -	Bloqueado por: %s", pcb_en_ejecucion->cde->pid, razon);
	
	pcb_en_ejecucion = NULL;
	
	sem_post(&cont_procesador_libre);
}

void enviar_de_exec_a_ready(){ // SOLO PARA YIELD (se llama al ejecutar yield)
	sem_wait(&cont_exec);
	agregar_pcb_a(procesosReady, pcb_en_ejecucion, &mutex_ready);
	
	log_info(logger, "PID: %d - Estado anterior: EXEC - Estado actual: READY", pcb_en_ejecucion->cde->pid); //OBLIGATORIO
	
	pcb_en_ejecucion = NULL;
	
	sem_post(&cont_procesador_libre);
	sem_post(&cont_ready);

	// log_info(logger, "Cola ready %s :");
	// mostrar_pids(pcb_a_ready->cde->lista_de_instrucciones);
}

void enviar_de_pseudoblock_a_ready(t_pcb* pcb){
	agregar_pcb_a(procesosReady, pcb, &mutex_ready);
	log_info(logger, "PID: %d - Estado anterior: BLOCK - Estado actual: READY", pcb->cde->pid); //OBLIGATORIO
	sem_post(&cont_ready);
}
void enviar_de_exec_a_exit(char* razon){
	sem_wait(&cont_exec);

	agregar_pcb_a(procesosExit, pcb_en_ejecucion, &mutex_exit);
	log_info(logger, "PID: %d -	Estado anterior: EXEC - Estado actual: EXIT", pcb_en_ejecucion->cde->pid); //OBLIGATORIO
	log_info(logger, "Finaliza el proceso %d - Motivo: %s", pcb_en_ejecucion->cde->pid, razon);
	
	pcb_en_ejecucion = NULL;
	
	sem_post(&cont_procesador_libre); // se libera el procesador
	sem_post(&cont_exit); // se agrega uno a procesosExit
	sem_post(&cont_grado_max_multiprog); // Habilita uno para que venga de NEW a READY
}
// FIN TRANSICIONES CORTO PLAZO ---------------------------------------------------------

// TRANSICIONES LARGO PLAZO -------------------------------------------------------------
void enviar_de_new_a_ready(){
	//while(1){
		sem_wait(&cont_new);
		sem_wait(&cont_grado_max_multiprog);
		// falta un semaforo por si procesosNew esta vacia
		t_pcb* pcb_a_ready = retirar_pcb_de(procesosNew, &mutex_new);
		
		// mandarle a memoria para que inicialize las estructuras necesarias, y tabla de segmentos para alm en pcb.
		
		//op_code inicio_memoria = INICIO_PROCESO_MEMORIA;
		//enviar_codigo(socket_memoria, inicio_memoria);

		//MEMORIA nos tiene que mandar la tabla de segmentos inicial para guardarla en el PCB !!!!
		agregar_pcb_a(procesosReady, pcb_a_ready, &mutex_ready);
		log_info(logger, "PID: %d - Estado anterior: NEW - Estado actual: READY", pcb_a_ready->cde->pid); //OBLIGATORIO
		sem_post(&cont_ready);

		
	//}
}

void recepcionar_proceso(void* arg){
	int* p = (int*) arg;
	int socket_a_atender = *p;

	// Recibe lista de instrucciones de la consola y arma el PCB
	t_buffer* buffer = recibir_buffer(socket_a_atender);

	t_list* lista_instrucciones = buffer_read_lista_instrucciones(buffer);

	destruir_buffer_nuestro(buffer);

	t_pcb* pcb_recibido = crear_pcb(lista_instrucciones, socket_a_atender);

	agregar_pcb_a(procesosNew, pcb_recibido, &mutex_new);
	log_info(logger, "Se crea el proceso %d en NEW", pcb_recibido->cde->pid);
	sem_post(&cont_new);
}

void terminar_procesos(){
	while(1){
		// Semaforo contador de cuantos procesos hay en exit
		sem_wait(&cont_exit);
		t_pcb* pcb = retirar_pcb_de(procesosExit, &mutex_exit);

		//liberar recursos asignados
		//liberar_todos_recursos(pcb);
		//avisar a memoria para liberar estructuras		
		//op_code aviso_de_fin_a_memoria = FIN_PROCESO_MEMORIA;
		//enviar_codigo(socket_memoria, aviso_de_fin_a_memoria);
		

		//avisar a consola para matar conexion
		op_code terminar_consola = FIN_PROCESO_CONSOLA;
		enviar_codigo(pcb->socket_consola, terminar_consola);

		log_info(logger, "PID Terminado: %d", pcb->cde->pid);
		destruir_pcb(pcb);
	}
}
// FIN TRANSICIONES LARGO PLAZO ---------------------------------------------------------
// ---------------------------------- FIN TRANSICIONES ----------------------------------

// PLANIFICADORES -----------------------------------------------------------------------
void planificadores(){
	planificadorLargoPlazo();
	planificadorCortoPlazo();
}

void planificadorLargoPlazo(){
	pthread_t hiloNew;
	pthread_t hiloExit;

	pthread_create(&hiloNew, NULL, (void *) enviar_de_new_a_ready, NULL);
	pthread_create(&hiloExit, NULL, (void *) terminar_procesos, NULL);

	pthread_detach(hiloNew);
	pthread_detach(hiloExit);
}

void planificadorCortoPlazo(){ 
	pthread_t poner_en_ejecucion;
	pthread_create(&poner_en_ejecucion, NULL, (void *) enviar_de_ready_a_exec, NULL);
	pthread_detach(poner_en_ejecucion);

	// Solo tiene 1 hilo el corto plazo
	// enviar_de_exec_a_pseudoblock se llama cuando se bloquea un proc
	// enviar_de_exec_a_ready solo para YIELD
	// enviar_de_pseudoblock_a_ready cuando se libera un recurso, etc (se desbloquea el proc)
	// enviar_de_exec_a_exit cuando finaliza x error o por EXIT ;)

}

// FIN PLANIFICADORES -------------------------------------------------------------------


// UTILS RECURSOS -----------------------------------------------------------------------
t_recurso* inicializar_recurso(char* nombre_recu, int instancias_tot){
	t_recurso* recurso = malloc(sizeof(t_recurso));
	int tam = 0;

	while(nombre_recu[tam] != NULL)
		tam++;

	recurso -> nombre_recurso  = malloc(tam);
	strcpy(recurso -> nombre_recurso, nombre_recu);

	recurso -> instancias_disponibles = instancias_tot;
	recurso -> cola_de_espera = queue_create();
	
	return recurso;
}

void inicializar_recurso_nulo(){
	recurso_nulo = inicializar_recurso("NULO", 0);
}

int asignar_instancia_recurso(char* nombre_recurso_a_asignar, t_pcb* pcb){
	t_recurso* recurso = encontrar_recurso_por_nombre(nombre_recurso_a_asignar);

	if(strcmp(recurso -> nombre_recurso, recurso_nulo -> nombre_recurso) == 0)
		return RECURSO_INVALIDO;
	// Si sigue de largo de este if(el de arriba) es porque es distinto de recurso_nulo

	// Por mas que no asigne, tengo que restar las instancias ya que seria como "reservarlo"	

	if(recurso->instancias_disponibles <= 0){
		list_add(recurso->cola_de_espera, pcb);
		recurso->instancias_disponibles--;
		// agregar en lista de pcb?
		return EN_ESPERA;
	}
	// Si sigue de largo de este if(el de arriba) es porque hay instancias disponibles

	recurso->instancias_disponibles--;
	int tam = 0;
	while(recurso -> nombre_recurso[tam] != NULL)
		tam++;

	// Necesito que &nombre != &(recurso -> nombre_recurso), porque despues hago free(nombre)
	char* nombre = malloc(tam);
	strcpy(nombre, recurso -> nombre_recurso);
	list_add(pcb->recursos_asignados, nombre);
	log_info(logger, "Wait: “PID: %d - Wait: %s - Instancias: %d", pcb -> cde -> pid, recurso -> nombre_recurso, recurso -> instancias_disponibles);
	return RECURSO_ASIGNADO;
}

int liberar_instancia_recurso(char* nombre_recurso_a_liberar, t_pcb* pcb){
	t_recurso* recurso = encontrar_recurso_por_nombre(nombre_recurso_a_liberar);

	// Suponemos que todos los recursos a liberar fueron asignados antes ya que no
	// aclara que sucederia en caso de que se quiera liberar un recurso que no
	// fue asignado con anterioridad

	if(strcmp(recurso -> nombre_recurso, recurso_nulo -> nombre_recurso) == 0)
		return RECURSO_INVALIDO;
	else{
		recurso->instancias_disponibles++;
		sacar_recurso(pcb -> recursos_asignados, nombre_recurso_a_liberar);
		
		// En caso de que corresponda, desbloquea al primer proceso de la cola de bloqueados de ese recurso
		if(recurso->instancias_disponibles < 0){
			t_pcb* pcb_a_desbloquear = queue_pop(recurso -> cola_de_espera);
			asignar_instancia_recurso(nombre_recurso_a_liberar, pcb_a_desbloquear);
			agregar_pcb_a(procesosReady, pcb_a_desbloquear, &mutex_ready);
		}
		log_info(logger, "Wait: “PID: %d - Signal: %s - Instancias: %d", pcb -> cde -> pid, recurso -> nombre_recurso, recurso -> instancias_disponibles);
		return RECURSO_LIBERADO;
	}
}

int sacar_recurso(t_list* recursos_asignados, char* recurso_a_sacar){
	for(int i = 0; i < list_size(recursos_asignados); i++){
		char* recurso = list_get(recursos_asignados, i);
		if(strcmp(recurso, recurso_a_sacar) == 0){
			recurso = list_remove(recursos_asignados, i);
			free(recurso);
			return RECURSO_LIBERADO;
		}
	}
}

void liberar_todos_recursos(t_pcb* pcb){
	int cant_rec_asignados = list_size(pcb -> recursos_asignados);
	for(int i = 0; i < cant_rec_asignados; i++){
		char* recurso_a_liberar = list_get(pcb -> recursos_asignados, i);
		liberar_instancia_recurso(recurso_a_liberar, pcb);
	}
}

t_recurso* encontrar_recurso_por_nombre(char* nombre_recurso_a_obtener){
	// En caso de que no exista el recurso (si sale del for) devuelve el recurso_nulo
	for(int i=0; i< list_size(config_kernel.recursos) ; i++){
		t_recurso* recurso = list_get(config_kernel.recursos,i);
		if(strcmp(nombre_recurso_a_obtener, recurso->nombre_recurso) == 0)
			return recurso;
	}
	return recurso_nulo;
}

// FIN UTILS RECURSOS -------------------------------------------------------------------


// UTILS PARA SACAR DE READY ------------------------------------------------------------
t_pcb* elegido_por_FIFO(){
	return retirar_pcb_de(procesosReady, &mutex_ready);
}

double calcular_response_ratio(t_pcb *pcb){
	double wait_time = -1; // cuanto tiempo espero en ready
	double expected_next_round = -1; // cuanto tiempo se estima que va a tardar la proxima rafaga de ejecucion
	double rr = -1; // response ratio

	double tiempo_actual = (double)time(NULL);

	wait_time = 

	// rr = (wait_time + expected_next_round) / expected_next_round

	wait_time = tiempo_actual - pcb->tiempo_llegada_ready;

	return rr;
}

bool maximo_response_ratio(t_pcb* pcb1, t_pcb* pcb2){
	if (calcular_response_ratio(pcb1) > calcular_response_ratio(pcb2))
		return true;
	else
		return false;
}

t_pcb* elegido_por_HRRN(){
	pthread_mutex_lock(&mutex_ready);
	list_sort(procesosReady->elements, (void*)maximo_response_ratio);
	pthread_mutex_unlock(&mutex_ready);

	t_pcb* pcb_elegido = retirar_pcb_de(procesosReady, &mutex_ready);
	
	return pcb_elegido;
}

t_pcb* retirar_pcb_de_ready_segun_algoritmo(){
	if (strcmp(config_kernel.algoritmo, "FIFO") == 0)
		return elegido_por_FIFO();
	else if(strcmp(config_kernel.algoritmo, "HRRN") == 0)
		return elegido_por_HRRN();
	
	log_info(logger, "Algoritmo no reconocido.");
	exit(3);
}
// FIN UTILS PARA SACAR DE READY --------------------------------------------------------


// UTLIS CREAR --------------------------------------------------------------------------
t_cde* crear_cde(t_list* instrucciones){
	t_cde* cde = malloc(sizeof(t_cde));

	pthread_mutex_lock(&mutex_pid_a_asignar);
	cde->pid = pid;
	pid++;
	pthread_mutex_unlock(&mutex_pid_a_asignar);
	
	cde->lista_de_instrucciones = instrucciones;
	cde->program_counter = 0;
	cde->registros_cpu = inicializar_registros();
    cde->tabla_segmentos = list_create();

	return cde;
}

t_pcb* crear_pcb(t_list *instrucciones, int socket_con){
	t_pcb* pcb = malloc(sizeof(t_pcb));
	
	pcb->cde = crear_cde(instrucciones);
	
	pcb->estimado_prox_rafaga = config_kernel.est_inicial;
	pcb->tiempo_llegada_ready = -1;

	pcb->archivos_abiertos = list_create();
	pcb->recursos_asignados = list_create();
	pcb->socket_consola = socket_con;
	return pcb;
}


t_registros inicializar_registros(){
	t_registros registros;

	for(int i=0;i<4;i++){
		registros.AX[i] = 'x';
		registros.BX[i] = 'x';
		registros.CX[i] = 'x';
		registros.DX[i] = 'x';
	}

	registros.AX[4] = '\0';
	registros.BX[4] = '\0';
	registros.CX[4] = '\0';
	registros.DX[4] = '\0';

	
	for(int i=0;i<8;i++){
		registros.EAX[i] = 'x';
		registros.EBX[i] = 'x';
		registros.ECX[i] = 'x';
		registros.EDX[i] = 'x';
	}
	registros.EAX[8] = '\0';
	registros.EBX[8] = '\0';
	registros.ECX[8] = '\0';
	registros.EDX[8] = '\0';


	for(int i=0;i<16;i++){
		registros.RAX[i] = 'x';
		registros.RBX[i] = 'x';
		registros.RCX[i] = 'x';
		registros.RDX[i] = 'x';
	}
	registros.RAX[16] = '\0';
	registros.RBX[16] = '\0';
	registros.RCX[16] = '\0';
	registros.RDX[16] = '\0';

	return registros;
}
// FIN UTILS CREAR ----------------------------------------------------------------------


// UTILS INSTRUCCIONES ------------------------------------------------------------------
void evaluar_instruccion(){
	// La evalua cuando vuelve de CPU
	int indice = pcb_en_ejecucion->cde->program_counter - 1;
	t_list* lista_instrucciones = pcb_en_ejecucion->cde->lista_de_instrucciones;
	t_instruction* instruccion_actual = list_get(lista_instrucciones, indice);
	switch(instruccion_actual->instruccion){
		case F_READ:
			break;
		case F_WRITE:
			break;
		case F_TRUNCATE:
			break;
		case F_SEEK:
			break;
		case CREATE_SEGMENT:
			break;
		case IO:
			log_info(logger,"PID: %d - Bloqueado por: %s", pcb_en_ejecucion->cde->pid, "IO");
			log_info(logger,"PID: %d - Ejecuta IO: %d", pcb_en_ejecucion->cde-> pid, instruccion_actual->numero1);
			administrar_io(pcb_en_ejecucion, instruccion_actual->numero1);
			break;
		case WAIT:
			switch(asignar_instancia_recurso(instruccion_actual -> string1, pcb_en_ejecucion)){
				case RECURSO_INVALIDO:
					enviar_de_exec_a_exit("INVALID_RESOURCE");
					break;
				case EN_ESPERA:
					enviar_de_exec_a_psedudoblock(instruccion_actual -> string1);
					break;
				case RECURSO_ASIGNADO:
					// Lo unico que tendria que hacer es el log_info que esta adentro de asignar_instancia_recurso
					sem_post(&necesito_enviar_cde);
					break;
			}
			break;
		case SIGNAL:
			switch(liberar_instancia_recurso(instruccion_actual -> string1, pcb_en_ejecucion)){
				case RECURSO_INVALIDO:
					enviar_de_exec_a_exit("INVALID_RESOURCE");
					break;
				case RECURSO_LIBERADO:
					// Lo unico que tendria que hacer es el log_info que esta adentro de liberar_instancia_recurso
					sem_post(&necesito_enviar_cde);
					break;
			}
			break;
		case F_OPEN:
			break;
		case F_CLOSE:
			break;
		case DELETE_SEGMENT:
			break;
		case YIELD:
			log_info(logger, "Entre a yield");
			enviar_de_exec_a_ready();
			break;
		case EXIT:
			enviar_de_exec_a_exit("SUCCES");
			break;
		default:
			log_info(logger, "Error: Instruccion no reconocida.");
			break;
	}
}

void administrar_io(t_pcb* pcb_a_dormir, int tiempo_siesta){
	t_io* datos_io = malloc(sizeof(t_io));

	datos_io -> pcb = pcb_a_dormir;
	datos_io -> tiempo = tiempo_siesta;

	pthread_t bloquear_proceso_por_io;
	pthread_create(&bloquear_proceso_por_io, NULL, (void *) dormir_proceso, (void*) datos_io);
	pthread_detach(bloquear_proceso_por_io);
}

void dormir_proceso(void* args){
	t_io* datos_io = (t_io*) args;
	enviar_de_exec_a_psedudoblock("IO");
	usleep(datos_io->tiempo * 1000000);	
	enviar_de_pseudoblock_a_ready(datos_io -> pcb);
}

// FIN UTILS INSTRUCCIONES --------------------------------------------------------------

#include "../includes/kernel.h"

int main(void){
	levantar_modulo();
	
	pthread_t t;
	pthread_create(&t, NULL, (void *) enviar_de_new_a_ready, NULL);
	pthread_detach(t);

	pthread_t t2;
	pthread_create(&t2, NULL, (void *) enviar_de_ready_a_exec, NULL);
	pthread_detach(t2);

	pthread_t t3;
	pthread_create(&t3, NULL, (void *) terminar_procesos, NULL);
	pthread_detach(t3);
	
	// usleep(5 * 1000000);

	enviar_de_exec_a_exit("PID1");
	enviar_de_exec_a_exit("PID2");
	enviar_de_exec_a_exit("PID3");
	enviar_de_exec_a_exit("PID4");
	enviar_de_exec_a_exit("PID5");
	enviar_de_exec_a_exit("PID6");
	
	log_info(logger, "Cant de consolas en ready: %d", queue_size(procesosReady));
	log_info(logger, "Cant de consolas en new: %d", list_size(procesosNew -> elements));
	
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
	// Este haria el ida y vuelta del CDE
	t_buffer* buffer = crear_buffer_nuestro();
	while(recv(socket_cpu, &(buffer -> codigo), sizeof(uint8_t), MSG_WAITALL) > 0){
		
	}

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

	/*
	socket_cpu = crear_conexion(config_kernel.ip_cpu, config_kernel.puerto_cpu);
	if (socket_cpu >= 0){			
		log_info(logger,"Conectado con cpu");

		enviar_handshake(socket_cpu);
		if (recibir_handshake(socket_cpu) == HANDSHAKE){
			log_info(logger, "Handshake con CPU realizado.");
			pthread_t conexion_cpu;
			pthread_create(&conexion_cpu, NULL, (void *) manejar_conexion_con_cpu, NULL);
			pthread_detach(conexion_cpu);
			
		}else
			log_info(logger, "Ocurio un error al realizar el handshake con CPU.");
	}
	else
		log_info(logger,"Error al conectar con cpu");
	*/
	

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
		t_pcb* pcb_a_exec = retirar_pcb_de_ready_segun_algoritmo();
		agregar_pcb_a(procesosExec, pcb_a_exec, &mutex_exec);
		sem_post(&cont_exec);
		log_info(logger, "PID: %d - Estado anterior: READY - Estado actual: EXEC", pcb_a_exec->cde->pid); //OBLIGATORIO
		
		// Falta enviar CDE a CPU

	}
}

void enviar_de_exec_a_psedudoblock(char* razon){ // SOLO PARA WAIT, por ARCHIVOS y por (IO -> va a ser un hilo aparte)
	// razones: "nombre de recurso" || "nombre de archivo" |||||| io va aparte
	t_pcb* pcb_a_blocked = retirar_pcb_de(procesosExec, &mutex_blocked);
	log_info(logger, "PID: %d - Estado anterior: EXEC -	Estado actual: BLOCK", pcb_a_blocked->cde->pid); //OBLIGATORIO
	log_info("PID: %d -	Bloqueado por: %s", pcb_a_blocked->cde->pid, razon);
}

void enviar_de_exec_a_ready(){ // SOLO PARA YIELD (se llama al ejecutar yield)
	t_pcb* pcb_a_ready = retirar_pcb_de(procesosExec, &mutex_exec);
	sem_post(&cont_procesador_libre);
	agregar_pcb_a(procesosReady, pcb_a_ready, &mutex_exec);
	sem_post(&cont_ready);
	// log_info(logger, "Cola ready %s :");
	// mostrar_pids(pcb_a_ready->cde->lista_de_instrucciones);
}

void enviar_de_pseudoblock_a_ready(t_pcb* pcb){
		agregar_pcb_a(procesosReady, pcb, &mutex_ready);
		log_info(logger, "PID: %d -	Estado anterior: BLOCK - Estado actual: READY", pcb->cde->pid); //OBLIGATORIO
		sem_post(&cont_ready);
}

void enviar_de_exec_a_exit(char* razon){
	sem_wait(&cont_exec);
	t_pcb* pcb_retirado = retirar_pcb_de(procesosExec, &mutex_exec);
	sem_post(&cont_procesador_libre); // se libera el procesador

	agregar_pcb_a(procesosExit, pcb_retirado, &mutex_exit);
	log_info(logger, "PID: %d -	Estado anterior: EXEC - Estado actual: EXIT", pcb_retirado->cde->pid); //OBLIGATORIO
	log_info(logger, "Finaliza el proceso %d - Motivo: %s", pcb_retirado->cde->pid, razon);
	sem_post(&cont_exit); // se agrega uno a procesosExit
	sem_post(&cont_grado_max_multiprog);
}
// FIN TRANSICIONES CORTO PLAZO ---------------------------------------------------------

// TRANSICIONES LARGO PLAZO -------------------------------------------------------------
void enviar_de_new_a_ready(){
	while(1){
		sem_wait(&cont_new);
		sem_wait(&cont_grado_max_multiprog);
		// falta un semaforo por si procesosNew esta vacia
		t_pcb* pcb_a_ready = retirar_pcb_de(procesosNew, &mutex_new);
		
		// mandarle a memoria para que inicialize las estructuras necesarias, y tabla de segmentos para alm en pcb.
		
		//op_code inicio_memoria = INICIO_PROCESO_MEMORIA;
		//enviar_codigo(socket_memoria, inicio_memoria);

		//MEMORIA nos tiene que mandar la tabla de segmentos inicial para guardarla en el PCB !!!!
		agregar_pcb_a(procesosReady, pcb_a_ready, &mutex_ready);
		log_info(logger, "PID: %d -	Estado anterior: NEW - Estado actual: READY", pcb_a_ready->cde->pid); //OBLIGATORIO
		sem_post(&cont_ready);

		
	}
}

void recepcionar_proceso(void* arg){
	int* p = (int*) arg;
	int socket_a_atender = *p;

	
	// Recibe lista de instrucciones de la consola y arma el PCB
	t_buffer* buffer = recibir_buffer(socket_a_atender);

	t_list* lista_instrucciones = buffer_read_lista_instrucciones(buffer);
	/*
	log_info(logger, "Mostrando instrucciones recibidas:");
	mostrar_instrucciones(logger, lista_instrucciones);
	*/

	t_pcb* pcb_recibido = crear_pcb(lista_instrucciones, socket_a_atender);

	agregar_pcb_a(procesosNew, pcb_recibido, &mutex_new);
	log_info(logger, "Se crea el proceso %d en NEW", pcb_recibido->cde->pid);
	sem_post(&cont_new);
}

void terminar_procesos(){
	//Verificar todo el tiempo si hay procesos en la lista de finalizados (x semaforos)
	while(1){
		// Semaforo contador de cuantos procesos hay en exit
		sem_wait(&cont_exit);
		t_pcb* pcb = retirar_pcb_de(procesosExit, &mutex_exit);

		//liberar recursos asignados
		//liberar_todos_recursos(pcb);
		//avisar a memoria para liberar estructuras		
		//op_code aviso_de_fin_a_memoria = FIN_PROCESO_MEMORIA;
		//enviar_codigo(socket_memoria, aviso_de_fin_a_memoria);
		
		log_info(logger, "PID Terminado: %d", pcb->cde->pid);

		//avisar a consola para matar conexion
		op_code terminar_consola = FIN_PROCESO_CONSOLA;
		//enviar_codigo(pcb->socket_consola, terminar_consola);


		//destruir_pcb(pcb);
	}
}
// FIN TRANSICIONES LARGO PLAZO ---------------------------------------------------------
// ---------------------------------- FIN TRANSICIONES ----------------------------------


// PLANIFICADORES -----------------------------------------------------------------------
void planificadorCortoPlazo(){ 
	pthread_t poner_en_ejecucion;
	pthread_create(&poner_en_ejecucion, NULL, (void *) enviar_de_ready_a_exec, NULL);
	pthread_detach(poner_en_ejecucion);

	// hilo para enviar_de_exec_a_block()??
	
	// hilo para enviar_de_exec_a_ready()?? SOLO PARA YIELD

	// hilo para enviar_de_block_a_ready()
	/*
	pthread_t poner_en_listo;
	pthread_create(&poner_en_listo, NULL, (void *) enviar_de_block_a_ready, NULL);
	pthread_detach(poner_en_listo);
	*/
	pthread_t matar_procesos;
	pthread_create(&matar_procesos, NULL, (void *) terminar_procesos, NULL);
	pthread_detach(matar_procesos);
}

void planificadorLargoPlazo(){
	pthread_t hiloNew;
	pthread_t hiloExit;
	pthread_create(&hiloNew, NULL, (void *) enviar_de_new_a_ready, NULL);
	pthread_create(&hiloExit, NULL, (void *) terminar_procesos, NULL);

	pthread_detach(hiloNew);
	pthread_detach(hiloExit);
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

void desbloquear_proceso(char* recurso_libearado){

}

// FIN UTILS RECURSOS -------------------------------------------------------------------


// UTILS PARA SACAR DE READY ------------------------------------------------------------
t_pcb* elegido_por_FIFO(){
	return retirar_pcb_de(procesosReady, &mutex_ready);
}

t_pcb* elegido_por_HRRN(){
	t_pcb* pcb;
	
	return pcb;
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
	
	for(int i=0;i<8;i++){
		registros.EAX[i] = 'x';
		registros.EBX[i] = 'x';
		registros.ECX[i] = 'x';
		registros.EDX[i] = 'x';
	}

	for(int i=0;i<16;i++){
		registros.RAX[i] = 'x';
		registros.RBX[i] = 'x';
		registros.RCX[i] = 'x';
		registros.RDX[i] = 'x';
	}

	return registros;
}
// FIN UTILS CREAR ----------------------------------------------------------------------



void ejecutarProceso(){
	// Arma el cde
	// Se lo envia a CPU
	// CPU ejecuta instrucciones y si necesita devuelve el CDE (do while de ulima instruccion != exit)
	// Cuando recibe el CDE, evalua la instruccion actual
	// Si la instruccion actual bloquea el proceso, lo saca de la colaExec
	// Llama al subprograma que bloquea.
	return;
}

// UTILS INSTRUCCIONES ------------------------------------------------------------------
void evaluar_instruccion(t_instruction* instruccion_actual, t_pcb* pcb){
	// La evalua cuando vuelve de CPU
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
			log_info(logger,"PID: %d - Bloqueado por: %s", pcb->cde->pid, "IO");
			log_info(logger,"PID: %d - Ejecuta IO: %d", pcb->cde-> pid, instruccion_actual->numero1);
			administrar_io(pcb, instruccion_actual->numero1);
			break;
		case WAIT:
			switch(asignar_instancia_recurso(instruccion_actual -> string1, pcb)){
				case RECURSO_INVALIDO:
					enviar_de_exec_a_exit("INVALID_RESOURCE");
					break;
				case EN_ESPERA:
					enviar_de_exec_a_psedudoblock(instruccion_actual -> string1);
					break;
				case RECURSO_ASIGNADO:
					// Lo unico que tendria que hacer es el log_info que esta adentro de asignar_instancia_recurso
					break;
			}
			break;
		case SIGNAL:
			switch(liberar_instancia_recurso(instruccion_actual -> string1, pcb)){
				case RECURSO_INVALIDO:
					enviar_de_exec_a_exit("INVALID_RESOURCE");
					break;
				case RECURSO_LIBERADO:
					// Lo unico que tendria que hacer es el log_info que esta adentro de liberar_instancia_recurso
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
			enviar_de_exec_a_ready();
			break;
		case EXIT:
			enviar_de_exec_a_exit("SUCCES");
			break;
		default:
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

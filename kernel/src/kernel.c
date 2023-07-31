#include "../includes/kernel.h"

int main(void){
	levantar_modulo();

	
	pthread_t matar_procesos;
	pthread_create(&matar_procesos, NULL, (void *) terminar_procesos, NULL);
	pthread_detach(matar_procesos);
	

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
	pthread_mutex_init(&mutex_new, NULL);
	pthread_mutex_init(&mutex_ready, NULL);
	pthread_mutex_init(&mutex_blocked, NULL);
	pthread_mutex_init(&mutex_exec, NULL);
	pthread_mutex_init(&mutex_exit, NULL);
	
	sem_init(&cont_grado_max_multiprog, 0, config_kernel.grado_max_multi); //para no acpetar mas procesos de los q permite el grado max de multi

	sem_init(&cont_exit, 0, 0); //para postear cuando uno entra a exit y poder matar su consola

	sem_init(&cont_procesador_libre, 0, 1);
	//Semaforos conexiones y recibos/envios
	//sem_init(&conexion_consola, 0, 0);
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
	int a = 0;

	config_kernel.recursos_compartidos = list_create();
	config_kernel.instancias = list_create();

	while(recursos[a]!= NULL){
		list_add(config_kernel.recursos_compartidos,recursos[a]);
		int num = strtol(instancias[a],NULL,10);
		list_add(config_kernel.instancias,num);
		a++;
	}
}
// FIN UTILS INICIAR MODULO -------------------------------------------------------------


// UTILS CONEXIONES ---------------------------------------------------------------------
void manejar_conexion_con_memoria(){

}

void manejar_conexion_con_cpu(){
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
		pthread_t conexion_cpu;
		pthread_create(&conexion_cpu, NULL, (void *) manejar_conexion_con_cpu, NULL);
		pthread_detach(conexion_cpu);
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
	//thread para espersar clientes
	while(1){
		int socket = esperar_cliente(server_fd, logger);

		pthread_t t;
		pthread_create(&t, NULL, (void *) recepcionar_proceso, (void *) &socket);
		pthread_detach(t);
	}
}
// FIN UTILS CONEXIONES -----------------------------------------------------------------

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


// ------------------------------------ TRANSICIONES ------------------------------------
// TRANSICIONES CORTO PLAZO -------------------------------------------------------------
void enviar_de_ready_a_exec(){
	while(1){
		sem_wait(&cont_procesador_libre);
		t_pcb* pcb_a_exec = retirar_pcb_de_ready_segun_algoritmo();
		agregar_pcb_a(procesosExec, pcb_a_exec, &mutex_exec);
		
		log_info(logger, "PID: %d	-	Estado anterior: READY	-	Estado actual: EXEC", pcb_a_exec->cde->pid); //OBLIGATORIO

	}
}

void enviar_de_exec_a_block(char* razon){ // SOLO PARA WAIT, por ARCHIVOS y por (IO -> va a ser un hilo aparte)
	// razones: "nombre de recurso" || "nombre de archivo" |||||| io va aparte
	t_pcb* pcb_a_blocked = retirar_pcb_de(procesosExec, &mutex_blocked);
	agregar_pcb_a(procesosBlocked, pcb_a_blocked, &mutex_blocked);
	log_info(logger, "PID: %d	-	Estado anterior: EXEC	-	Estado actual: BLOCK", pcb_a_blocked->cde->pid); //OBLIGATORIO

	log_info("PID: %d	-	Bloqueado por: %s", pcb_a_blocked->cde->pid, razon);
}

void enviar_de_exec_a_ready(){ // SOLO PARA YIELD (se llama al ejecutar yield)
	t_pcb* pcb_a_ready = retirar_pcb_de(procesosExec, &mutex_exec);
	sem_post(&cont_procesador_libre);
	agregar_pcb_a(procesosReady, pcb_a_ready, &mutex_exec);
	log_info(logger, "Cola ready %s :");
	//mostrar_pids(pcb_a_ready->cde->lista_de_instrucciones);
}

void enviar_de_block_a_ready(){
	while(1){
		// 
	}
}

void enviar_de_exec_a_exit(char* razon){
	t_pcb* pcb_retirado = retirar_pcb_de(procesosExec, &mutex_exec);
	sem_post(&cont_procesador_libre); // se libera el procesador

	agregar_pcb_a(procesosExit, pcb_retirado, &mutex_exit);
	sem_post(&cont_exit); // se agrega uno a procesosExit
	sem_post(&cont_grado_max_multiprog);
	log_info(logger, "Finaliza el proceso %d - Motivo: %s", pcb_retirado->cde->pid, razon);
}
// FIN TRANSICIONES CORTO PLAZO ---------------------------------------------------------

// TRANSICIONES LARGO PLAZO -------------------------------------------------------------
void enviar_de_new_a_ready(){
	while(1){
		sem_wait(&cont_grado_max_multiprog);
		t_pcb* pcb_a_ready = retirar_pcb_de(procesosNew, &mutex_new);
		
		// mandarle a memoria para que inicialize las estructuras necesarias, y tabla de segmentos para alm en pcb.
		//op_code inicio_memoria = INICIO_PROCESO_MEMORIA;
		//enviar_codigo(socket_memoria, inicio_memoria);

		//MEMORIA nos tiene que mandar la tabla de segmentos inicial para guardarla en el PCB !!!!
		agregar_pcb_a(procesosReady, pcb_a_ready, &mutex_ready);
		
	}
}

void recepcionar_proceso(void* arg){
	int* p = (int*) arg;
	int socket_a_atender = *p;

	
	// Recibe lista de instrucciones de la consola y arma el PCB
	t_buffer* buffer = recibir_buffer(socket_a_atender);

	t_list* lista_instrucciones = buffer_read_lista_instrucciones(buffer);

	log_info(logger, "Mostrando instrucciones recibidas:");

	mostrar_instrucciones(logger, lista_instrucciones);

	//Crear pcb
	t_pcb* pcb_recibido = crear_pcb(lista_instrucciones, socket_a_atender);

	//Agregarlo a new
	agregar_pcb_a(procesosNew, pcb_recibido, &mutex_new);
	log_info(logger, "Se crea el proceso %d en NEW", pcb_recibido->cde->pid);
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
		
		//enviar_codigo(socket_memoria, FIN_PROCESO_MEMORIA);
		//avisar a consola para matar conexion
		op_code terminar_consola = FIN_PROCESO_CONSOLA;
		enviar_codigo(pcb->socket_consola, terminar_consola);


		// Hacer free de todos los malloc (PCB Y CDE)
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
	pthread_t poner_en_listo;
	pthread_create(&poner_en_listo, NULL, (void *) enviar_de_block_a_ready, NULL);
	pthread_detach(poner_en_listo);
	
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
void restar_instancia_recurso(char* recurso){
	actualizar_instancias_recurso(recurso, -1);
}

void sumar_instancia_recurso(char* recurso){
	actualizar_instancias_recurso(recurso, 1);
}

void liberar_todos_recursos(t_pcb* pcb){
	int cant_recursos = list_size(pcb -> recursos_asignados);

	for (int i=0; i < cant_recursos; i++){
		char* recurso = list_remove(pcb -> recursos_asignados, i);
		sumar_instancia_recurso(recurso);

	}
}
// FIN UTILS RECURSOS -------------------------------------------------------------------

t_pcb* elegido_por_FIFO(){
	t_pcb* pcb;

	return pcb;
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


// UTLIS CREAR --------------------------------------------------------------------------
t_cde* crear_cde(t_list* instrucciones){
	t_cde* cde = malloc(sizeof(t_cde));

	cde->pid = pid;
	cde->lista_de_instrucciones = instrucciones;
	cde->program_counter = 0;
	cde->registros_cpu = inicializar_registros();
    cde->tabla_segmentos = list_create();


	return cde;
}

t_pcb* crear_pcb(t_list *instrucciones, int socket_con){
	t_pcb* pcb = malloc(sizeof(t_pcb));
	
	pcb->cde = crear_cde(instrucciones);
	pid++;

	pcb->estimado_prox_rafaga = config_kernel.est_inicial;
	pcb->tiempo_llegada_ready = -1;

	pcb->archivos_abiertos = list_create();
	pcb->recursos_asignados = list_create();
	pcb->recursos_solicitados = list_create();
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


// UTILS DESTRUIR (NO PROBADOS) -----------------------------------------------------------------------
void destruir_cde(t_cde* cde){	
	list_destroy(cde -> lista_de_instrucciones);
	list_destroy(cde -> tabla_segmentos);
	free(cde);
}

void destruir_pcb(t_pcb* pcb){
	destruir_cde(pcb -> cde);
	list_destroy(pcb -> recursos_asignados);
	list_destroy(pcb -> recursos_solicitados);
	list_destroy(pcb -> archivos_abiertos);
	free(pcb);
}

void destruir_instrucciones(t_instruction* instruccion){
	free(instruccion -> string1);
	free(instruccion -> string2);
	free(instruccion);
}
// FIN UTILS DESTRUIR -------------------------------------------------------------------

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
void evaluar_instruccion(t_instruction* instruccion_actual, t_cde cde_recibido){
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
			log_info(logger,"PID: %d - Ejecuta IO: %d",cde_recibido.pid,instruccion_actual->numero1);
			break;
		case WAIT:
			break;
		case SIGNAL:
			// Primero si existe -> no existe => proceso a exit
				// Si existe => sumo uno a las instancias
					// 
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

void actualizar_instancias_recurso(char* recurso_a_actualizar, int numero){
	// Paso el numero para poder hacer +1 y -1 por wait y signal
	// Aca no hay chequeos, solo cambia la cantidad de instancias del recurso
	char* recurso;
	int* instancia;

	for(int i=0; i< list_size(config_kernel.recursos_compartidos) ; i++){ 
		recurso = list_get(config_kernel.recursos_compartidos,i);
		if(strcmp(recurso, recurso_a_actualizar) == 0){
			instancia = list_get(config_kernel.instancias,i) + numero;
			list_replace(config_kernel.instancias, i, instancia);
		}
	}
}

int obtener_instancia_recurso(char* recurso_a_obtener){
	char* recurso;

	for(int i=0; i< list_size(config_kernel.recursos_compartidos) ; i++){
		recurso = list_get(config_kernel.recursos_compartidos,i);
		if(strcmp(recurso, recurso_a_obtener) == 0)
			return list_get(config_kernel.instancias, i);
	}
	return -1;
}

// FIN UTILS INSTRUCCIONES --------------------------------------------------------------

#include "../includes/kernel.h"

int main(void){
	levantar_modulo();

	pthread_t matar_procesos;
	pthread_create(&matar_procesos, NULL, (void *) terminar_procesos, NULL);
	pthread_detach(matar_procesos);

	server_fd = iniciar_servidor(logger, config_kernel.puerto_escucha);

	consola_fd = esperar_cliente(server_fd, logger);
	
	t_buffer* buffer = crear_buffer_nuestro();

	// Enviamos el codigo de operacion
    log_info(logger, "A punto de recibir el codigo de operacion");
    recv(consola_fd, &(buffer->codigo), sizeof(uint8_t), MSG_WAITALL);
    log_info(logger, "Recibi el codigo de op: %d", buffer -> codigo);

    // Enviamos el tamanio del buffer
    log_info(logger, "A punto de recibir el tamanio del buffer");
    recv(consola_fd, &(buffer->size), sizeof(uint32_t), MSG_WAITALL);
    log_info(logger, "Recibi el tamanio del buffer: %d", buffer -> size);


	buffer -> stream = malloc(buffer -> size);
    // Enviamos el stream del buffer
    log_info(logger, "A punto de recibir el stream del buffer");
    recv(consola_fd, buffer->stream, buffer -> size, MSG_WAITALL);
    log_info(logger, "Recibi el stream del buffer");

	
	t_list* lista_instrucciones = buffer_read_lista_instrucciones(buffer);

	mostrar_instrucciones(logger, lista_instrucciones);

	t_pcb* pcb_prueba = crear_pcb(lista_instrucciones, consola_fd);
	
	log_info(logger, "Por agregar pcb a exit");
	agregar_pcb_a(procesosExit, pcb_prueba, &mutex_exit);
	sem_post(&cont_exit);
	log_info(logger, "pcb agregado a exit");

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

	//establecer_conexiones();
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
	
	//FALTA INICIALIZAR ALGUNOS
	sem_init(&cont_exit, 0, 0);

	//Semaforos conexiones y recibos/envios
	sem_init(&conexion_consola, 0, 0);
}

void finalizar_modulo(){
	log_destroy(logger);
	config_destroy(config);
	return;
}

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


void conectarse_con_memoria(){
	socket_memoria = crear_conexion(config_kernel.ip_memoria, config_kernel.puerto_memoria);
	if (socket_memoria >= 0)
		log_info(logger,"Conectado con memoria");
	else
	{
		//perror(socket_memoria);
		log_info(logger,"Error al conectar con memoria");
	}
}


void conectarse_con_cpu(){
	socket_cpu = crear_conexion(config_kernel.ip_cpu, config_kernel.puerto_cpu);
	if (socket_cpu >= 0){			
		log_info(logger,"Conectado con cpu");
		//sem_post(&prueba1); verificar dsps este semaforo
	}
	else
		log_info(logger,"Error al conectar con cpu");
}


void conectarse_con_file_system(){
	socket_file_system = crear_conexion(config_kernel.ip_file_system, config_kernel.puerto_file_system);
	if (socket_file_system >= 0)
			log_info(logger,"Conectado con file system");
		else
			log_info(logger,"Error al conectar con file system");
}

void establecer_conexiones()
{
	/*
	pthread_t conexion_memoria;
	pthread_create(&conexion_memoria, NULL, (void *) conectarse_con_memoria, NULL);
	pthread_detach(conexion_memoria);

	*/

	/*
	pthread_t conexion_cpu;
	pthread_create(&conexion_cpu, NULL, (void *) conectarse_con_cpu, NULL);
	pthread_detach(conexion_cpu);
	*/

	/*
	pthread_t conexion_file_system;
	pthread_create(&conexion_file_system, NULL, (void *) conectarse_con_file_system, NULL);
	pthread_detach(conexion_file_system);
	*/


	server_fd = iniciar_servidor(logger, config_kernel.puerto_escucha);
	
	pthread_t manejo_consolas;
	pthread_create(&manejo_consolas, NULL, (void *) manejar_clientes, (void *) server_fd);
	pthread_detach(manejo_consolas);


}

void manejar_clientes(int server_fd){
	//thread para espersar clientes
	while(1){
		//t_conexiones conexiones;
		int socket = esperar_cliente(server_fd, logger);
		//conexiones.socket_anterior = 0;
		// threads para recepcion de info de las consolas

		pthread_t t;
		pthread_create(&t, NULL, (void *) recepcionar_proceso, (void *) &socket);
		pthread_detach(t);
	}

}

void recepcionar_proceso(int socket_a_atender){
	//Recibir, deserializar lista de instrucciones de consola
	t_buffer* buffer = crear_buffer_nuestro();

	log_info(logger, "Por recibir el tamanio");
	recv(socket_a_atender, &(buffer->size), sizeof(uint32_t), MSG_WAITALL);
	log_info(logger, "Ya recibi el tamanio");

	buffer->stream = malloc(buffer->size);

	log_info(logger, "Por recibir el stream.");
	recv(socket_a_atender, buffer->stream, buffer->size, MSG_WAITALL);
	log_info(logger, "Recibi el stream.");

	t_list* lista_instrucciones = buffer_read_lista_instrucciones(buffer);

	log_info(logger, "Mostrando instrucciones recibidas:");

	for(int i = 0; i < list_size(lista_instrucciones); i++){
		t_instruction* instruccion = list_get(lista_instrucciones, i);
		log_info(logger, "Codigo instruccion: %d", instruccion->instruccion);
	}

	//Crear pcb
	t_pcb* pcb_recibido = crear_pcb(lista_instrucciones, socket_a_atender);

	//Agregarlo a new
	/*
	sem_wait(&m_nuevos);
	queue_push(procesosNuevos, &pcb_recibido);
	sem_post(&m_nuevos);
	*/
}


void planificadorLargoPlazo(){
	pthread_t hiloNew;
	pthread_t hiloExit;
	pthread_create(&hiloNew,NULL,mandar_a_ready,NULL);
	pthread_create(&hiloExit,NULL,terminar_procesos,NULL);

	pthread_detach(hiloNew);
	pthread_detach(hiloExit);
}

void mandar_a_ready(){
	// Chequear grado de multiprogramacion

	// mandarle a memoria para que inicialize las estructuras necesarias, y tabla de segmentos para alm en pcb.
	// Ponerlo en la cola de ready (x FIFO SIEMPRE)
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
		log_info(logger, "A punto de matar consola");
		//avisar a consola para matar conexion
		op_code terminar_consola = FIN_PROCESO_CONSOLA;
		enviar_codigo(pcb->socket_consola, terminar_consola);


		// Hacer free de todos los malloc (PCB Y CDE)
	}
}

t_pcb* retirar_pcb_de(t_queue* cola, pthread_mutex_t* mutex){
	t_pcb* pcb;
	pthread_mutex_lock(mutex);
	pcb = queue_pop(cola);
	pthread_mutex_unlock(mutex);
	return pcb;
}

void agregar_pcb_a(t_queue* cola, t_pcb* pcb_a_agregar, pthread_mutex_t* mutex){
	log_info(logger, "Estoy en agregar_pcb_a()");
	pthread_mutex_lock(mutex);
	queue_push(cola, pcb_a_agregar);
	pthread_mutex_unlock(mutex);
}

void liberar_todos_recursos(t_pcb* pcb){
	int cant_recursos = list_size(pcb -> recursos_asignados);

	for (int i=0; i < cant_recursos; i++){
		char* recurso = list_remove(pcb -> recursos_asignados, i);
		sumar_instancia_recurso(recurso);

	}
}

void restar_instancia_recurso(char* recurso){
	actualizar_instancias_recurso(recurso, -1);
}

void sumar_instancia_recurso(char* recurso){
	actualizar_instancias_recurso(recurso, 1);
}

void planificadorCortoPlazo(){
	if (strcmp(config_kernel.algoritmo, "FIFO") == 0)
		planificacionFIFO();
	else if(strcmp(config_kernel.algoritmo, "HRRN") == 0)
		planificacionHRRN();
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
	//pcb.estado = NEW;
	pcb->socket_consola = socket_con;
	log_info(logger, "Se crea el proceso %d en NEW", pcb->cde->pid);

	return pcb;
}


void planificacionFIFO(){
	

	//semaforo para ver que
	//if(!list_is_empty(procesosReady) || !list_is_empty(procesosBloqueados)){

	//hilo de ready	a exec
	//hilo de bloc a ready

	pthread_t transicionReadyExec;
	pthread_t  transicionBlockReady;
	pthread_create(&transicionReadyExec,NULL, enviarDeReadyAExecFIFO,NULL);
	pthread_create(&transicionBlockReady,NULL,enviarDeBlockAReadyFIFO,NULL);

	pthread_detach(transicionReadyExec);
	pthread_detach(transicionBlockReady);

	// (de exec a bloc es adentro del switch)
	// de exec a ready es YIELD
	return;
}

void enviarDeReadyAExecFIFO(){
	while(1){
		//chequear que no haya ninguno ejecutando
		//Sale el primero x FIFO
		if(!list_is_empty(procesosReady) && list_is_empty(procesosExec)){

			t_pcb* pcb = retirar_pcb_de(procesosReady, &mutex_ready);
			agregar_pcb_a(procesosExec, pcb, &mutex_exec);
			
			ejecutarProceso(pcb);
		}

	}
	return;
}


void ejecutarProceso(){
	// Arma el cde
	// Se lo envia a CPU
	// CPU ejecuta instrucciones y si necesita devuelve el CDE (do while de ulima instruccion != exit)
	// Cuando recibe el CDE, evalua la instruccion actual
	// Si la instruccion actual bloquea el proceso, lo saca de la colaExec
	// Llama al subprograma que bloquea.
	return;
}

void enviarDeBlockAReadyFIFO(){
	while(1){
		//Varias maneras de desbloquear un proceso, 1 por que hay un signal de un recurso q el proceso necesitbaa y no estaba disponible
		// antes,
	}
	return;
}

void planificacionHRRN(){

	return;
}

// UTILS INSTRUCCIONES
void evaluar_instruccion(t_instruction* instruccion_actual, t_cde cde_recibido){
	switch(instruccion_actual->instruccion){
		case F_READ:
			break;
		case F_WRITE:
			break;
		case MOV_IN:
			break;
		case MOV_OUT:
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
			break;
		case F_OPEN:
			break;
		case F_CLOSE:
			break;
		case DELETE_SEGMENT:
			break;
		case YIELD:
				ejecutar_YIELD();
			break;
		case EXIT:
			//agregar_pcb_a(procesosFinalizados, pcb);
			//sem_post(&cont_exit); este esta bien pero dsps lo seguimos
			break;
		default:
			break;
	}
}

void ejecutar_YIELD(){
	//hay que mandar al final de la cola de ready
	//hacemos el recv del cde

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

// FIN UTILS INSTRUCCIONES

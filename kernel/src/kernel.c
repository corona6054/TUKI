#include "../includes/kernel.h"

int main(void){

	sem_init(&m_nuevos, 0, 1);
	sem_init(&m_ready, 0, 1);
	sem_init(&m_bloqueados, 0, 1);
	sem_init(&m_finalizados, 0, 1);
	sem_init(&m_exec, 0, 1);

	levantar_modulo();
	
	procesosNuevos = list_create();
	procesosReady = list_create();
	procesosEjecutando = list_create();

	while(1);

	finalizar_modulo();

	return 0;
}


// SUBPROGRAMAS

void levantar_modulo(){
	logger = iniciar_logger();
	config = iniciar_config();
	levantar_config();
	establecer_conexiones();
	return;
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
	log_info(logger,"Levantando Config");

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

	/*
	char **recursos = config_get_array_value(config,"RECURSOS");

	char *recurso;
	log_info(logger,"Rompi Levantar Config");

	while(!list_is_empty(recursos)){
		recurso = list_remove(recursos,0);
		list_add_in_index(config_kernel.recursos_compartidos,0,recurso);
	}
	*/
	/*
	char **instancias_recursos = config_get_array_value(config,"INSTANCIAS_RECURSOS");


	while(!list_is_empty(instancias_recursos)){
		int instancia = atoi(list_remove(instancias_recursos,0));
		list_add_in_index(config_kernel.instancias,0,instancia);
	}

	log_info(logger,config_kernel.instancias);
	*/
}


void conectarse_con_memoria(){
	socket_memoria = crear_conexion(config_kernel.ip_memoria, config_kernel.puerto_memoria);
}


void conectarse_con_cpu(){
	socket_cpu = crear_conexion(config_kernel.ip_cpu, config_kernel.puerto_cpu);
}


void conectarse_con_file_system(){
	socket_file_system = crear_conexion(config_kernel.ip_file_system, config_kernel.puerto_file_system);	
}

void establecer_conexiones()
{
	pthread_t conexion_memoria;
	pthread_create(&conexion_memoria, NULL, (void *) conectarse_con_memoria, NULL);
	pthread_detach(conexion_memoria);

	pthread_t conexion_cpu;
	pthread_create(&conexion_cpu, NULL, (void *) conectarse_con_cpu, NULL);
	pthread_detach(conexion_cpu);

	pthread_t conexion_file_system;
	pthread_create(&conexion_file_system, NULL, (void *) conectarse_con_file_system, NULL);
	pthread_detach(conexion_file_system);

	server_fd = abrir_servidor(logger,config);
	
	pthread_t manejo_consolas;
	pthread_create(&manejo_consolas, NULL, (void *) manejar_clientes, (void *) server_fd);
	pthread_detach(manejo_consolas);	

}

void manejar_clientes(int server_fd){
	//thread para esperar clientes
	while(1){
		t_conexiones conexiones;
		conexiones.socket = esperar_cliente(server_fd, logger);
		log_info("Salio de esperar cliente",logger);
		conexiones.socket_anterior = 0;
		// threads para recepcion de info de las consolas


		pthread_t t;
		pthread_create(&t, NULL, (void *) manejar_conexion_con_consola, (void *) &conexiones);
		pthread_detach(t);
	}

}

t_list* recibir_paquete(int socket_cliente){
	int size;
	int desplazamiento = 0;
	void * buffer;
	t_list* valores = list_create();
	int tamanio;

	buffer = recibir_buffer(&size, socket_cliente);
	while(desplazamiento < size)
	{
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
		desplazamiento+=sizeof(int);
		char* valor = malloc(tamanio);
		memcpy(valor, buffer+desplazamiento, tamanio);
		desplazamiento+=tamanio;
		list_add(valores, valor);
	}
	free(buffer);
	return valores;
}

void manejar_conexion_con_consola(t_conexiones* conexiones){
	// Seria el planificador a largo plazo
	
	t_list* instrucciones ;
	int a = recibir_operacion(conexiones->socket);
	instrucciones = recibir_paquete(conexiones->socket);

	//log_info(logger, "size %d", list_size(instrucciones));
	/*
	void *instr = list_get(instrucciones,0);
	int primera;
	memcpy(&primera, instr, sizeof(int));
	*/
	//log_info(logger, "primera instr %d", primera);

	int estado_anterior = -1;
	pcb pcb = crear_pcb(instrucciones);
	
	list_add(procesosNuevos,&pcb);

	if(list_size(procesosReady) < config_kernel.grado_max_multi){
		
		sem_wait(&m_nuevos);
		list_remove(procesosNuevos,0);
		sem_post(&m_nuevos);

		estado_anterior = pcb.estado;
		pcb.estado = READY;

		sem_wait(&m_ready);
		list_add(procesosReady,&pcb);
		sem_post(&m_ready);
		
		//log_info(logger,"Cola Ready %s: , ")
		//mandar msje a memoria para que inicialice sus estructuras necesarias y obtenga la tabla de segmentos inicial
		log_info(logger,"PID: %d - Estado Anterior: %d - Estado Actual: %d", pcb.pid, estado_anterior, pcb.estado); //no va a terminar estando aca este log info

	}
	
	if (strcmp(config_kernel.algoritmo, "FIFO") == 0){
		planificacionFIFO();
	}
	else if(strcmp(config_kernel.algoritmo, "HRRN") == 0){
		planificacionHRRN();
	}	
}

Registros inicializar_registros(){
	Registros registros;

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

pcb crear_pcb(t_list *instrucciones){
	pcb pcb;

	pcb.pid = pid;
	pcb.lista_de_instrucciones = instrucciones;
	pcb.program_counter = 0;
	pcb.registro_cpu = inicializar_registros();
    pcb.tabla_segmentos = list_create();
	pcb.estimado_prox_rafaga = config_kernel.est_inicial;
	pcb.tiempo_llegada_ready = -1;
	t_list* archivos_abiertos = list_create();
	pcb.estado = NEW;

	log_info(logger, "Se crea el proceso %d en NEW", pcb.pid);

	pid++;
	return pcb;
}


void planificacionFIFO(){
	
	pcb *pcb_a_ejecutar;
	

	while(!list_is_empty(procesosReady)){
		sem_wait(&m_ready);
		pcb_a_ejecutar = list_remove(procesosReady,0);
		sem_post(&m_ready);

		// Contexto de ejecucion al cpu
	}
	return;
}

void planificacionHRRN(){
	
	return;
}

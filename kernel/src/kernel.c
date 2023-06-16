#include "../includes/kernel.h"

int main(void){

	sem_init(&m_nuevos, 0, 1);
	sem_init(&m_ready, 0, 1);
	sem_init(&m_bloqueados, 0, 1);
	sem_init(&m_finalizados, 0, 1);
	sem_init(&m_exec, 0, 1);
	

	procesosNuevos = list_create();
	procesosReady = list_create();
	procesosEjecutando = list_create();
	procesosBloqueados = list_create();
	
	levantar_modulo();
	

	pthread_t planificadorCorto;
	pthread_create(&planificadorCorto, NULL, (void *) planificadorCortoPlazo, NULL);
	pthread_detach(planificadorCorto);

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
		list_add(config_kernel.instancias,instancias[a]);
		a++;
	}
}


void conectarse_con_memoria(){
	socket_memoria = crear_conexion(config_kernel.ip_memoria, config_kernel.puerto_memoria);
	if (socket_memoria == 0)
		log_info(logger,"Conectado con memoria");
	else
	{
		//perror(socket_memoria);
		log_info(logger,"Error al conectar con memoria");
	}
}


void conectarse_con_cpu(){
	socket_cpu = crear_conexion(config_kernel.ip_cpu, config_kernel.puerto_cpu);
	if (socket_cpu == 0)
			log_info(logger,"Conectado con cpu");
		else
			log_info(logger,"Error al conectar con cpu");
}


void conectarse_con_file_system(){
	socket_file_system = crear_conexion(config_kernel.ip_file_system, config_kernel.puerto_file_system);
	if (socket_file_system == 0)
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

	pthread_t conexion_cpu;
	pthread_create(&conexion_cpu, NULL, (void *) conectarse_con_cpu, NULL);
	pthread_detach(conexion_cpu);

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
		t_conexiones conexiones;
		conexiones.socket = esperar_cliente(server_fd, logger);
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

void printElement(void* ptr) {
	Instruction* seleccionado = (Instruction*) ptr;
	printf("%d ", seleccionado->instruccion);
	        if (seleccionado->numero1 != 0)
	            printf("%d ", seleccionado->numero1);
	        if (seleccionado->numero2 != 0)
	            printf("%d ", seleccionado->numero2);
	        if (strcmp(seleccionado->string1, "") != 0)
	            printf("%s ", seleccionado->string1);
	        if (strcmp(seleccionado->string2, "") != 0)
	            printf("%s ", seleccionado->string2);
	        printf("\n");
}

void* deserializarInst(void* data){
		Instruction* instruction = malloc(sizeof(Instruction));
		int offset=0;
		memcpy(&instruction->instruccion,data + offset, sizeof(int));
		offset += sizeof(int);
		memcpy(&instruction->numero1,data + offset, sizeof(int));
		offset += sizeof(int);
		memcpy(&instruction->numero2,data + offset, sizeof(int));
		offset += sizeof(int);
		memcpy(&instruction->string1,data + offset, sizeof(char[15]));
		offset += sizeof(char[15]);
		memcpy(&instruction->string2,data + offset, sizeof(char[15]));
		offset += sizeof(char[15]);
		return instruction;
		}

void manejar_conexion_con_consola(t_conexiones* conexiones){
	// Seria el planificador a largo plazo
	log_info(logger,"Entro a planificador a largo plazo.");
	t_list* instrucciones ;
	int op = recibir_operacion(conexiones->socket);
	instrucciones = recibir_paquete(conexiones->socket);
	log_info(logger,"Paquete recibido.");
	list_map(instrucciones,deserializarInst);

	int estado_anterior = -1;
	pcb pcb = crear_pcb(instrucciones);
	list_add(procesosNuevos,&pcb);
	
	int total=0;
	total+=list_size(procesosReady);
	total+=list_size(procesosEjecutando);
	total+=list_size(procesosBloqueados);

	if( total < config_kernel.grado_max_multi){
		
		sem_wait(&m_nuevos);
		list_remove(procesosNuevos,0);
		sem_post(&m_nuevos);

		estado_anterior = pcb.estado;
		pcb.estado = READY;

		sem_wait(&m_ready);
		list_add(procesosReady,&pcb);
		sem_post(&m_ready);
		log_info(logger,"PID: %d - Estado Anterior: %d - Estado Actual: %d", pcb.pid, estado_anterior, pcb.estado); //no va a terminar estando aca este log info
		
		//planificadorCortoPlazo();
		//log_info(logger,"Cola Ready %s: , ")
		//mandar msje a memoria para que inicialice sus estructuras necesarias y obtenga la tabla de segmentos inicial

	}
	
	

}

void planificadorCortoPlazo(){
	if (strcmp(config_kernel.algoritmo, "FIFO") == 0){
		pthread_t fifo;
		pthread_create(&fifo, NULL, (void *) planificacionFIFO, NULL);
		pthread_detach(fifo);
	}
	else if(strcmp(config_kernel.algoritmo, "HRRN") == 0){
		pthread_t hrrn;
		pthread_create(&hrrn, NULL, (void *) planificacionHRRN, NULL);
		pthread_detach(hrrn);
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
	pcb.registro_proceso = inicializar_registros();
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
	
	while(1){
		
		if(!list_is_empty(procesosReady) || !list_is_empty(procesosBloqueados)){	
			pcb *pcb_a_ejecutar;
			
			sem_wait(&m_ready);
			pcb_a_ejecutar = list_remove(procesosReady,0);
			sem_post(&m_ready);
			
			contexto_de_ejecucion cde;

			cde.pid = pcb_a_ejecutar->pid;
    		cde.program_counter = pcb_a_ejecutar->program_counter;    
			cde.registrosCpu = pcb_a_ejecutar->registro_proceso;
			cde.tabla_segmentos = pcb_a_ejecutar->tabla_segmentos;
			cde.estado = pcb_a_ejecutar->estado;
			
			Cde_serializado cde_ser = serializar_cde(cde);
			t_paquete *paquete = crear_paquete();
			agregar_a_paquete(paquete,&cde_ser,tamanio_cde_serializado(cde_ser));
			
			//agregar a paquete
			// enviar a cpu

			// tiene que esperar rta del cpu
			// deserializa el paquete
			// replanifica si es necesario

		}
	}
	return;
}


void planificacionHRRN(){

	return;
}

// UTILS SERIALIZACION

Cde_serializado serializar_cde(contexto_de_ejecucion cde){
	// Ver tamanios de los nodos de las listas
	Cde_serializado cde_serializado;

	cde_serializado.pid = cde.pid;
	cde_serializado.program_counter = cde.program_counter;

    cde_serializado.tam_lista_instrucciones = sizeof(Instruction) * list_size(cde.lista_de_instrucciones); //despues cambiar el 42 (tamaño de una instrucion, el tema es que dsps hay q cambiarlo por uint_32)
    cde_serializado.lista_de_instrucciones = malloc(cde_serializado.tam_lista_instrucciones);
	
    cde_serializado.registrosCpu = cde.registrosCpu; // tamanio fijo de 112 bytes
	
    cde_serializado.tam_tabla_segmentos = sizeof(Segmento) * list_size(cde.tabla_segmentos);
    cde_serializado.tabla_segmentos;
    
    cde_serializado.estado = cde.estado;

	return cde_serializado;
}


int tamanio_cde_serializado(Cde_serializado cde_ser){
	return sizeof(uint32_t) * 4 + 112 + cde_ser.tam_lista_instrucciones + cde_ser.tam_tabla_segmentos;
}
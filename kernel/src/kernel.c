#include "../includes/kernel.h"


int main(void){
	// prueba utils buffer
	uint32_t b = 5;
	int elemento = -1;
	char* a;
	
	sem_init(&prueba1, 0, 0);
	
	levantar_modulo();
	
	


	t_buffer* buffer = crear_buffer_nuestro();
	
	buffer_write_string(buffer,"CHAU");
	buffer_write_string(buffer,"HOLA123");
	buffer_write_uint32(buffer,10);

	
	log_info(logger, "Ya cargue el stream");
	
	sem_wait(&prueba1);

	// Primero enviamos el tamanio del buffer
	log_info(logger,"Tamanio del buffer: %d",buffer->size);
	// [INFO] 22:24:28:391 Kernel/(4462:4462): Tamanio del buffer: 23
	send(socket_cpu, &(buffer->size), sizeof(uint32_t), 0);
	
	// Despues enviamos el stream del buffer
	send(socket_cpu, buffer->stream, buffer->size, 0);
	log_info(logger, "Ya mande el stream");
		
	/*
	a = buffer_read_string(buffer);
	log_info(logger,"%s", a);
	free(a);
	a = buffer_read_string(buffer);
	log_info(logger,"%s", a);
	b = buffer_read_uint32(buffer);
	log_info(logger, "%lu", b);
	for(int i = 0;i<2;i++){
		b = buffer_read_uint32(buffer);
		log_info(logger, "%lu", b);
	}
	


	b = buffer_read_uint32(buffer);
		log_info(logger, "%lu", b);

	*/
	while(1);
	// fin prueba utils buffer
	
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
		sem_post(&prueba1);
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

	
	pthread_t conexion_cpu;
	pthread_create(&conexion_cpu, NULL, (void *) conectarse_con_cpu, NULL);
	pthread_detach(conexion_cpu);
	

	/*
	pthread_t conexion_file_system;
	pthread_create(&conexion_file_system, NULL, (void *) conectarse_con_file_system, NULL);
	pthread_detach(conexion_file_system);
	*/

	/*
	server_fd = iniciar_servidor(logger, config_kernel.puerto_escucha);
	
	pthread_t manejo_consolas;
	pthread_create(&manejo_consolas, NULL, (void *) manejar_clientes, (void *) server_fd);
	pthread_detach(manejo_consolas);
	*/

}

void manejar_clientes(int server_fd){
	//thread para espersar clientes
	while(1){
		t_conexiones conexiones;
		conexiones.socket = esperar_cliente(server_fd, logger);
		conexiones.socket_anterior = 0;
		// threads para recepcion de info de las consolas


		pthread_t t;
		pthread_create(&t, NULL, (void *) planificadorLargoPlazo, (void *) &conexiones);
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
		memcpy(&instruction->instruccion,data + offset, sizeof(u_int32_t));
		offset += sizeof(u_int32_t);
		memcpy(&instruction->numero1,data + offset, sizeof(u_int32_t));
		offset += sizeof(u_int32_t);
		memcpy(&instruction->numero2,data + offset, sizeof(u_int32_t));
		offset += sizeof(u_int32_t);
		memcpy(&instruction->string1,data + offset, sizeof(char[15]));
		offset += sizeof(char[15]);
		memcpy(&instruction->string2,data + offset, sizeof(char[15]));
		offset += sizeof(char[15]);
		return instruction;
		}

void planificadorLargoPlazo(t_conexiones* conexiones){
	// Seria el planificador a largo plazo
	log_info(logger,"Entro a planificador a largo plazo.");
	t_list* instrucciones ;
	int op = recibir_operacion(conexiones->socket);
	instrucciones = recibir_paquete(conexiones->socket);
	log_info(logger,"Paquete recibido.");
	list_map(instrucciones,deserializarInst);

	list_iterate(instrucciones,printElement);

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

		//estado_anterior = pcb.estado;
		//pcb.estado = READY;

		sem_wait(&m_ready);
		list_add(procesosReady,&pcb);
		sem_post(&m_ready);
		log_info(logger,"PID: %d - Estado Anterior: %d - Estado Actual: READY", pcb.pid, estado_anterior); //no va a terminar estando aca este log info
		
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
	//pcb.estado = NEW;

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


			sem_wait(&m_exec);
			list_add(procesosEjecutando, pcb_a_ejecutar);
			sem_post(&m_exec);
			
			log_info(logger,"Pid pcb_a_ejecutar: %d", pcb_a_ejecutar->pid);
			//log_info(logger,"Estado pcb_a_ejecutar: %d", pcb_a_ejecutar->estado);

			contexto_de_ejecucion cde_a_enviar;

			cde_a_enviar.pid = pcb_a_ejecutar->pid;
    		cde_a_enviar.program_counter = pcb_a_ejecutar->program_counter;    
			cde_a_enviar.registrosCpu = pcb_a_ejecutar->registro_proceso;
			cde_a_enviar.tabla_segmentos = pcb_a_ejecutar->tabla_segmentos;
			//cde_a_enviar.estado = pcb_a_ejecutar->estado;
			
			serializar_cde(cde_a_enviar); //serializa el cde, arma el paquete y lo envia a cpu
			Cde_serializado cde_recibido;
				// = deserializar_cde(); // recibe y deserializa cde;
			
			
			// tiene que esperar rta del cpu
			// deserializa el paquete
			Instruction* instruccion_actual = list_get(cde_recibido.lista_de_instrucciones,cde_recibido.program_counter - 1);
			
			// evaluar_instruccion(instruccion_actual, cde_recibido);
			
			

			// replanifica si es necesario

		}
	}
	return;
}


void planificacionHRRN(){

	return;
}

// UTILS SERIALIZACION
/*
void serializar2_cde(contexto_de_ejecucion cde){
	t_paquete* paquete = crear_paquete();
	
  	int32_t tam_lista = list_size(cde.lista_de_instrucciones) * sizeof(Instruction);
	int32_t tam_tabla = list_size(cde.tabla_segmentos) * (sizeof(Segmento));
	
	int tam_malloc = 
		tam_lista + tam_tabla +
		112 + (sizeof(uint32_t) * 4) + sizeof(uint8_t);

	void* stream = malloc(tam_malloc);
	int offset = 0;

	agregar_a_paquete(paquete, &cde.pid, sizeof(uint32_t));
	agregar_a_paquete(paquete, &cde.program_counter, sizeof(uint32_t));
	agregar_a_paquete(paquete, tam_lista, sizeof(uint32_t));

	for(int i=0;i<tam_lista;i++){
		agregar_a_paquete(paquete,list_get(cde.lista_de_instrucciones,i),sizeof())
	}	
}
*/

t_buffer* cargar_buffer(contexto_de_ejecucion cde){
	t_buffer* buffer = crear_buffer_nuestro();


		

	return buffer;
}

// -------------- UTILS BUFFER ---------------

t_buffer* crear_buffer_nuestro(){
	t_buffer* b = malloc(sizeof(t_buffer));
	b->size = 0;
	b->stream = NULL;
	return b;
}

// UINT32
void buffer_write_uint32(t_buffer* buffer, uint32_t entero){
	buffer->stream = realloc(buffer->stream, buffer->size + sizeof(uint32_t));
	
	memcpy(buffer->stream + buffer->size, &entero, sizeof(uint32_t));
	buffer->size += sizeof(uint32_t);
}

uint32_t buffer_read_uint32(t_buffer *buffer){
	uint32_t entero;
	
	memcpy(&entero, buffer->stream, sizeof(uint32_t));
	buffer->stream += sizeof(uint32_t);

	return entero;
}

// UINT8
void buffer_write_uint8(t_buffer* buffer, uint8_t entero){
	buffer->stream = realloc(buffer->stream, buffer->size + sizeof(uint8_t));
	
	memcpy(buffer->stream + buffer->size, &entero, sizeof(uint8_t));
	buffer->size += sizeof(uint8_t);
}

uint8_t buffer_read_uint8(t_buffer *buffer){
	uint8_t entero;
	
	memcpy(&entero, buffer->stream, sizeof(uint8_t));
	buffer->stream += sizeof(uint8_t);

	return entero;
}

//STRING
void buffer_write_string(t_buffer* buffer, char* cadena){
	uint32_t tam = 0;
	
	while(cadena[tam] != NULL)
		tam++;
	
	buffer_write_uint32(buffer,tam);
	
	buffer->stream = realloc(buffer->stream, buffer->size + tam);

	memcpy(buffer->stream + buffer->size, cadena , tam);
	buffer->size += tam;
}


char* buffer_read_string(t_buffer *buffer){
	uint32_t tam = buffer_read_uint32(buffer);
	char* cadena = malloc(tam);

	memcpy(cadena, buffer->stream, tam);
	buffer->stream += tam;

	return cadena;
}

//INSTRUCTION
void buffer_write_Instruction(t_buffer* buffer, Instruction instruccion){
	buffer_write_uint8(buffer, instruccion.instruccion);
	buffer_write_uint32(buffer, instruccion.numero1);
	buffer_write_uint32(buffer, instruccion.numero2);
	buffer_write_string(buffer, instruccion.string1);
	buffer_write_string(buffer, instruccion.string2);
}

// Por ahi lo deberia devolver como puntero (ver issue)
Instruction buffer_read_Instruction(t_buffer* buffer){
	Instruction inst;
	
	inst.instruccion = buffer_read_uint8(buffer);
	inst.numero1 = buffer_read_uint32(buffer);
	inst.numero2 = buffer_read_uint32(buffer);
	strcpy(inst.string1, buffer_read_string(buffer));
	strcpy(inst.string2, buffer_read_string(buffer));

	return inst;
}

//REGISTROS
void buffer_write_Registros(t_buffer* buffer, Registros registros){
	buffer_write_string(buffer, registros.AX);
	buffer_write_string(buffer, registros.BX);
	buffer_write_string(buffer, registros.CX);
	buffer_write_string(buffer, registros.DX);

	buffer_write_string(buffer, registros.EAX);
	buffer_write_string(buffer, registros.EBX);
	buffer_write_string(buffer, registros.ECX);
	buffer_write_string(buffer, registros.EDX);

	buffer_write_string(buffer, registros.RAX);
	buffer_write_string(buffer, registros.RBX);
	buffer_write_string(buffer, registros.RCX);
	buffer_write_string(buffer, registros.RDX);
}

Registros buffer_read_Registros(t_buffer* buffer){
	Registros regis;

	strcpy(regis.AX, buffer_read_string(buffer));
	strcpy(regis.BX, buffer_read_string(buffer));
	strcpy(regis.CX, buffer_read_string(buffer));
	strcpy(regis.DX, buffer_read_string(buffer));

	strcpy(regis.EAX, buffer_read_string(buffer));
	strcpy(regis.EBX, buffer_read_string(buffer));
	strcpy(regis.ECX, buffer_read_string(buffer));
	strcpy(regis.EDX, buffer_read_string(buffer));

	strcpy(regis.RAX, buffer_read_string(buffer));
	strcpy(regis.RBX, buffer_read_string(buffer));
	strcpy(regis.RCX, buffer_read_string(buffer));
	strcpy(regis.RDX, buffer_read_string(buffer));

	return regis;
}

// SEGMENTOS
void buffer_write_segmento(t_buffer* buffer,Segmento segm){
	buffer_write_uint32(buffer, segm.id);
	buffer_write_uint32(buffer, segm.direccion_base);
	buffer_write_uint32(buffer, segm.tamanio_segmentos);
}


Segmento buffer_read_segmento(t_buffer* buffer){
	Segmento seg;

	seg.id = buffer_read_uint32(buffer);
	seg.direccion_base = buffer_read_uint32(buffer);
	seg.tamanio_segmentos = buffer_read_uint32(buffer);

	return seg;
}
// ------------ FIN DE UTILS DE BUFFER -------------------

void serializacion(contexto_de_ejecucion cde){
	int tam;
	//void* stream = armar_stream(cde,tam);
	//t_paquete* paquete = crear_paquete();
	//agregar_a_paquete(paquete, stream, tam);
	//eliminar_paquete(paquete);
	
}

void serializar_cde(contexto_de_ejecucion cde){
	// Ver tamanios de los nodos de las listas
	Cde_serializado cde_serializado;

	cde_serializado.pid = cde.pid;
	cde_serializado.program_counter = cde.program_counter;

	int tam_de_1_nodo_instruccion = (sizeof(uint32_t)*2 + sizeof(uint8_t) + 15 + 15);

    cde_serializado.tam_lista_instrucciones = 
		tam_de_1_nodo_instruccion * list_size(cde.lista_de_instrucciones); 
	
	cde_serializado.lista_de_instrucciones = cde.lista_de_instrucciones;
	
    cde_serializado.registrosCpu = cde.registrosCpu; // tamanio fijo de 112 bytes
	
    cde_serializado.tam_tabla_segmentos = (sizeof(uint32_t)*3) * list_size(cde.tabla_segmentos);
    
	cde_serializado.tabla_segmentos = cde.tabla_segmentos;
    
    //cde_serializado.estado = 0;

	t_paquete* paquete = crear_paquete();

	int tam =
		cde_serializado.tam_lista_instrucciones + cde_serializado.tam_tabla_segmentos +
		112 + (sizeof(uint32_t) * 4) + sizeof(uint8_t);

	void* stream = malloc(tam);
	int offset = 0;
	
	memcpy(stream + offset, &cde_serializado.pid,sizeof(uint32_t));
	offset += sizeof(uint32_t);
	
	memcpy(stream + offset, &cde_serializado.program_counter,sizeof(uint32_t));
	offset += sizeof(uint32_t);

	// tam_lista_instrucciones esta en bytes
	memcpy(stream + offset, &cde_serializado.tam_lista_instrucciones,sizeof(uint32_t));
	offset += sizeof(uint32_t);
	

	for(int i=0; i < list_size(cde_serializado.lista_de_instrucciones); i++){
		memcpy(stream + offset, list_get(cde_serializado.lista_de_instrucciones,i),tam_de_1_nodo_instruccion);
		offset += tam_de_1_nodo_instruccion;
	}
	
	memcpy(stream + offset, &cde_serializado.registrosCpu, 112);
	offset += 112;

	// tam_tabla_segmentos esta en bytes
	memcpy(stream + offset, &cde_serializado.tam_tabla_segmentos,sizeof(uint32_t));
	offset += sizeof(uint32_t);
	
	memcpy(stream + offset, cde_serializado.tabla_segmentos, cde_serializado.tam_tabla_segmentos);
	offset += cde_serializado.tam_tabla_segmentos;
	
	/*
	memcpy(stream + offset, &cde_serializado.estado, sizeof(uint8_t));
	offset += sizeof(uint8_t);
	*/
	mem_hexdump(stream,tam);

	log_info("offset: %d, tam: %d", offset, tam);

	agregar_a_paquete(paquete, stream, 8);
	enviar_paquete(paquete, socket_cpu);
	eliminar_paquete(paquete);

	// Llenamos el paquete --------------------------------------------------------
	//paquete->codigo_operacion = CONTEXTOEJEC; // Podemos usar una constante por operación

	// Armamos el stream a enviar	

	/*
	void* a_enviar = malloc(paquete->buffer->size + sizeof(uint8_t) + sizeof(uint32_t));
	offset = 0;
	
	memcpy(a_enviar + offset, &(paquete->codigo_operacion), sizeof(uint8_t));
	offset += sizeof(uint8_t);

	memcpy(a_enviar + offset, &(paquete->buffer->size), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	
	memcpy(a_enviar + offset, paquete->buffer->stream, paquete->buffer->size);

	// Por último enviamos
	send(socket_cpu, a_enviar, paquete->buffer->size + sizeof(uint8_t) + sizeof(uint32_t), 0);
	*/
	// No nos olvidamos de liberar la memoria que ya no usaremos
	list_destroy(cde_serializado.tabla_segmentos);
	list_destroy(cde_serializado.lista_de_instrucciones);
	free(stream);

	/*
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
	*/
}


int tamanio_cde_serializado(Cde_serializado cde_ser){
	return sizeof(uint32_t) * 4 + 112 + cde_ser.tam_lista_instrucciones + cde_ser.tam_tabla_segmentos;
}

void deserializar_cde(){

}



// UTILS INSTRUCCIONES

void evaluar_instruccion(Instruction* instruccion_actual, contexto_de_ejecucion cde_recibido){
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
			if(actualizar_instancias_recurso(instruccion_actual->string1, -1) == 0)
				log_info(logger,"PID: %d - Wait: %s - Instancias: %d",cde_recibido.pid,instruccion_actual->string1,
						obtener_instancia_recurso(instruccion_actual->string1));
			else{
				log_info(logger,"Fin de Proceso: “Finaliza el proceso %d - Motivo: INVALID_RESOURCE",cde_recibido.pid);
				// LIBERAR RECURSOS DEL PROCESO
			}
			break;
		case SIGNAL:
			if(actualizar_instancias_recurso(instruccion_actual->string1, 1) == 0)
				log_info(logger,"PID: %d - Signal: %s - Instancias: %d",cde_recibido.pid,instruccion_actual->string1,
						obtener_instancia_recurso(instruccion_actual->string1));
			else{
				log_info(logger,"Fin de Proceso: “Finaliza el proceso %d - Motivo: INVALID_RESOURCE",cde_recibido.pid);
				// LIBERAR RECURSOS DEL PROCESO
			}
			break;
		case F_OPEN:
			break;
		case F_CLOSE:
			break;
		case DELETE_SEGMENT:
			break;
		case YIELD:

			break;
		case EXIT:
			break;
		default:
			break;
	}
}

int actualizar_instancias_recurso(char* recurso_a_actualizar, int numero){
	// Paso el numero para poder hacer +1 y -1 por wait y signal

	char* recurso;
	int* instancia;

	for(int i=0; i< list_size(config_kernel.recursos_compartidos) ; i++){
		recurso = list_get(config_kernel.recursos_compartidos,i);
		if(strcmp(recurso, recurso_a_actualizar) == 0){
			instancia = list_get(config_kernel.instancias,i) + numero;
			list_replace(config_kernel.instancias, i, instancia);
			return 0;
		}
	}
	return -1;

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

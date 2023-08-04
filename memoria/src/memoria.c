#include "../includes/memoria.h"

int main(void){
	levantar_modulo();

	
	while(1);
	
	//list_iterate(tabla_segmentos,printList);
	//list_iterate(espacios_libres,printElement);



	pedidoEscritura(memoria_principal,sizeof(int),needed_memory);
	void * leido=pedidoLectura(memoria_principal,sizeof(int));
	printf("Pedido lectura %d \n",(*(int*)leido) );
	free(leido);

	compactarMemoria();
	terminarEstructuras();


	while(1);
	return 0;
}



//crea memoria auxiliar y realoca segmentos ahi
void compactarMemoria(){
	log_info(logger,"Solicitud Compactación");
	list_iterate(tabla_segmentos_global, removeSeg0);
	void* mem_auxiliar = malloc(config_memoria.tam_memoria);
	memcpy(mem_auxiliar,segmento0->direccion_base,segmento0->tamanio_segmentos);
	segmento0->direccion_base= mem_auxiliar;
	seg_anterior->start=segmento0->direccion_base;
	seg_anterior->size=segmento0->direccion_base;
	list_iterate(tabla_segmentos_global,realocarLista);
	*needed_memory=128;
	list_iterate(tabla_segmentos_global,GetTotalSize);
	list_iterate(tabla_segmentos_global,addSeg0);
	list_clean_and_destroy_elements(espacios_libres,DestroySegment);
	Segment *inicial;
	inicial = malloc(sizeof(Segment));
	inicial->start =(void *)((intptr_t) mem_auxiliar+*needed_memory);
	inicial->size =config_memoria.tam_memoria-*needed_memory;
	list_add(espacios_libres,inicial);
	free(memoria_principal);
	memoria_principal=mem_auxiliar;
	//list_iterate(espacios_libres,printElement);
	*needed_memory=0;
	list_iterate(tabla_segmentos_global,ResultadoCompactacion);

}

void  ResultadoCompactacion (void* ptr) {
	t_list* tabla = (t_list*) ptr;
	Segment* segmento;
	for(int i=1;i<config_memoria.cant_segmentos;i++){
		segmento = list_get(tabla,i);
		if(segmento->size!=0) {
			log_info(logger,"PID: %d - Segmento: %d - Base: %p - Tamaño %d",*needed_memory,i,segmento->start,segmento->size);
		}
		(*needed_memory)++;
	}
}

void  DestroySegment (void* ptr) {
	Segment* seleccionado = (Segment*) ptr;
	free(seleccionado);
}


// Devuelve el total de segmentos asignados
void  GetTotalSize (void* ptr) {
	t_list* seleccionado = (t_list*) ptr;
	list_iterate(seleccionado,getEachSize);
}
void  getEachSize (void* ptr) {
	Segment* seleccionado = (Segment*) ptr;
	*needed_memory+=seleccionado->size;
}


void  realocarLista (void* ptr) {
	t_list* seleccionado = (t_list*) ptr;
	list_iterate(seleccionado,realocarSegmento);
}

// Coloca cada segmento al final del anterior
void  realocarSegmento (void* ptr) {
	Segment* seleccionado = (Segment*) ptr;
	void* fin_adjacent= seg_anterior->start +seg_anterior->size;
	if(seleccionado->size!=0){
	void * nueva_pos =(void *)((intptr_t) mem_auxiliar+ (intptr_t) fin_adjacent);
	memcpy(nueva_pos,seleccionado->start,seleccionado->size);
	seleccionado->start=fin_adjacent;
	seg_anterior->start = seleccionado->start;
	seg_anterior->size = seleccionado->size;
	}
}

//agregar y remover seg0 de tablas de segmentos
void  addSeg0 (void* ptr) {
	t_list* seleccionado = (t_list*) ptr;
	list_add_in_index(seleccionado,0,segmento0);
}
void  removeSeg0 (void* ptr) {
	t_list* seleccionado = (t_list*) ptr;
	list_remove_element(seleccionado, segmento0);
}



// lee y escribe posiciones en memoria principal
void *pedidoLectura(int *direccion, int size){
	void *leido=malloc(size);
	memcpy(leido,direccion,size);
	return leido;
}

int pedidoEscritura(int *direccion, int size, void*nuevo_dato){
	memcpy(direccion,nuevo_dato,size);
	return 0;
}


// crea un nuevo proceso y le asigna segmento0
t_tabla_segmentos_por_proceso* crearProceso(uint32_t pid_a_crear){
	t_tabla_segmentos_por_proceso* tabla_segmentos_proceso_nuevo = malloc(sizeof(t_tabla_segmentos_por_proceso));
	tabla_segmentos_proceso_nuevo->tabla_segmentos = list_create();
	tabla_segmentos_proceso_nuevo->pid = pid_a_crear;
	list_add(tabla_segmentos_proceso_nuevo->tabla_segmentos, segmento0);

	list_add(tabla_segmentos_global, tabla_segmentos_proceso_nuevo);

	log_info(logger,"Creacion Proceso PID: %d", pid_a_crear);
	return tabla_segmentos_proceso_nuevo;
}


//crea y elimina segmentos
void crearSegmento(int id,int index, int size){
	t_list* proceso=list_get(tabla_segmentos_global,id);
	Segment* seg_nuevo;
	seg_nuevo =(Segment*)list_get(proceso,index);
	if (seg_nuevo->size == 0 && index < 16){
		seg_nuevo->size = size;
		agregarSegmento(seg_nuevo);
		list_replace(proceso,index,seg_nuevo);
		log_info(logger,"PID: %d - CrearSeg: %d - Base: %p - TAMANIO: %d",id,index,seg_nuevo->start,size);
		}
	}

void eliminarSegmento(int id, int index){
	t_list* proceso=list_get(tabla_segmentos_global,id);
	Segment* seg_viejo;
	seg_viejo =(Segment*)list_get(proceso,index);
	liberarSegmento(seg_viejo);
	log_info(logger,"PID: %d - EliminarSeg: %d - Base: %p - TAMANIO: %d",id,index,seg_viejo->start,seg_viejo->size);
	seg_viejo->start = NULL;
	seg_viejo->size =0;
	list_replace(proceso,index,seg_viejo);
}

//algoritmos para comparar
bool Equivalente(void* data){
		Segment* element = (Segment*)data;
		return (element->start == seg_anterior->start);

}

bool FirstFit(void* data){
		Segment* element = (Segment*)data;
		int tamanio = element->size;
		return (tamanio >= *needed_memory);

}

void* BestFit(void* a, void* b) {
    Segment* segA = (Segment*)a;
    Segment* SegB = (Segment*)b;
    return (segA->size > SegB->size) ? a : b;
}

void* WorstFit(void* a, void* b) {
    Segment* segA = (Segment*)a;
    Segment* SegB = (Segment*)b;
    return (segA->size > SegB->size) ? b : a;
}

// printea listas
void printList (void* ptr) {
	t_list* seleccionado = (t_list*) ptr;
	list_iterate(seleccionado,printElement);

}
void printElement(void* ptr) {
	Segment* seleccionado = (Segment*) ptr;
    printf("start: %p\n", seleccionado->start);
    printf("size: %d\n", seleccionado->size);
}

/*
int pedidoSegmento(int size){
	*needed_memory = 0;
	seleccionado = (Segment*) list_find(espacios_libres,FirstFit);
	if(seleccionado){
		agregarSegmento()
	}
	list_iterate(espacios_libres,getEachSize);
	if(size>)
}
*/
// agrega  segmento segun algoritmo elegido
void agregarSegmento(Segment *nuevo){
	*needed_memory = nuevo->size;
	Segment *seleccionado;
	switch(config_memoria.algoritmos_asignacion){
		case FIRST:
		seleccionado = (Segment*)list_remove_by_condition(espacios_libres,FirstFit);
		break;
		case BEST:
		seleccionado = (Segment*)list_get_maximum(espacios_libres,BestFit);
		seg_anterior->start=seleccionado->start;
		seleccionado = (Segment*)list_remove_by_condition(espacios_libres,Equivalente);
		break;
		case WORST:
		seleccionado = (Segment*)list_get_minimum(espacios_libres,WorstFit);
		seg_anterior->start=seleccionado->start;
		seleccionado= (Segment*)list_remove_by_condition(espacios_libres,Equivalente);
		break;
	}
	nuevo->start = seleccionado->start;
	seleccionado->start+=nuevo->size;
	seleccionado->size-=nuevo->size;
	list_add(espacios_libres,seleccionado);
}

bool AdyacenteIzquierda(void* data){
		Segment* element = (Segment*)data;
		void* final_seg = element->start+element->size;
		return (seg_anterior->start == final_seg );

}

bool AdyacenteDerecha(void* data){
		Segment* element = (Segment*)data;
		void* final_seg = seg_anterior->start+seg_anterior->size;
		return (element->start == final_seg );

}

// libera  segmento concatenando huecos adyacentes
void liberarSegmento(Segment *viejo){
	seg_anterior->start = viejo->start;
	seg_anterior->size = viejo->size;
	Segment *seg_derecha;
	Segment *seg_izquierda;
	seg_derecha=(Segment*)list_remove_by_condition(espacios_libres,AdyacenteDerecha);
	seg_izquierda=(Segment*)list_remove_by_condition(espacios_libres,AdyacenteIzquierda);
	if(seg_derecha!=NULL && seg_izquierda!=NULL){
		seg_izquierda->size+=seg_derecha->size +viejo->size;
		list_add(espacios_libres,seg_izquierda);
	}else
		if(seg_derecha!=NULL){
		seg_derecha->start-=viejo->size;
		seg_derecha->size+=viejo->size;
		list_add(espacios_libres,seg_derecha);
	}else
		if(seg_izquierda!=NULL){
		seg_izquierda->size+=viejo->size;
		list_add(espacios_libres,seg_izquierda);
	}
	else {
		seg_derecha = malloc(sizeof(Segment));
		seg_derecha->start =viejo->start;
		seg_derecha->size =viejo->size;
		list_add(espacios_libres,seg_derecha);
	}
}

// crea segmento0, memoria principal y agrega segmento0
int crearEstructuras(){
	memoria_principal = malloc(config_memoria.tam_memoria);
	
	segmento0 = malloc(sizeof(t_segmento));
	segmento0->id = 0;
	segmento0->direccion_base = 0;
 	segmento0->tamanio_segmentos = config_memoria.tam_segmento_0;
	
	tabla_segmentos_global = list_create();
	//agregarSegmento(segmento0);
	
	espacios_libres = list_create();

	needed_memory = malloc(sizeof(int));
	seg_anterior= malloc(sizeof(Segment));
	
	seg_maxsize= malloc(sizeof(int));
	*seg_maxsize= 128;

	Segment *inicial;
	inicial = malloc(sizeof(Segment));
	inicial->start =memoria_principal;
	inicial->size =config_memoria.tam_memoria;

	list_add(espacios_libres,inicial);
	
	log_info(logger,"Estructuras creadas");
	return 0;
}
int terminarEstructuras(){
	free(memoria_principal);
	free(segmento0);
	free(needed_memory);
	free(seg_anterior);
	free(seg_maxsize);
	free(contador_procesos);
	log_info(logger,"Estructuras terminadas");
	return 0;
}

void levantar_modulo(){
	logger = iniciar_logger();
	config = iniciar_config();
	levantar_config();
	crearEstructuras();
	establecer_conexiones();
}
void finalizar_modulo(){
	log_destroy(logger);
	config_destroy(config);
	return;
}

t_log* iniciar_logger(void)
{
	t_log* nuevo_logger;
	nuevo_logger = log_create("memoria.log", "Memoria", 1, LOG_LEVEL_INFO);

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

	nuevo_config = config_create("memoria.config");

	if (nuevo_config == NULL){
		printf("Error al crear el nuevo config\n");
		exit(2);
	}

	return nuevo_config;
}

Algorithm selectAlgorithm(char* input) {
    if (strcmp(input, "FIRST") == 0) {
        return FIRST;
    } else if (strcmp(input, "BEST") == 0) {
        return BEST;
    } else if (strcmp(input, "WORST") == 0) {
        return WORST;
    } else {
        return -1;
    }
}

void levantar_config(){
	config_memoria.puerto_escucha = config_get_int_value(config,"PUERTO_ESCUCHA");
	config_memoria.tam_memoria = config_get_int_value(config,"TAM_MEMORIA");
	config_memoria.tam_segmento_0 = config_get_int_value(config,"TAM_SEGMENTO_0");
	config_memoria.cant_segmentos = config_get_int_value(config,"CANT_SEGMENTOS");
	config_memoria.retardo_memoria = config_get_int_value(config,"RETARDO_MEMORIA");
	config_memoria.retardo_compactacion = config_get_int_value(config,"RETARDO_COMPACTACION");
	config_memoria.algoritmos_asignacion = 	selectAlgorithm(config_get_string_value(config,"ALGORITMO_ASIGNACION"));
}

void establecer_conexiones()
{
	server_fd = iniciar_servidor(logger, config_memoria.puerto_escucha);

	//cpu_fd = esperar_cliente(server_fd, logger);
	//fileSystem_fd = esperar_cliente(server_fd, logger);
	kernel_fd = esperar_cliente(server_fd, logger);
	if (kernel_fd >= 0){
		log_info(logger, "Conectado con KERNEL");
		if (recibir_handshake(kernel_fd) == HANDSHAKE){
			enviar_handshake(kernel_fd);
			log_info(logger, "Handshake con KERNEL realizado.");

			
			pthread_t comunicacion_kernel;
			pthread_create(&comunicacion_kernel, NULL, (void* ) manejar_conexion_con_kernel, NULL);
			pthread_detach(comunicacion_kernel);
			
		}
		else
			log_info(logger, "Ocurio un error al realizar el handshake con KERNEL.");
	}else
		log_info(logger, "Error al conectar con KERNEL");


	
}

void manejar_conexion_con_kernel(){
	while(1){
		t_buffer* buffer_recibido = recibir_buffer(kernel_fd);
		log_info(logger, "Buffer recibido");
		switch(buffer_recibido->codigo){
			case SOLICITUD_INICIO_PROCESO_MEMORIA:
				uint32_t pid_a_crear = buffer_read_uint32(buffer_recibido);
				destruir_buffer_nuestro(buffer_recibido);
				
				t_tabla_segmentos_por_proceso* tabla_segmentos_inicial_nueva = crearProceso(pid_a_crear);

				t_buffer* buffer_a_enviar = crear_buffer_nuestro();
				buffer_write_tabla_segmentos(buffer_a_enviar, tabla_segmentos_inicial_nueva->tabla_segmentos);
				buffer_a_enviar->codigo = INICIO_EXITOSO_PROCESO_MEMORIA;
				
				enviar_buffer(buffer_a_enviar, kernel_fd);
				log_info(logger, "Buffer enviado");
				destruir_buffer_nuestro(buffer_a_enviar);

				break;
			case SOLICITUD_FIN_PROCESO_MEMORIA:

				uint32_t pid_a_destruir = buffer_read_uint32(buffer_recibido);
				destruir_buffer_nuestro(buffer_recibido);
				t_tabla_segmentos_por_proceso* tabla_a_destruir = encontrar_tabla_por_pid(pid_a_destruir);

				destruir_tabla_segmentos_por_proceso(tabla_a_destruir);
				
				log_info(logger, "Elminacion de proceso PID: %d", tabla_a_destruir->pid);
				op_code fin_proceso = FIN_EXITOSO_PROCESO_MEMORIA;
				enviar_codigo(kernel_fd, fin_proceso);
				break;
			default:
				log_info(logger, "Default");
				break;
			}
			
	}		
}
// Supongo que existe la tabla del proceso a buscar
// FALTA PROBARRR !!!!!
t_tabla_segmentos_por_proceso* encontrar_tabla_por_pid(uint32_t pid_a_buscar){
	for(int i = 0; i < list_size(tabla_segmentos_global); i++){
		t_tabla_segmentos_por_proceso* tabla = list_get(tabla_segmentos_global, i);
		if (tabla->pid == pid_a_buscar)
			log_info(logger, "Tabla encontrada");
			return tabla;
	}
}

void destruir_tabla_segmentos_por_proceso(t_tabla_segmentos_por_proceso* tabla_segmentos_por_proceso){
	list_destroy(tabla_segmentos_por_proceso->tabla_segmentos);
	free(tabla_segmentos_por_proceso);
	return;
}
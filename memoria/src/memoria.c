#include "../includes/memoria.h"

t_log* logger;
t_config* config;

int server_fd;

// Clientes
int cpu_fd;
int fileSystem_fd;
int kernel_fd;



void *memoria_principal;
t_list * espacios_libres;
t_list * tabla_segmentos;
Segment *segmento0;
int *needed_memory;
Segment *adjacent_seg;
int *seg_maxsize;
void* mem_auxiliar;

int main(void){
	levantar_modulo();
	crearEstructuras();
	t_list *p1= crearProceso();
	t_list *p2= crearProceso();
	crearSegmento(p1,1,64);
	crearSegmento(p1,2,32);

	crearSegmento(p2,1,32);

	eliminarSegmento(p1,1);
	//eliminarSegmento(p1,2);
	list_iterate(tabla_segmentos,printList);
	list_iterate(espacios_libres,printElement);



	pedidoEscritura(memoria_principal,sizeof(int),needed_memory);
	printf("Pedido lectura %d \n", *(int*)pedidoLectura(memoria_principal,sizeof(int)));

	compactarMemoria();


	//while(1);
	finalizar_modulo();
	return 0;
}

void compactarMemoria(){
	tabla_segmentos =list_map(tabla_segmentos,removeSeg0);
	void* mem_auxiliar = malloc(config_memoria.tam_memoria);
	memcpy(mem_auxiliar,segmento0->start,segmento0->size);
	segmento0->start= mem_auxiliar;
	adjacent_seg->start=segmento0->start;
	adjacent_seg->size=segmento0->size;
	tabla_segmentos=list_map(tabla_segmentos,realocarLista);
	*needed_memory=128;
	list_iterate(tabla_segmentos,GetTotalSize);
	tabla_segmentos=list_map(tabla_segmentos,addSeg0);
	list_clean(espacios_libres);
	Segment *inicial;
	inicial = malloc(sizeof(struct Segment));
	inicial->start =(intptr_t) mem_auxiliar+*needed_memory;
	inicial->size =config_memoria.tam_memoria-*needed_memory;
	list_add(espacios_libres,inicial);
	list_iterate(espacios_libres,printElement);
}
void  GetTotalSize (void* ptr) {
	t_list* seleccionado = (t_list*) ptr;
	list_iterate(seleccionado,getEachSize);
}
void  getEachSize (void* ptr) {
	Segment* seleccionado = (Segment*) ptr;
	*needed_memory+=seleccionado->size;
}

void * realocarLista (void* ptr) {
	t_list* seleccionado = (t_list*) ptr;
	t_list* nueva =list_map(seleccionado,realocarSegmento);
	 list_clean(seleccionado);
	return nueva;
}
void * realocarSegmento (void* ptr) {
	Segment* seleccionado = (Segment*) ptr;
	void* fin_adjacent= adjacent_seg->start +adjacent_seg->size;
	if(seleccionado->size!=0){
	memcpy((intptr_t) mem_auxiliar+ (intptr_t) fin_adjacent,seleccionado->start,seleccionado->size);
	seleccionado->start=fin_adjacent;
	adjacent_seg->start = seleccionado->start;
	adjacent_seg->size = seleccionado->size;
	}
	return seleccionado;
}

void * addSeg0 (void* ptr) {
	t_list* seleccionado = (t_list*) ptr;
	t_list* nueva=list_create();
	list_add(nueva,segmento0);
	list_add_all(nueva,seleccionado);
	list_clean(seleccionado);
	return nueva;
}
void * removeSeg0 (void* ptr) {
	t_list* seleccionado = (t_list*) ptr;
	t_list* nueva =list_slice(seleccionado,1,config_memoria.cant_segmentos);
	 list_clean(seleccionado);
	return nueva;
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


void *pedidoLectura(int *direccion, int size){
	void *leido= malloc(size);
	memcpy(leido,direccion,size);
	return leido;
}

int pedidoEscritura(int *direccion, int size, void*nuevo_dato){
	memcpy(direccion,nuevo_dato,size);
	return 0;
}

t_list* crearProceso(){
	t_list* proceso_nuevo= list_create();
	list_add(proceso_nuevo,segmento0);
	Segment* seg_nuevo;
	for(int i =1;i<config_memoria.cant_segmentos; i++){
		seg_nuevo=malloc(sizeof(Segment));
		seg_nuevo->start =0;
		seg_nuevo->size =0;
		list_add(proceso_nuevo,seg_nuevo);
	}
	list_add(tabla_segmentos,proceso_nuevo);
	return proceso_nuevo;
}
void crearSegmento(t_list* proceso,int id, int size){
	Segment* seg_nuevo=malloc(sizeof(Segment));
	seg_nuevo =(Segment*)list_get(proceso,id);
	if (seg_nuevo->size==0&&id<16){
		seg_nuevo->size = size;
		agregarSegmento(seg_nuevo);
		list_replace(proceso,id,seg_nuevo);
		}
	}

void eliminarSegmento(t_list* proceso, int id){
	Segment* seg_viejo=malloc(sizeof(Segment));
	seg_viejo =(Segment*)list_get(proceso,id);
	sacarSegmento(seg_viejo);
	seg_viejo->start = NULL;
	seg_viejo->size =0;
	list_replace(proceso,id,seg_viejo);
}
bool Equivalente(void* data){
		Segment* element = (Segment*)data;
		return (element->start == adjacent_seg->start);

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

void printList (void* ptr) {
	t_list* seleccionado = (t_list*) ptr;
	list_iterate(seleccionado,printElement);

}
void printElement(void* ptr) {
	Segment* seleccionado = (Segment*) ptr;
    printf("start: %p\n", seleccionado->start);
    printf("size: %d\n", seleccionado->size);
}
void agregarSegmento(Segment *nuevo){
	*needed_memory = nuevo->size;
	Segment *seleccionado;
	seleccionado = malloc(sizeof(Segment));
	switch(config_memoria.algoritmos_asignacion){
		case FIRST:
		seleccionado=(Segment*)list_remove_by_condition(espacios_libres,FirstFit);
		break;
		case BEST:
		seleccionado=(Segment*)list_get_maximum(espacios_libres,BestFit);
		adjacent_seg->start=seleccionado->start;
		seleccionado=(Segment*)list_remove_by_condition(espacios_libres,Equivalente);
		break;
		case WORST:
		seleccionado=(Segment*)list_get_minimum(espacios_libres,WorstFit);
		adjacent_seg->start=seleccionado->start;
		seleccionado=(Segment*)list_remove_by_condition(espacios_libres,Equivalente);
		break;
	}
	nuevo->start = seleccionado->start;
	seleccionado->start+=nuevo->size;
	seleccionado->size-=nuevo->size;
	list_add(espacios_libres,seleccionado);
}

bool Adyacente(void* data){
		Segment* element = (Segment*)data;
		void* final_seg = adjacent_seg->start+adjacent_seg->size;
		return (element->start == final_seg );

}
void sacarSegmento(Segment *viejo){
	adjacent_seg->start = viejo->start;
	adjacent_seg->size = viejo->size;
	Segment *seleccionado;
	seleccionado = malloc(sizeof(Segment));
	seleccionado=(Segment*)list_remove_by_condition(espacios_libres,Adyacente);
	if(seleccionado!=NULL){
	seleccionado->start-=viejo->size;
	seleccionado->size+=viejo->size;
	list_add(espacios_libres,seleccionado);
	}else{
		seleccionado = malloc(sizeof(Segment));
		seleccionado->start =viejo->start;
		seleccionado->size =viejo->size;
		list_add(espacios_libres,seleccionado);
	}
}


int crearEstructuras(){
	memoria_principal = malloc(config_memoria.tam_memoria);
	segmento0 = malloc(sizeof(struct Segment));
	needed_memory = malloc(sizeof(int));
	adjacent_seg= malloc(sizeof(struct Segment));
	seg_maxsize= malloc(sizeof(int));
	*seg_maxsize= 128;
	tabla_segmentos = list_create();
	espacios_libres = list_create();
	Segment *inicial;
	inicial = malloc(sizeof(struct Segment));
	inicial->start =memoria_principal;
	inicial->size =config_memoria.tam_memoria;
	list_add(espacios_libres,inicial);
	segmento0->size = config_memoria.tam_segmento_0;
	agregarSegmento(segmento0);
	log_info(logger,"Estructuras creadas");
	return 0;
}

void levantar_modulo(){
	logger = iniciar_logger();
	config = iniciar_config();
	levantar_config();
	//establecer_conexiones();
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
	server_fd = abrir_servidor(logger, config);

	cpu_fd = esperar_cliente(server_fd, logger);
	fileSystem_fd = esperar_cliente(server_fd, logger);
	kernel_fd = esperar_cliente(server_fd, logger);
}

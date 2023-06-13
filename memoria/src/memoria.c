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
int *seg_maxsize;


int main(void){
	levantar_modulo();
	crearEstructuras();
	t_list *p1;
	p1 = crearProceso(480);
	list_iterate(p1,printElement);
	list_iterate(espacios_libres,printElement);

	//while(1);
	finalizar_modulo();
	return 0;
}
t_list* crearProceso(int totalSize){
	t_list* proceso_nuevo= list_create();
	list_add(proceso_nuevo,segmento0);
	Segment* seg_nuevo;
	int currentSize=0;
	while (currentSize < totalSize) {
		seg_nuevo= malloc(sizeof(Segment));
		if(currentSize + *seg_maxsize <= totalSize){
			seg_nuevo->size = *seg_maxsize;
		} else{
			seg_nuevo->size= totalSize - currentSize;
	}
		agregar_segmento(seg_nuevo);
		list_add(proceso_nuevo,seg_nuevo);
        currentSize += *needed_memory;
}
	return proceso_nuevo;
}


bool FirstFit(void* data){
		Segment* element = (Segment*)data;
		int tamanio = element->size;
		return (tamanio >= *needed_memory);

}
void printElement(void* ptr) {
	Segment* seleccionado = (Segment*) ptr;
    printf("start: %p\n", seleccionado->start);
    printf("size: %d\n", seleccionado->size);
}
void agregar_segmento(Segment *nuevo){
	*needed_memory = nuevo->size;
	Segment *seleccionado;
	seleccionado = malloc(sizeof(Segment));
	seleccionado=(Segment*)list_remove_by_condition(espacios_libres,FirstFit);
	nuevo->start = seleccionado->start;
	seleccionado->start+=nuevo->size;
	seleccionado->size-=nuevo->size;
	list_add(espacios_libres,seleccionado);
}


int crearEstructuras(){
	memoria_principal = malloc(config_memoria.tam_memoria);
	segmento0 = malloc(sizeof(struct Segment));
	needed_memory = malloc(sizeof(int));
	seg_maxsize= malloc(sizeof(int));
	*seg_maxsize= 128;
	espacios_libres = list_create();
	Segment *inicial;
	inicial = malloc(sizeof(struct Segment));
	inicial->start =memoria_principal;
	inicial->size =config_memoria.tam_memoria;
	list_add(espacios_libres,inicial);
	segmento0->start = memoria_principal;
	segmento0->size = config_memoria.tam_segmento_0;
	agregar_segmento(segmento0);
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
	config_memoria.algoritmos_asignacion = config_get_string_value(config,"ALGORITMO_ASIGNACION");
}

void establecer_conexiones()
{
	server_fd = abrir_servidor(logger, config);

	cpu_fd = esperar_cliente(server_fd, logger);
	fileSystem_fd = esperar_cliente(server_fd, logger);
	kernel_fd = esperar_cliente(server_fd, logger);
}

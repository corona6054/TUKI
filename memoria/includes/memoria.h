#ifndef INCLUDES_MEMORIA_H_
#define INCLUDES_MEMORIA_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commons/log.h"
#include "commons/config.h"
#include "commons/collections/list.h"
#include "serializacion.h"


typedef enum {
    FIRST,
    BEST,
    WORST
} Algorithm;

typedef struct Segment {
    void* start;
    int size;
} Segment;

typedef struct{
	int puerto_escucha;
	int tam_memoria;
	int tam_segmento_0;
	int cant_segmentos;
	int retardo_memoria;
	int retardo_compactacion;
	int algoritmos_asignacion;
}Config_memoria;


// Variables globales
Config_memoria config_memoria;
t_log* logger;
t_config* config;

int server_fd;

int cpu_fd;
int fileSystem_fd;
int kernel_fd;

void *memoria_principal;
t_list* espacios_libres;
t_list* tabla_segmentos;
Segment* segmento0;
int *needed_memory;
Segment *seg_anterior;
int *seg_maxsize;
void* mem_auxiliar;
int *contador_procesos;

// Prototipos funciones
void levantar_modulo();
void finalizar_modulo();

t_log* iniciar_logger(void);
t_config* iniciar_config(void);
void levantar_config();;


void  ResultadoCompactacion (void* ptr);
void  DestroyList (void* ptr);
void  DestroySegment (void* ptr);
void  GetTotalSize (void* ptr);
void  getEachSize (void* ptr);
void  realocarSegmento (void* ptr);
void  realocarLista (void* ptr);
void compactarMemoria();
void  addSeg0 (void* ptr);
void  removeSeg0 (void* ptr);
Algorithm selectAlgorithm(char* input);
int crearEstructuras();
int terminarEstructuras();
bool FirstFit(void* data);
bool AdyacenteIzquierda(void* data);
bool AdyacenteDerecha(void* data);
void printElement(void* ptr);
void printList (void* ptr);
void crearSegmento(int id,int index, int size);
void eliminarSegmento(int id, int index);
void agregarSegmento(Segment *nuevo);
void liberarSegmento(Segment *viejo);
void crearProceso();
void *pedidoLectura(int *direccion, int size);
int pedidoEscritura(int *direccion, int size, void*nuevo_dato);


void levantar_modulo();
void establecer_conexiones();

#endif /* INCLUDES_MEMORIA_H_ */

#ifndef INCLUDES_MEMORIA_H_
#define INCLUDES_MEMORIA_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commons/log.h"
#include "commons/config.h"
#include "commons/collections/list.h"
#include "utils_server.h"

typedef struct{
	int puerto_escucha;
	int tam_memoria;
	int tam_segmento_0;
	int cant_segmentos;
	int retardo_memoria;
	int retardo_compactacion;
	char *algoritmos_asignacion;
}Config_memoria;

// Variables globales
Config_memoria config_memoria;
t_log* logger;
t_config* config;

int server_fd;

int cpu_fd;
int fileSystem_fd;
int kernel_fd;

typedef struct Segment {
    void* start;
    int size;
} Segment;

// Prototipos funciones
void levantar_modulo();
void finalizar_modulo();

t_log* iniciar_logger(void);
t_config* iniciar_config(void);
void levantar_config();;

int crearEstructuras();
bool FirstFit(void* data);
bool Adyacente(void* data);
void printElement(void* ptr);
void printList (void* ptr);
void crearSegmento(t_list* proceso,int id, int size);
void eliminarSegmento(t_list* proceso, int id);
void agregarSegmento(Segment *nuevo);
void sacarSegmento(Segment *viejo);
t_list* crearProceso();
void *pedidoLectura(int *direccion, int size);
int pedidoEscritura(int *direccion, int size, void*nuevo_dato);


void levantar_modulo();
void establecer_conexiones();

#endif /* INCLUDES_MEMORIA_H_ */

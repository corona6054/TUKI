#ifndef INCLUDES_FILE_SYSTEM_H_
#define INCLUDES_FILE_SYSTEM_H_

#include <stdio.h>
#include <stdlib.h>
#include "commons/config.h"
#include "commons/log.h"
#include "commons/bitarray.h"
#include <pthread.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include "comunicacion.h"




typedef struct {
	uint32_t  block_size;
	uint32_t block_count;
}superBloque;

typedef struct {
	char*  file_name;
	uint32_t file_size;
	uint32_t  direct_pointer;
	uint32_t indirect_pointer;
}FCB;

typedef struct{
	char* ip_memoria;
	int puerto_memoria;
	int puerto_escucha;
	char *path_superbloque;
	char *path_bitmap;
	char *path_bloques;
	char *path_fcb;
	int retardo_acceso_bloque;
}Config_file_system;

// Variables globales
Config_file_system config_file_system;
t_log* logger;
t_config* config;


int kernel_fd;
int socket_memoria;
int server_fd;

int bitarray_fd;
int archivobloques_fd;
char *archivobloques_pointer;
char *bitarray_pointer;
off_t bitarray_size;
off_t archivobloques_size;
t_bitarray * bitarray;
t_list* fcb_list;
char buscado[64];
superBloque superbloque;


// PROTOTIPOS DE FUNCIONES ----------------------------------------------------
// Inicializaciones
void levantar_modulo(char*config_path);
void finalizar_modulo();
t_log* iniciar_logger();
t_config* iniciar_config(char*config_path);
void levantar_config();

// Conexiones
void conectarse_con_memoria();
void establecer_conexiones();

// Subprogramas generales
uint32_t eliminarBloques(int eliminar,FCB* seleccionado);
uint32_t crearEstructuras();
uint32_t cerrarEstructuras();
uint32_t crearArchivo(char* nombre);
uint32_t abrirArchivo(char* nombre);
bool igualBuscado(void * ptr);
uint32_t  bloqueLibre();
uint32_t liberarBloque(uint32_t bit);
uint32_t truncarArchivo(char* nombre, int size);
char * leerArchivo(char* nombre, int pos, int size);
uint32_t escribirArchivo(char* nombre, int pos, int size, char*datos);
void manejar_conexion_con_kernel();


// FIN DE PROTOTIPOS DE FUNCIONES ---------------------------------------------

#endif /* INCLUDES_FILE_SYSTEM_H_ */

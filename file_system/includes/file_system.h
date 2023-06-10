#ifndef INCLUDES_FILE_SYSTEM_H_
#define INCLUDES_FILE_SYSTEM_H_

#include <stdio.h>
#include <stdlib.h>
#include "commons/config.h"
#include "commons/log.h"
#include "utils_cliente.h"
#include <pthread.h>

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



// Prototipos funciones
void levantar_modulo();
void finalizar_modulo();

t_log* iniciar_logger(void);
t_config* iniciar_config(void);
void levantar_config();

void conectarse_con_memoria();
void establecer_conexiones();

#endif /* INCLUDES_FILE_SYSTEM_H_ */

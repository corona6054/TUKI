#ifndef INCLUDES_CPU_H_
#define INCLUDES_CPU_H_

#include <stdio.h>
#include <stdlib.h>
#include "commons/config.h"
#include "commons/log.h"
#include "utils_cliente.h"
#include <pthread.h>


typedef enum{
	SET,
	YIELD,
	EXIT
}Op_code;

typedef struct{
	int retardo;
	char *ip_memoria;
	int puerto_memoria;
	int puerto_escucha;
	int tam_max_segmento;
}Config_cpu;

// Variables globales
Config_cpu config_cpu;
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

void ejecutar_set(char[], char[], int);
void ejecutar_exit();
void ejecutar_yield();

#endif /* INCLUDES_CPU_H_ */

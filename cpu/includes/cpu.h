#ifndef INCLUDES_CPU_H_
#define INCLUDES_CPU_H_

#include <stdio.h>
#include <stdlib.h>
#include "commons/config.h"
#include "commons/log.h"
#include "utils_cliente.h"
#include <pthread.h>
#include <stdint.h>
#include <semaphore.h>
#include "serializacion.h"

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

sem_t *sem_conexion;
sem_t *prueba1cpu;

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

void ejecutar_set(char[], char[], int, Registros);
void ejecutar_exit();
void ejecutar_yield();
void ejecutar_io(int);
void ejecutar_signal(/*recurso*/);
void ejecutar_wait(/*recurso*/);
void ejecutar_fopen(/*archivo*/);
void ejecutar_fclose(/*archivo*/);
void ejecutar_fread(/*archivo, int , int */);
void ejecutar_fwrite(/*archivo, int , int */);
void ejecutar_fseek(/*archivo, int */);
void ejecutar_ftruncate(/*archivo, int */);
void ejecutar_createsegment(/*int , int*/ );
void ejecutar_deletesegment(/*int */);

void deserializar_cde();
t_list* recibir_paquete(int);



#endif /* INCLUDES_CPU_H_ */

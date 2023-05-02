#ifndef INCLUDES_KERNEL_H_
#define INCLUDES_KERNEL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/config.h>
#include <pthread.h>
#include <semaphore.h>
#include "utils_server.h"
#include "utils_cliente.h"

typedef struct{
    int socket;
    int socket_anterior;
} t_conexiones;

typedef struct{
    int pid;
    t_list* lista_de_instrucciones;
    int program_counter;
    
}pcb;

t_log* iniciar_logger(void);
t_config* iniciar_config(void);

void levantar_modulo();
void establecer_conexiones();

void manejar_conexion_con_consola(t_conexiones*);
pcb crear_pcb(t_list*);
void finalizar_modulo();
void manejar_clientes(int);

#endif /* INCLUDES_KERNEL_H_ */

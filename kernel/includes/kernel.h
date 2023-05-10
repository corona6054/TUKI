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
#include <unistd.h>
#include "utils_server.h"
#include "utils_cliente.h"
#include <unistd.h>



typedef struct{
    int socket;
    int socket_anterior;
} t_conexiones;

typedef enum{
	NEW,
	READY,
	EXEC,
	BLOCK,
	EXIT
}estados;

typedef struct{
	char AX[4], BX[4], CX[4], DX[4];
	char EAX[8], EBX[8], ECX[8], EDX[8];
	char RAX[16], RBX[16], RCX[16], RDX[16];
}registros;

typedef struct{
    int pid;
    t_list* lista_de_instrucciones;
    int program_counter;
    registros registro_cpu;
    t_list* tabla_segmentos;
    int estimado_prox_rafaga;
    int tiempo_llegada_ready;
    t_list* archivos_abiertos;
    estados estado;
}pcb;

t_log* iniciar_logger(void);
t_config* iniciar_config(void);

void levantar_modulo();
void establecer_conexiones();

void manejar_conexion_con_consola(t_conexiones*);
pcb crear_pcb(t_list*);
void finalizar_modulo();
void manejar_clientes(int);

void establecer_conexiones_2();
void manejar_cliente(t_conexiones);


#endif /* INCLUDES_KERNEL_H_ */

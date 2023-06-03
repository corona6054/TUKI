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
	EXIT1
}estados;

typedef enum
{
    F_READ,
    F_WRITE,
    SET,
    MOV_IN,
    MOV_OUT,
    F_TRUNCATE,
    F_SEEK,
    CREATE_SEGMENT,
    IO,
    WAIT,
    SIGNAL,
    F_OPEN,
    F_CLOSE,
    DELETE_SEGMENT,
    YIELD,
    EXIT,
    INVALID
} InstructionType;

typedef struct
{
    int instruccion;
    int numero1;
    int numero2;
    char string1[15];
    char string2[15];
} Instruction;

typedef struct{
	char AX[4], BX[4], CX[4], DX[4];
	char EAX[8], EBX[8], ECX[8], EDX[8];
	char RAX[16], RBX[16], RCX[16], RDX[16];
}registros;


typedef struct{
	int id;
	int direccion_base;
	int tamanio_segmentos;
}segmento;

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

typedef struct{
	int pid;
	int pc; // no creo q sea int
	registros registrosCpu;
	t_list* tabla_segmentos;
}contexto_de_ejecucion;

typedef struct{
	char* ip_memoria;
	int puerto_memoria;
	char* ip_cpu;
	int puerto_cpu;
	char* ip_file_system;
	int puerto_file_system;
	int puerto_escucha;
	char* algoritmo;
	int est_inicial;
	int alpha_hrrn;
	int grado_max_multi;
	t_list* recursos_compartidos;
	t_list* instancias;
}app_config_kernel;

app_config_kernel config_kernel;

t_log* iniciar_logger(void);
t_config* iniciar_config(void);

void levantar_modulo();
void establecer_conexiones();

void manejar_conexion_con_consola(t_conexiones*);
pcb crear_pcb(t_list*);
void finalizar_modulo();
void manejar_clientes(int);

void planificacionFIFO();
void planificacionHRRN();
#endif /* INCLUDES_KERNEL_H_ */

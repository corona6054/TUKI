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
}Registros;

typedef enum{
	NEW,
	READY,
	EXEC,
	BLOCK,
	EXIT1
}estados;

typedef struct{
    uint32_t pid;
    t_list* lista_de_instrucciones;
    uint32_t program_counter;
	Registros registrosCpu;
	t_list* tabla_segmentos;
    estados estado;
}contexto_de_ejecucion;

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

typedef struct{
	int retardo;
	char *ip_memoria;
	int puerto_memoria;
	int puerto_escucha;
	int tam_max_segmento;
}Config_cpu;


typedef struct{
    uint32_t pid;
    uint32_t program_counter; 
    
    t_list* lista_de_instrucciones;
    uint32_t tam_lista_instrucciones;
	
    Registros registrosCpu; // tamanio fijo de 112 bytes
	
    t_list* tabla_segmentos;
    uint32_t tam_tabla_segmentos;
    
    estados estado;
}Cde_serializado;

typedef struct{
    uint32_t pid;
    uint32_t program_counter; 
    
    // t_list* lista_de_instrucciones;
    uint32_t tam_lista_instrucciones;
	
    Registros registrosCpu; // tamanio fijo de 112 bytes
	
    // t_list* tabla_segmentos;
    uint32_t tam_tabla_segmentos;
    
    estados estado;
}Cde_serializado_sin_listas;


typedef struct{
	uint32_t id;
	uint32_t direccion_base;
	uint32_t tamanio_segmentos;
}Segmento;

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

void ejecutar_set(char[], char[], int);
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

//---------FUNCIONES BUFFER---------------
t_buffer* crear_buffer_nuestro();

void buffer_write_uint32(t_buffer*, uint32_t);
void buffer_write_string(t_buffer*, char*);
void buffer_write_Instruction(t_buffer*, Instruction);
void buffer_write_uint8(t_buffer*, uint8_t);
void buffer_write_Registros(t_buffer*, Registros);

uint32_t buffer_read_uint32(t_buffer*);
char* buffer_read_string(t_buffer*);
Instruction buffer_read_Instruction(t_buffer*);
uint8_t buffer_read_uint8(t_buffer*);
Registros buffer_read_Registros(t_buffer*);
//----------------------------------------

#endif /* INCLUDES_CPU_H_ */

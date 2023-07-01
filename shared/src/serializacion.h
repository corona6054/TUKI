#ifndef SERIALIZACION_H_
#define SERIALIZACION_H_


#include <stdint.h> // Para uintX_t
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/config.h>
#include <commons/memory.h>
#include <pthread.h>
#include <semaphore.h>


// STRUCTS NECESARIOS ------------------------------------------------------


typedef enum
{
	CONTEXTOEJEC,
	MENSAJE,
	PAQUETE
}op_code;

typedef struct
{
	uint32_t size;
	uint32_t offset;
	void* stream;
} t_buffer;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

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
    InstructionType instruccion;
    uint32_t numero1;
    uint32_t numero2;
    char string1[15];
    char string2[15];
} Instruction;

typedef struct{
	char AX[4], BX[4], CX[4], DX[4];
	char EAX[8], EBX[8], ECX[8], EDX[8];
	char RAX[16], RBX[16], RCX[16], RDX[16];
}Registros;

typedef struct{
	uint32_t id;
	uint32_t direccion_base;
	uint32_t tamanio_segmentos;
}Segmento;

typedef struct{
    int pid;
    t_list* lista_de_instrucciones;
    int program_counter;
    Registros registro_proceso;
    t_list* tabla_segmentos;
    int estimado_prox_rafaga;
    int tiempo_llegada_ready;
    t_list* archivos_abiertos;
}pcb;


typedef struct{
    uint32_t pid;
    t_list* lista_de_instrucciones;
    uint32_t program_counter;
	Registros registrosCpu;
	t_list* tabla_segmentos;
}contexto_de_ejecucion;



// FIN DE STRUCTS NECESARIOS -----------------------------------------------



// PROTOTIPOS DE FUNCIONES -------------------------------------------------


t_buffer* crear_buffer_nuestro();

void buffer_write_uint32(t_buffer*, uint32_t);
uint32_t buffer_read_uint32(t_buffer*);

void buffer_write_string(t_buffer*, char*);
char* buffer_read_string(t_buffer*);

void buffer_write_Instruction(t_buffer*, Instruction);
Instruction* buffer_read_Instruction(t_buffer*);

void buffer_write_uint8(t_buffer*, uint8_t);
uint8_t buffer_read_uint8(t_buffer*);

void buffer_write_lista_instrucciones(t_buffer*, t_list*);
t_list* buffer_read_lista_instrucciones(t_buffer*);

void buffer_write_Registros(t_buffer*, Registros);
Registros buffer_read_Registros(t_buffer*);

void buffer_write_segmento(t_buffer*,Segmento);
Segmento* buffer_read_segmento(t_buffer*);

void buffer_write_tabla_segmentos(t_buffer*, t_list*);
t_list* buffer_read_tabla_segmentos(t_buffer*);

void buffer_write_cde(t_buffer*, contexto_de_ejecucion);
contexto_de_ejecucion buffer_read_cde(t_buffer*);


// FIN DE PROTOTIPOS DE FUNCIONES ------------------------------------------

#endif /* SERIALIZACION_H_ */

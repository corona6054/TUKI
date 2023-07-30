#ifndef SERIALIZACION_H_
#define SERIALIZACION_H_

#define INT_VACIO 123456789
#define CHAR_VACIO "v"

#include <stdint.h> // Para uintX_t
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <netdb.h>

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
	PAQUETE,
    LISTA_INSTRUCCIONES,
	INICIO_PROCESO_MEMORIA, // aviso a memoria para que inicialice las estructuras del proceso
	FIN_PROCESO_MEMORIA, //para dar aviso que tiene uqe liberar las estructuras
	FIN_PROCESO_CONSOLA //aviso a consola para finalizacion del procesos
}op_code;

typedef struct
{
	uint32_t size;
	uint32_t offset;
	uint8_t codigo; // Para decir que guarda el buffer (nose si en vez de uint8_t tendria que ser op_code)
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
    char* string1;
    char* string2;
} t_instruction;

typedef struct{
	char AX[4], BX[4], CX[4], DX[4];
	char EAX[8], EBX[8], ECX[8], EDX[8];
	char RAX[16], RBX[16], RCX[16], RDX[16];
}t_registros;

typedef struct{
	uint32_t id;
	uint32_t direccion_base;
	uint32_t tamanio_segmentos;
}t_segmento;


typedef struct{
    uint32_t pid;
    t_list* lista_de_instrucciones;
    uint32_t program_counter;
	t_registros registros_cpu;
	t_list* tabla_segmentos;
}t_cde;

typedef struct{
    t_cde* cde;
    int socket_consola;

    uint32_t estimado_prox_rafaga;
    uint32_t tiempo_llegada_ready;
    
    t_list* archivos_abiertos;
    t_list* recursos_asignados;
    t_list* recursos_solicitados;
}t_pcb;


// FIN DE STRUCTS NECESARIOS -----------------------------------------------



// PROTOTIPOS DE FUNCIONES -------------------------------------------------

t_paquete* crear_paquete(op_code, t_buffer*);
void enviar_paquete(t_paquete*, int);
void* serializar_paquete(t_paquete*, int);
t_buffer* recibir_buffer(int);

t_buffer* crear_buffer_nuestro();

void buffer_write_uint32(t_buffer*, uint32_t);
uint32_t buffer_read_uint32(t_buffer*);

void buffer_write_string(t_buffer*, char*);
char* buffer_read_string(t_buffer*);
char* buffer_read_stringV2(t_buffer*, uint32_t* tam);

void buffer_write_Instruction(t_buffer*, t_instruction);
t_instruction* buffer_read_Instruction(t_buffer*);

void buffer_write_uint8(t_buffer*, uint8_t);
uint8_t buffer_read_uint8(t_buffer*);

void buffer_write_lista_instrucciones(t_buffer*, t_list*);
t_list* buffer_read_lista_instrucciones(t_buffer*);

void buffer_write_Registros(t_buffer*, t_registros);
t_registros buffer_read_Registros(t_buffer*);

void buffer_write_segmento(t_buffer*,t_segmento);
t_segmento* buffer_read_segmento(t_buffer*);

void buffer_write_tabla_segmentos(t_buffer*, t_list*);
t_list* buffer_read_tabla_segmentos(t_buffer*);

void buffer_write_cde(t_buffer*, t_cde);
t_cde buffer_read_cde(t_buffer*);

void mostrar_instrucciones(t_log*, t_list*);
void  mostrar_instruccion(t_log*, t_instruction*);

// FIN DE PROTOTIPOS DE FUNCIONES ------------------------------------------

#endif /* SERIALIZACION_H_ */

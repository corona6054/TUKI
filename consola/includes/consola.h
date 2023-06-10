#ifndef INCLUDES_CONSOLA_H_
#define INCLUDES_CONSOLA_H_

#define MAX_INSTRUCTIONS 100
#define MAX_PARAMETERS 10
#define MAX_PARAMETER_LENGTH 50

#include <stdio.h>
#include <stdlib.h>
#include "commons/config.h"
#include "commons/log.h"
#include "commons/collections/list.h"
#include "utils_cliente.h"
#include <string.h>
#include <pthread.h>

 typedef struct
{
    int instr;
    int valor;
    int valor2;
    char nombre[32];
    char nombre2[32];

}tablaInstr;

typedef struct
{
    int instruccion;
    int numero1;
    int numero2;
    char string1[15];
    char string2[15];
} Instruction;

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
	char *ip_kernel;
	int puerto_kernel;
}Config_consola;

//Variables globales
t_log* logger;
t_config* config;

Config_consola config_consola;

int socket_kernel;
Instruction instructions[MAX_INSTRUCTIONS];
int instructionCount = 0;


// Prototipos funciones
void serializeInstruction(Instruction* instruction, void* stream, int offset);
int enviarLista();

void levantar_modulo(char*);
void finalizar_modulo();

t_log* iniciar_logger(void);
t_config* iniciar_config(char* );
void levantar_config();

void establecer_conexiones();
void conectarse_con_kernel();

InstructionType getNextInstruction(FILE *file);

void readIntegerFromFile(FILE *file, int index);
void readNextWordFromFile(FILE *file, int index);
void parseInstructions(FILE *file);
void match(FILE *file, const char *expected);
void error();
int crearLista(char* filename);


#endif /* INCLUDES_CONSOLA_H_ */

#ifndef INCLUDES_CONSOLA_H_
#define INCLUDES_CONSOLA_H_

#include <stdio.h>
#include <stdlib.h>
#include "commons/config.h"
#include "commons/log.h"
#include "commons/collections/list.h"
#include "bison.tab.h"
#include "utils_cliente.h"
#include <string.h>

 typedef struct
{
    int instr;
    int valor;
    int valor2;
    char nombre[32];
    char nombre2[32];

}tablaInstr;

t_log* iniciar_logger(void);
t_config* iniciar_config(char* );

t_config* iniciar_config2();

void enviarLista(t_list*);
void levantar_modulo(char*);
void establecer_conexiones();
void finalizar_modulo();

#endif /* INCLUDES_CONSOLA_H_ */

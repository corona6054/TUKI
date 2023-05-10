#ifndef INCLUDES_CPU_H_
#define INCLUDES_CPU_H_

#include <stdio.h>
#include <stdlib.h>
#include "commons/config.h"
#include "commons/log.h"
#include "utils_cliente.h"

t_log* iniciar_logger(void);
t_config* iniciar_config(void);

void levantar_modulo();
void establecer_conexiones();

void ejecutar_set(char[], char[], int);
void ejecutar_exit();
void ejecutar_yield();

#endif /* INCLUDES_CPU_H_ */

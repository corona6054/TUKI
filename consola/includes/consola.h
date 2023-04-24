#ifndef INCLUDES_CONSOLA_H_
#define INCLUDES_CONSOLA_H_

#include <stdio.h>
#include <stdlib.h>
#include "commons/config.h"
#include "commons/log.h"
#include "utils_cliente.h"
#include <string.h>


t_log* iniciar_logger(void);
t_config* iniciar_config(void);

void levantar_modulo();
void establecer_conexiones();

#endif /* INCLUDES_CONSOLA_H_ */

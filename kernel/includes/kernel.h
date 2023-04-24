#ifndef INCLUDES_KERNEL_H_
#define INCLUDES_KERNEL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <commons/config.h>
#include "utils_server.h"
#include "utils_cliente.h"


t_log* iniciar_logger(void);
t_config* iniciar_config(void);

void levantar_modulo();
void establecer_conexiones();

#endif /* INCLUDES_KERNEL_H_ */

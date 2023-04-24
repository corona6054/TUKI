#ifndef INCLUDES_MEMORIA_H_
#define INCLUDES_MEMORIA_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commons/log.h"
#include "commons/config.h"
#include "utils_server.h"


t_log* iniciar_logger(void);
t_config* iniciar_config(void);

void levantar_modulo();
void establecer_conexiones();

#endif /* INCLUDES_MEMORIA_H_ */

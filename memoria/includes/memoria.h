#ifndef INCLUDES_MEMORIA_H_
#define INCLUDES_MEMORIA_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commons/log.h"
#include "utils_server.h"

int iniciar_servidor(void);
int esperar_cliente(int socket_servidor);

#endif /* INCLUDES_MEMORIA_H_ */

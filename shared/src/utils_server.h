#ifndef UTILS_SERVER_H_
#define UTILS_SERVER_H_

#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netdb.h>
#include<commons/string.h>
#include<commons/log.h>
#include<commons/config.h>
#include<commons/collections/list.h>
#include<string.h>
#include<assert.h>


//extern t_log* logger;




int iniciar_servidor(t_log*, int);
int esperar_cliente(int, t_log*);
t_list* recibir_paquete(int);
void recibir_mensaje(int);
int recibir_operacion(int);
void* recibir_buffer(int*, int);


int abrir_servidor(t_log*,t_config*);


#endif /* UTILS_SERVER_H_ */

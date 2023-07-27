#ifndef SOCKETS_H_
#define SOCKETS_H_


#include "serializacion.h"
#include <sys/socket.h>
#include <netdb.h>


// STRUCTS NECESARIOS ------------------------------------------------------



// FIN DE STRUCTS NECESARIOS -----------------------------------------------



// PROTOTIPOS DE FUNCIONES -------------------------------------------------

int iniciar_servidor(t_log*, int);
int crear_conexion(char*, int);


t_paquete* crear_paquete(op_code, t_buffer*);
void enviar_paquete(t_paquete*, int);
void* serializar_paquete(t_paquete*, int);
t_buffer* recibir_buffer(int);


void enviar_codigo(int, op_code);
op_code recibir_codigo(int);

// FIN DE PROTOTIPOS DE FUNCIONES ------------------------------------------

#endif /* SOCKETS_H_ */

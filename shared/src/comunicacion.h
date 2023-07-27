#ifndef COMUNICACION_H_
#define COMUNICACION_H_


#include <sys/socket.h>
#include <netdb.h>
#include <stdint.h> // Para uintX_t
#include <commons/log.h>
#include <unistd.h>


// STRUCTS NECESARIOS ------------------------------------------------------



// FIN DE STRUCTS NECESARIOS -----------------------------------------------



// PROTOTIPOS DE FUNCIONES -------------------------------------------------

int iniciar_servidor(t_log*, int);
int crear_conexion(char*, int);

void enviar_codigo(int, uint8_t);
uint8_t recibir_codigo(int);

// FIN DE PROTOTIPOS DE FUNCIONES ------------------------------------------

#endif /* COMUNICACION_H_ */

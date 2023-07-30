#include "comunicacion.h"


int iniciar_servidor(t_log* logger, int puerto)
{
	int socket_servidor;

	struct addrinfo hints, *servinfo;// *p

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(NULL, string_itoa(puerto), &hints, &servinfo);

	// Creamos el socket de escucha del servidor

	socket_servidor = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);


	// Asociamos el socket a un puerto

	bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen);


	// Escuchamos las conexiones entrantes

	listen(socket_servidor, SOMAXCONN);

	freeaddrinfo(servinfo);
	if(socket_servidor != -1)
		log_info(logger, "Listo para escuchar a mi cliente");
	else
		log_info(logger,"No se pudo abrir el servidor correctamente.");
	return socket_servidor;
}

int esperar_cliente(int socket_servidor, t_log* logger)
{
	int socket_cliente;
	socket_cliente = accept(socket_servidor, NULL, NULL);
	log_info(logger, "Se conecto un cliente!");

	return socket_cliente;
}

int crear_conexion(char *ip, int puerto)
{
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, string_itoa(puerto), &hints, &server_info);

	// Ahora vamos a crear el socket.
	int socket_cliente = 0;

	socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

	// Ahora que tenemos el socket, vamos a conectarlo

	connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen);


	freeaddrinfo(server_info);

	return socket_cliente;
}


void enviar_codigo(int socket, op_code codigo){
	send(socket, &codigo, sizeof(op_code), 0);
}

op_code recibir_codigo(int socket){
	op_code codigo;

	recv(socket, &codigo, sizeof(op_code), MSG_WAITALL);

	return codigo;
}

#include "sockets.h"


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


t_paquete* crear_paquete(op_code codigo, t_buffer* buffer)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = codigo;
	paquete->buffer = buffer;
	return paquete;
}

void enviar_paquete(t_paquete* paquete, int socket_cliente)
{
	int tamanio_paquete_serializado = paquete->buffer->size + sizeof(uint8_t) + sizeof(uint32_t);
	void* paquete_serializado = serializar_paquete(paquete, tamanio_paquete_serializado);

	send(socket_cliente, paquete_serializado, tamanio_paquete_serializado, 0);

	free(paquete_serializado);
}



void* serializar_paquete(t_paquete* paquete, int tam_paquete)
{
	// Esta funcion, mete al paquete en un stream para que se pueda enviar
	void* paquete_serializado = malloc(tam_paquete);
	int desplazamiento = 0;

	// Codigo de operacion
	memcpy(paquete_serializado + desplazamiento, &(paquete->codigo_operacion), sizeof(uint8_t));
	desplazamiento+= sizeof(int);

	// Tamanio del stream
	memcpy(paquete_serializado + desplazamiento, &(paquete->buffer->size), sizeof(uint32_t));
	desplazamiento+= sizeof(int);

	// Stream en si
	memcpy(paquete_serializado + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento+= paquete->buffer->size;

	return paquete_serializado;
}


t_buffer* recibir_buffer(int socket)
{
	// Esta funcion desarma el paquete y arma el buffer que luego sera deserializado
	t_buffer* buffer = crear_buffer_nuestro();

	// Recibo el codigo de operacion
	recv(socket, &(buffer -> codigo), sizeof(uint8_t), MSG_WAITALL);
	
	// Recibo el tamanio del buffer y reservo espacio en memoria
	recv(socket, &(buffer -> size), sizeof(uint32_t), MSG_WAITALL);
	buffer -> stream = malloc(buffer -> size);

	// Recibo stream del buffer
	recv(socket, buffer -> stream, buffer -> size, MSG_WAITALL);

	return buffer;
}


void enviar_codigo(int socket, op_code codigo){
	send(socket, &codigo, sizeof(uint8_t), 0);	
}

op_code recibir_codigo(int socket){
	op_code codigo;
	
	recv(socket, &codigo,sizeof(uint8_t), MSG_WAITALL);

	return codigo;
}

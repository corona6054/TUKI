#include "../includes/file_system.h"

t_log* logger;
t_config* config;

int main(void){

	logger = iniciar_logger();
	config = iniciar_config();

	// Hacerlo cliente de memoria:
	char* ip_memoria;
	int puerto_memoria;
	int socket_memoria;

	ip_memoria = config_get_string_value(config, "IP_MEMORIA");
	puerto_memoria = config_get_int_value(config, "PUERTO_MEMORIA");
	log_info(logger, "Ip Memoria: %s", ip_memoria);
	log_info(logger, "Puerto Memoria %d", puerto_memoria);

	socket_memoria = crear_conexion(ip_memoria, puerto_memoria);

	// Para que sea servidor:
	int puerto_escucha;
	puerto_escucha = config_get_int_value(config, "PUERTO_ESCUCHA");
	int server_fd = iniciar_servidor(logger, puerto_escucha);
	log_info(logger, "Servidor listo para recibir al cliente");
	int cliente_fd = esperar_cliente(server_fd, logger);

	return 0;
}

t_log* iniciar_logger(void)
{
	t_log* nuevo_logger;
	nuevo_logger = log_create("fileSystem.log", "Conectar file System a memoria", 1, LOG_LEVEL_INFO);

	if (nuevo_logger == NULL)
	{
		printf("Error al crear el logger\n");
		exit(1);
	}

	return nuevo_logger;
}

t_config* iniciar_config(void)
{
	t_config* nuevo_config;

	nuevo_config = config_create("fileSystem.config");

	if (nuevo_config == NULL){
		printf("Error al crear el nuevo config\n");
		exit(2);
	}

	return nuevo_config;
}

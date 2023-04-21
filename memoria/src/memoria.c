#include "../includes/memoria.h"

t_log* logger;
t_config* config;

int main(void){

	logger = iniciar_logger();
	config = iniciar_config();

	int puerto_escucha;

	puerto_escucha = config_get_int_value(config, "PUERTO_ESCUCHA");

	log_info(logger, "Puerto escucha: %d ", puerto_escucha);

	int server_fd = iniciar_servidor(logger, puerto_escucha);
	log_info(logger, "Servidor listo para recibir al cliente");

	int fileSystem_fd = esperar_cliente(server_fd, logger);
	log_info(logger, "Servidor escuchando al cliente 1");

	int cpu_fd = esperar_cliente(server_fd, logger);
	log_info(logger, "Servidor escuchando al cliente 2");

	int kernel_fd = esperar_cliente(server_fd, logger);
	log_info(logger, "Servidor escuchando al cliente 3");

	return 0;
}

t_log* iniciar_logger(void)
{
	t_log* nuevo_logger;
	nuevo_logger = log_create("memoria.log", "Memoria", 1, LOG_LEVEL_INFO);

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

	nuevo_config = config_create("memoria.config");

	if (nuevo_config == NULL){
		printf("Error al crear el nuevo config\n");
		exit(2);
	}

	return nuevo_config;
}

#include "../includes/memoria.h"

t_log* logger;

int main(void){
	t_config* config;
	int puerto_escucha;

	// Cambiar a LOG_LEVEL_DEBUG
	logger = log_create("memoria.log", "Memoria", 1, LOG_LEVEL_INFO);

	config = iniciar_config();

	puerto_escucha = config_get_int_value(config, "PUERTO_ESCUCHA");
	// log_info(logger, "Puerto escucha: %d ", puerto_escucha);

	int server_fd = iniciar_servidor(logger, puerto_escucha);
	log_info(logger, "Servidor listo para recibir al cliente");
	int cliente_fd = esperar_cliente(server_fd, logger);
	log_info(logger, "Servidor escuchando al cliente");

	return 0;
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

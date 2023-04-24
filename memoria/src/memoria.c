#include "../includes/memoria.h"

t_log* logger;
t_config* config;

int server_fd;

// Clientes
int cpu_fd;
int fileSystem_fd;
int kernel_fd;

int main(void){
	levantar_modulo();

	return 0;
}

void levantar_modulo(){
	logger = iniciar_logger();
	config = iniciar_config();
	establecer_conexiones();
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

void establecer_conexiones()
{
	server_fd = abrir_servidor(logger, config);

	cpu_fd = esperar_cliente(server_fd, logger);
	fileSystem_fd = esperar_cliente(server_fd, logger);
	kernel_fd = esperar_cliente(server_fd, logger);
}

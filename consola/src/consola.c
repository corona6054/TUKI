#include "../includes/consola.h"

t_log* logger;
t_config* config;

int socket_kernel;

int main(void){
	levantar_modulo();
	return 0;
}

// SUBPROGRAMAS

void levantar_modulo(){
	logger = iniciar_logger();
	config = iniciar_config();
	establecer_conexiones();
}

t_log* iniciar_logger(void)
{
	t_log* nuevo_logger;
	nuevo_logger = log_create("consola.log", "Consola", true, LOG_LEVEL_INFO);

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

	nuevo_config = config_create("consola.config");

	if (nuevo_config == NULL){
		printf("Error al crear el nuevo config\n");
		exit(2);
	}

	return nuevo_config;
}

void establecer_conexiones()
{
	socket_kernel = conectarse_a("KERNEL",config);
}

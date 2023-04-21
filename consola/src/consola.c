#include "../includes/consola.h"

t_log* logger;
t_config* config;

int main(void){

	logger = iniciar_logger();
	config = iniciar_config();

	// Hacerlo cliente de kernel
	char* ip_kernel;
	int puerto_kernel;
	int conexion;

	ip_kernel = config_get_string_value(config, "IP_KERNEL");
	puerto_kernel = config_get_int_value(config, "PUERTO_KERNEL");

	log_info(logger, "Ip Kernel: %s", ip_kernel);
	log_info(logger, "Puerto Kernel %d", puerto_kernel);

	conexion = crear_conexion(ip_kernel, puerto_kernel);


	return 0;
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


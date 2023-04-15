#include "../includes/consola.h"

int main(void){

	char* ip_kernel;
	char *puerto_kernel;
	t_log* logger;
	t_config* config;
	int conexion;

	logger = iniciar_logger();

	//log_info(logger, "Hola soy un log");

	config = iniciar_config();

	ip_kernel = config_get_string_value(config, "IP_KERNEL");
	puerto_kernel = config_get_string_value(config, "PUERTO_KERNEL");

	//Logueamos los valores

	log_info(logger, "Ip Kernel: %s", ip_kernel);
	log_info(logger, "Puerto Kernel %s", puerto_kernel);

	//conexion = crear_conexion(ip_kernel, puerto_kernel);


	return 0;
}

t_log* iniciar_logger(void)
{
	t_log* nuevo_logger;
	nuevo_logger = log_create("/home/utnso/Documents/tp-2023-1c-aLaGrandeLePuseCuca/consola/consola.log", "Conectar consola a kernel", true, LOG_LEVEL_INFO);

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

	nuevo_config = config_create("/home/utnso/Documents/tp-2023-1c-aLaGrandeLePuseCuca/consola/consola.config");

	if (nuevo_config == NULL){
		printf("Error al crear el nuevo config\n");
		exit(2);
	}

	return nuevo_config;
}


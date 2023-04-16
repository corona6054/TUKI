#include "../includes/cpu.h"

t_log* logger;

int main(void){
	char* ip_memoria;
	int puerto_memoria;
	t_config* config;
	int conexion;

	logger = iniciar_logger();

	//log_info(logger, "Hola soy un log");

	config = iniciar_config();


	ip_memoria = config_get_string_value(config, "IP_MEMORIA");
	puerto_memoria = config_get_int_value(config, "PUERTO_MEMORIA");

	//Logueamos los valores

	log_info(logger, "Ip Memoria: %s", ip_memoria);
	log_info(logger, "Puerto Memoria %d", puerto_memoria);

	conexion = crear_conexion(ip_memoria, puerto_memoria);

	return 0;
}

t_log* iniciar_logger(void)
{
	t_log* nuevo_logger;
	nuevo_logger = log_create("cpu.log", "Conectar cpu a memoria", 1, LOG_LEVEL_INFO);

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

	nuevo_config = config_create("cpu.config");

	if (nuevo_config == NULL){
		printf("Error al crear el nuevo config\n");
		exit(2);
	}

	return nuevo_config;
}


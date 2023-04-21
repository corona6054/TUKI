#include "../includes/kernel.h"

t_log* logger;
t_config* config;

int main(void){
	logger = iniciar_logger();
	config = iniciar_config();

	// Cliente de memoria
	char *ip_memoria;
	int puerto_memoria;
	int socket_memoria;
	ip_memoria = config_get_string_value(config, "IP_MEMORIA");
	puerto_memoria = config_get_int_value(config, "PUERTO_MEMORIA");

	log_info(logger, "Ip Memoria: %s", ip_memoria);
	log_info(logger, "Puerto Memoria %d", puerto_memoria);

	socket_memoria = crear_conexion(ip_memoria,puerto_memoria);


	// Cliente de fileSystem
	char *ip_fileSystem;
	int puerto_fileSystem;
	int socket_fileSystem;

	ip_fileSystem = config_get_string_value(config, "IP_FILESYSTEM");
	puerto_fileSystem = config_get_int_value(config, "PUERTO_FILESYSTEM");

	log_info(logger, "Ip FileSystem: %s", ip_fileSystem);
	log_info(logger, "Puerto FileSystem %d", puerto_fileSystem);

	socket_fileSystem = crear_conexion(ip_fileSystem,puerto_fileSystem);

	// Cliente de cpu
	char *ip_cpu;
	int puerto_cpu;
	int socket_cpu;

	ip_cpu = config_get_string_value(config, "IP_CPU");
	puerto_cpu = config_get_int_value(config, "PUERTO_CPU");

	log_info(logger, "Ip Cpu: %s", ip_cpu);
	log_info(logger, "Puerto  %d", puerto_cpu);

	socket_cpu = crear_conexion(ip_cpu,puerto_cpu);

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
	nuevo_logger = log_create("kernel.log", "Kernel", 1, LOG_LEVEL_INFO);

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

	nuevo_config = config_create("kernel.config");

	if (nuevo_config == NULL){
		printf("Error al crear el nuevo config\n");
		exit(2);
	}

	return nuevo_config;
}

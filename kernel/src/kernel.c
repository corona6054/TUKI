#include "../includes/kernel.h"

t_log* logger;
t_config* config;

int socket_memoria;
int socket_fileSystem;
int socket_cpu;

int server_fd;

int consola_fd;

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

void establecer_conexiones()
{

	socket_memoria = conectarse_a("MEMORIA",config);
	socket_fileSystem = conectarse_a("FILESYSTEM",config);
	socket_cpu = conectarse_a("CPU",config);

	server_fd = abrir_servidor(logger,config);
	consola_fd = esperar_cliente(server_fd, logger);

}

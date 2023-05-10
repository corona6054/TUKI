#include "../includes/consola.h"

t_log* logger;
t_config* config;

int socket_kernel;

int main(int argc, char** argv){
	char* config_path=argv[1];
	char* instruccion_path=argv[2];
	levantar_modulo(config_path);
	//t_list *lista;
	//lista = crearLista(instruccion_path);

	enviar_mensaje("SET",socket_kernel);
	finalizar_modulo();
	return 0;
}


// SUBPROGRAMAS


void levantar_modulo(char* config_path){
	logger = iniciar_logger();
	config = iniciar_config2();
	establecer_conexiones();
}

void finalizar_modulo(){
	log_destroy(logger);
	config_destroy(config);
	return;
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

t_config* iniciar_config(char* config_path)
{
	t_config* nuevo_config;

	nuevo_config = config_create(config_path);

	if (nuevo_config == NULL){
		printf("Error al crear el nuevo config\n");
		exit(2);
	}

	return nuevo_config;
}

t_config* iniciar_config2(void)
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

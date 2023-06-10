#include "../includes/cpu.h"

int main(void){
	levantar_modulo();

	/*
	//recv(socket_kernel, &cod_op,sizeof(), MSG_WAITALL)// Aca deberia recibir la instruccion y los datos necesarios
	int code_op; //Solo la inicializo para que no me tire error

	switch(code_op){
		case SET:
			//ejecutar_set(ax, "0000", 4); Esto era solo para la prueba y es un ejemplo de lo que habria que enviarle a la funcion
			//ejecutar_set(registro,valor,tamanio); Lo dejo comentado porque me tienen que mandar los valores
		break;
		case YIELD:
			ejecutar_yield();
		;break;
		case EXIT:
			ejecutar_exit();
		break;
	}
	*/
	finalizar_modulo();

	return 0;
}

// SUBPROGRAMAS

void levantar_modulo(){
	logger = iniciar_logger();
	config = iniciar_config();
	levantar_config();
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
	nuevo_logger = log_create("cpu.log", "Cpu", 1, LOG_LEVEL_INFO);

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

void levantar_config(){
	config_cpu.retardo = config_get_int_value(config,"RETARDO_INSTRUCCION");
	config_cpu.ip_memoria = config_get_string_value(config,"IP_MEMORIA");
	config_cpu.puerto_memoria = config_get_int_value(config,"PUERTO_MEMORIA");
	config_cpu.puerto_escucha = config_get_int_value(config,"PUERTO_ESCUCHA");
	config_cpu.tam_max_segmento = config_get_int_value(config,"TAM_MAX_SEGMENTO");
}

void conectarse_con_memoria(){
	socket_memoria = crear_conexion(config_cpu.ip_memoria, config_cpu.puerto_memoria);
	log_info(logger,"Conectado con memoria");
}

void establecer_conexiones()
{
	pthread_t conexion_memoria;
	pthread_create(&conexion_memoria, NULL, (void *) conectarse_con_memoria, NULL);
	pthread_detach(conexion_memoria);

	server_fd = abrir_servidor(logger,config);
	kernel_fd = esperar_cliente(server_fd, logger);

}

void ejecutar_set(char registro[], char valor[], int tamanio){

	//verificar tamaño mas adelante
	//fijarme el tiempo de retraso

	for (int i=0; i < tamanio; i++){
		registro[i] = valor[i];
	}
}


void ejecutar_exit(){
	//int terminalo = 5
	//send(socket_kernel, &contexto_ejecucion, sizeof(), NULL);
	//send(socket_kernel, &terminalo, sizeof(int), NULL);
	//Envio el contexto de ejecucion y un valor para que el kernel sepa que tiene que mandar terminar la conexion


}


void ejecutar_yield(){
	//int mandalo_al_final = 5
		//send(socket_kernel, &contexto_ejecucion, sizeof(), NULL);
		//send(socket_kernel, &mandalo_al_final, sizeof(int), NULL);
	//Envio el contexto de ejecucion y un valor para que el kernel sepa que tiene que mandar la instruccio al final de la cola

	/*switch(contexto.algoritmo){
		case FIFO:
		//int valorandasaber
		//send(socket_kernel, &valorandasaber, sizeof(), NULL);
	}Esto es en caso de que que  me mande si es FIFO, HRRN, etc*/


}

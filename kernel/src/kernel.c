#include "../includes/kernel.h"

t_log* logger;
t_config* config;

int socket_memoria;
int socket_fileSystem;
int socket_cpu;

int server_fd;

int consola_fd;

sem_t* sem_debug;

int main(void){

	sem_init(&sem_debug, 0, 0);

	levantar_modulo();
	while(1);
	//sleep(10000);
	/*
	procesosNuevos;
	procesosReady;
	procesosExec;
	procesosBloqueados;
	procesosFinalizados;
	*/

	finalizar_modulo();
	return 0;
}


// SUBPROGRAMAS

void levantar_modulo(){
	logger = iniciar_logger();
	config = iniciar_config();
	establecer_conexiones();
	return;
}

void finalizar_modulo(){
	log_destroy(logger);
	config_destroy(config);
	return;
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
	//socket_memoria = conectarse_a("MEMORIA",config);
	//socket_fileSystem = conectarse_a("FILESYSTEM",config);
	//socket_cpu = conectarse_a("CPU",config);

	server_fd = abrir_servidor(logger,config);
	
	pthread_t manejo_consolas;
	pthread_create(&manejo_consolas, NULL, (void *) manejar_clientes, (void *) server_fd);
	pthread_detach(manejo_consolas);
	//sem_wait(&sem_debug);
	

}


void manejar_clientes(int server_fd){
	//thread para esperar clientes
	while(1){
		t_conexiones conexiones;
		conexiones.socket = esperar_cliente(server_fd, logger);
		log_info("Salio de esperar cliente",logger);
		conexiones.socket_anterior = 0;
		// threads para recepcion de info de las consolas
		manejar_conexion_con_consola(&conexiones);
		/*
		pthread_t t;
		pthread_create(&t, NULL, (void *) manejar_conexion_con_consola, (void *) &conexiones);
		pthread_detach(t);
		*/
	}
	//sem_post(&sem_debug);
}

void manejar_conexion_con_consola(t_conexiones* conexiones){
	printf("Abrio el hilo de manejar conexion con consola");
	int socket = conexiones->socket;
	int socket_anterior = conexiones->socket_anterior;
	printf("Antes de recibir operacion");
	int cod_op = recibir_operacion(socket);
	log_info("Recibimos operacion: %s", cod_op, logger);
	



	// Aca recibiriamos lo de conosola, que nos tiene que mandar ...
	/*
	t_list* lista_de_instrucciones = list_create();

	switch(cod_op){
		case 0:
			break;
		case 1:
			lista_de_instrucciones = recibir_paquete(socket);
			crear_pcb(lista_de_instrucciones);
			break;
		default:
			break;
	}
	*/
}


void establecer_conexiones_2()
{
	//socket_memoria = conectarse_a("MEMORIA",config);
	//socket_fileSystem = conectarse_a("FILESYSTEM",config);
	//socket_cpu = conectarse_a("CPU",config);

	server_fd = abrir_servidor(logger,config);
	
	while (1){
		pthread_t manejo_consolas;
		pthread_create(&manejo_consolas, NULL, (void *) manejar_cliente, (void *) server_fd);
		pthread_detach(manejo_consolas);
	}
}

// Se conecta con el servidor y maneja los mensajes
void manejar_cliente(t_conexiones conexiones){
	conexiones.socket = esperar_cliente(server_fd, logger);
	char* cod_op = recibir_operacion_2(conexiones.socket);
	
	//printf()
}


pcb crear_pcb(t_list *instrucciones){
	pcb pcb;
	return pcb;
}

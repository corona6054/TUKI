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
	

}


void manejar_clientes(int server_fd){
	//thread para esperar clientes
	while(1){
		t_conexiones conexiones;
		conexiones.socket = esperar_cliente(server_fd, logger);
		conexiones.socket_anterior = 0;
		// threads para recepcion de info de las consolas
		pthread_t t;
		pthread_create(&t, NULL, (void *) manejar_conexion_con_consola, (void *) &conexiones);
		pthread_detach(t);
	}
}

void manejar_conexion_con_consola(t_conexiones* conexiones){
	int socket = conexiones->socket;
	int socket_anterior = conexiones->socket_anterior;

	int cod_op = recibir_operacion(socket);
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
	
}


pcb crear_pcb(t_list *instrucciones){
	pcb pcb;
	return pcb;
/*
Program_counter: Número de la próxima instrucción a ejecutar.
Registros de la CPU: Estructura que contendrá los valores de los registros de uso general de la CPU.
Tabla de Segmentos: Contendrá ids, direcciones base y tamaños de los segmentos de datos del proceso.
Estimado de próxima ráfaga: Estimación utilizada para planificar los procesos en el algoritmo HRRN, la misma tendrá un valor inicial definido por archivo de configuración y será recalculada bajo la fórmula de promedio ponderado vista en clases.
Tiempo de llegada a ready: Timestamp en que el proceso llegó a ready por última vez (utilizado para el cálculo de tiempo de espera del algoritmo HRRN).
Tabla de archivos abiertos: Contendrá la lista de archivos abiertos del proceso con la posición del puntero de cada uno de ellos.
*/
}

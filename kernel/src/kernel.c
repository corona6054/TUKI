#include "../includes/kernel.h"

t_log* logger;
t_config* config;


int socket_memoria;
int socket_fileSystem;
int socket_cpu;

int server_fd;

int consola_fd;

sem_t* sem_debug;

int pid = 0;

int ejecutando = 0;

t_list* procesosNuevos;
t_list*	procesosReady;
t_list*	procesosBloqueados;
t_list*	procesosFinalizados;

int main(void){

	sem_init(&sem_debug, 0, 0);

	levantar_modulo();
	procesosNuevos = list_create();
	procesosReady = list_create();
	while(1);

	//sleep(10000);

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
	levantar_config();
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

void levantar_config(){
	config_kernel.ip_memoria = config_get_string_value(config,"IP_MEMORIA");

	config_kernel.puerto_escucha = config_get_int_value(config,"PUERTO_ESCUCHA");
	config_kernel.algoritmo = config_get_string_value(config,"ALGORITMO_PLANIFICACION");
	config_kernel.grado_max_multi = config_get_int_value(config,"GRADO_MAX_MULTIPROGRAMACION");

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


		pthread_t t;
		pthread_create(&t, NULL, (void *) manejar_conexion_con_consola, (void *) &conexiones);
		pthread_detach(t);
	}
	//sem_post(&sem_debug);
}t_list* recibir_paquete(int socket_cliente)
{
	int size;
	int desplazamiento = 0;
	void * buffer;
	t_list* valores = list_create();
	int tamanio;

	buffer = recibir_buffer(&size, socket_cliente);
	while(desplazamiento < size)
	{
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
		desplazamiento+=sizeof(int);
		char* valor = malloc(tamanio);
		memcpy(valor, buffer+desplazamiento, tamanio);
		desplazamiento+=tamanio;
		list_add(valores, valor);
	}
	free(buffer);
	return valores;
}

void manejar_conexion_con_consola(t_conexiones* conexiones){
	t_list* instrucciones ;
	int a = recibir_operacion(conexiones->socket);
	instrucciones = recibir_paquete(conexiones->socket);
	log_info(logger, "size %d", list_size(instrucciones));
	void *instr = list_get(instrucciones,0);
	int primera;
	memcpy(&primera, instr, sizeof(int));
	log_info(logger, "primera instr %d", primera);

	int estado_anterior = -1;
	pcb pcb = crear_pcb(instrucciones);
	list_add(procesosNuevos,&pcb);

	if(ejecutando < config_kernel.grado_max_multi){
		ejecutando++;
		list_remove(procesosNuevos,0);
		estado_anterior = pcb.estado;
		pcb.estado = READY;
		list_add(procesosReady,&pcb);
		//log_info("Cola Ready %s: , ")
		//mandar msje a memoria para que inicialice sus estructuras necesarias y obtenga la tabla de segmentos inicial
		log_info("PID: %d - Estado Anterior: %d - Estado Actual: %d", logger, pcb.pid, estado_anterior, pcb.estado); //no va a terminar estando aca este log info
	}

	if (strcmp(config_kernel.algoritmo, "FIFO") == 0){
		planificacionFIFO();
	}
	else if(strcmp(config_kernel.algoritmo, "HRRN") == 0){
		planificacionHRRN();
	}
}
pcb crear_pcb(t_list *instrucciones){
	pcb pcb;

	pcb.pid = pid;
	pcb.lista_de_instrucciones = instrucciones;
	pcb.program_counter = 0;
	//pcb.registro_cpu = inicializar_registros();
    pcb.tabla_segmentos = list_create();
	pcb.estimado_prox_rafaga = config_kernel.est_inicial;
	pcb.tiempo_llegada_ready = -1;
	t_list* archivos_abiertos = list_create();
	pcb.estado = NEW;

	log_info("Se crea el proceso %d en NEW", logger, pcb.pid);

	pid++;
	return pcb;
}
/*
registros inicializar_registros(){
	registros registros;

	registros.AX = "XXXX";
	registros.BX = "XXXX";
	registros.CX = "XXXX";
	registros.DX = "XXXX";

	registros.EAX = "XXXXXXXX";
	registros.EBX = "XXXXXXXX";
	registros.ECX = "XXXXXXXX";
	registros.EDX = "XXXXXXXX";

	registros.RAX = "XXXXXXXXXXXXXXXX";
	registros.RBX = "XXXXXXXXXXXXXXXX";
	registros.RCX = "XXXXXXXXXXXXXXXX";
	registros.RDX = "XXXXXXXXXXXXXXXX";

	return registros;
}
*/
void planificacionFIFO(){
	pcb *pcb_a_ejecutar = list_remove(procesosReady,0);

	while(!list_is_empty(pcb_a_ejecutar->lista_de_instrucciones)){
		Instruction *instruccion_a_ejecutar = list_remove(pcb_a_ejecutar->lista_de_instrucciones,0);


		switch(instruccion_a_ejecutar->instruccion){
				case F_READ:

		            break;
		        case F_WRITE:

		            break;
		        case SET:
		        	printf("entro en set");
		            break;
		        case MOV_IN:
		            break;
		        case MOV_OUT:
		            break;
		        case F_TRUNCATE:
		            break;
		        case F_SEEK:
		            break;
		        case CREATE_SEGMENT:
		            break;
		        case IO:
		            break;
		        case WAIT:
		            break;
		        case SIGNAL:
		            break;
		        case F_OPEN:
		            break;
		        case F_CLOSE:
		            break;
		        case DELETE_SEGMENT:
		            break;
		        case YIELD:

		            break;
		        case EXIT:
		            //instructionCount++;
		            return;

		            break;

		        default:
		            break;
		        }

		}
}

void planificacionHRRN(){
	return;
}

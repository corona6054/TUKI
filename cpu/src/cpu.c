#include "../includes/cpu.h"

int main(void){

	sem_init(&sem_conexion, 0, 0);

	levantar_modulo();

	sem_wait(&sem_conexion);

	deserializar_cde(); // recibe, deserializa y printea el paquete
	/*
	t_list contexto_ejecucion;
	recibir_contexto();
	//Aca se tiene que deserializar el contexto

	int instruccion = list_get(contexto_de_ejecucion.lista_de_instrucciones, contexto_de_ejecucion.pc);

	int code_op; //Solo la inicializo para que no me tire error

	switch(instruccion.instruccion){
	switch(code_op){
		case SET:
			//ejecutar_set(ax, "0000", 4); Esto era solo para la prueba y es un ejemplo de lo que habria que enviarle a la funcion
			//ejecutar_set(registro,valor,tamanio); Lo dejo comentado porque me tienen que mandar los valores
			//
			//ejecutar_set(instruccion.string1,instruccion.string2,tamanio)
		break;
		case YIELD:
			ejecutar_yield();
		break;
		case EXIT:
			ejecutar_exit();
		break;
		case IO:
			ejecutar_io(tiempo);
		break;
		case SIGNAL:
			ejecutar_signal(recurso);
		break;
		case WAIT:
			ejecutar_wait(recurso);
		break;
		case F_OPEN:
			ejecutar_fopen();
		break;
		case F_CLOSE:
			ejecutar_fclose();
		break;
		case F_SEEK:
			ejecutar_fseek();
		break;
		case F_READ:
			ejecutar_fread();
		break;
		case F_WRITE:
			ejecutar_fwrite();
		break;
		case F_TRUNCATE:
			ejecutar_ftruncate();
		break;
		case CREATE_SEGMENT:
			ejecutar_createsegment();
		break;
		case DELETE_SEGMENT:
			ejecutar_deletesegment();
		break;
	}
	*/

	//finalizar_modulo();
	
	while(1);
	/*
	char *reg = "AX";
	log_info(logger,"tam registro: %d",tamanioRegistro(reg));
	*/
	return 0;
}

// SUBPROGRAMAS

/*
void recibir_contexto(){
	int op = recibir_operacion(kernel_fd);
	contexto_ejecucion = recibir_paquete(kernel_fd)
}
*/

int tamanioRegistro(char *registro){
	if(strcmp(registro,"AX") == 0)
		return 4;
}

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
	/*
	pthread_t conexion_memoria;
	pthread_create(&conexion_memoria, NULL, (void *) conectarse_con_memoria, NULL);
	pthread_detach(conexion_memoria);
	*/
	server_fd = abrir_servidor(logger,config);
	kernel_fd = esperar_cliente(server_fd, logger);
	if (kernel_fd == -1)
	{
		log_info(logger, "No se conecto el kernel");
	}
	else{
		sem_post(&sem_conexion);
		log_info(logger, "Se conecto el kernel");
	}

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

	//contexto_de_ejecucion.estados = EXIT
	//esta es una opcion o mande un mensaje para que kernel cambie el estado a exit (el enunciado dice que es un syscall)

	//enviar_contexto_de_ejecucion();

}


void ejecutar_yield(){
	//int mandalo_al_final = 5
		//send(socket_kernel, &contexto_ejecucion, sizeof(), NULL);
		//send(socket_kernel, &mandalo_al_final, sizeof(int), NULL);
	//Envio el contexto de ejecucion y un valor para que el kernel sepa que tiene que mandar la instruccion al final de la cola

	/*switch(contexto.algoritmo){
		case FIFO:
		//int valorandasaber
		//send(socket_kernel, &valorandasaber, sizeof(), NULL);
	}Esto es en caso de que que  me mande si es FIFO, HRRN, etc*/

	//creo que tengo que mandar un mensaje para ver que tienen que hacer
}

void ejecutar_io(int tiempo_bloqueo){
	//yo cambio el estado a bloqueado o mando el contexto con el tiempo de bloqueo y lo cambia el kernel
	//como mando el tiempo de bloqueo??

}

void ejecutar_signal(/*recurso*/){
	//tengo que tener los recursos en un struct o es un tipo de variable?
	//como le mandamos a kernel el recurso, como mensaje simple?

}

void ejecutar_wait(/*recurso*/){
	//tengo que tener los recursos en un struct o es un tipo de variable?
	//como le mandamos a kernel el recurso, como mensaje simple?
}

void ejecutar_fopen(/*archivo*/){

}
void ejecutar_fclose(/*archivo*/){

}
void ejecutar_fread(/*archivo, int direccion_logica, int cant_bytes*/){

}
void ejecutar_fwrite(/*archivo, int direccion_logica, int cant_bytes*/){

}
void ejecutar_fseek(/*archivo, int posicion*/){

}
void ejecutar_ftruncate(/*archivo, int tamanio*/){

}
void ejecutar_createsegment(/*int id_segmento, int tamanio*/){

}
void ejecutar_deletesegment(/*int id_segmento*/){

}


void deserializar_cde(){
	t_paquete* paquete = crear_paquete();

	log_info(logger,"Entre a deserializar_cde");
	
	// Primero recibimos el codigo de operacion
	recv(kernel_fd, &(paquete->codigo_operacion), sizeof(uint8_t), MSG_WAITALL);


	log_info(logger,"Recibi el codigo de operacion");

	// Después ya podemos recibir el buffer. Primero su tamaño seguido del contenido
	recv(kernel_fd, &(paquete->buffer->size), sizeof(uint32_t), MSG_WAITALL);
	log_info(logger,"Recibi el tamanio");

	paquete->buffer->stream = malloc(paquete->buffer->size);

	recv(kernel_fd, paquete->buffer->stream, paquete->buffer->size, MSG_WAITALL);
	log_info(logger,"Recibi el paquete");
	// ACA VA A HABER UN SWITCH DE COD OP PARA VER SI LE LLEGA DEL KERNEL O DE MEMORIA
	
	Cde_serializado* cde_recibido = malloc(sizeof(Cde_serializado));

	void* stream = paquete->buffer->stream;

	log_info(logger,"Empiezo a deserializar");
	// Deserializamos los campos que tenemos en el buffer
    memcpy(&(cde_recibido->pid), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    
	memcpy(&(cde_recibido->program_counter), stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);

	memcpy(&cde_recibido->tam_lista_instrucciones, stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);
	
	cde_recibido->lista_de_instrucciones = malloc(cde_recibido->tam_lista_instrucciones);

	memcpy(cde_recibido->lista_de_instrucciones, stream,  cde_recibido->tam_lista_instrucciones);
	stream += cde_recibido->tam_lista_instrucciones;
	
	memcpy(&(cde_recibido->registrosCpu), stream,  112);
	stream += 112;

	memcpy(&(cde_recibido->tam_tabla_segmentos), stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);
	
	cde_recibido->tabla_segmentos = malloc(cde_recibido->tam_tabla_segmentos);

	memcpy(cde_recibido->tabla_segmentos, stream, cde_recibido->tam_tabla_segmentos);
	stream += cde_recibido->tam_tabla_segmentos;

	memcpy(&cde_recibido->estado, stream, sizeof(uint8_t));
	stream += sizeof(uint8_t);

	log_info(logger,"Termine de deserializar.");

	log_info(logger,"PID: %d", cde_recibido->pid);

	log_info(logger, "PC: %d", cde_recibido->program_counter);
   
	log_info(logger,"Estado: %d", cde_recibido->estado);
}

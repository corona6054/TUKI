#include "../includes/cpu.h"


int main(void){
	//sem_init(&prueba1cpu, 0, 0);
	
	levantar_modulo();
	

	/*t_buffer* buffer = crear_buffer_nuestro();

	// Primero recibo el tamanio del buffer
	sem_wait(&prueba1cpu);
	//uint32_t tamanio = 10;
	recv(kernel_fd, &(buffer->size), sizeof(uint32_t), MSG_WAITALL);
	log_info(logger,"Tamanio del buffer: %d", buffer->size);

	// Reservo espacio en memoria
	buffer->stream = malloc(buffer->size);

	// Recibo el stream
	recv(kernel_fd, buffer->stream, buffer->size, MSG_WAITALL);
	log_info(logger,"Ya recibi el stream.");
	
	//contexto_de_ejecucion cde_recibido= buffer_read_cde(buffer);

	//mostrar_instrucciones(cde_recibido.lista_de_instrucciones);

	while(1);
	*/
	t_instruction *inst1 ;
	inst1 = malloc(sizeof(t_instruction));
	inst1->instruccion = MOV_IN;
	inst1->string1 = malloc(sizeof("AX"));
	inst1->string2 = malloc(sizeof("HOLA"));
	strcpy(inst1->string1,"RAX");
	strcpy(inst1->string2,"HOLA-SOY-NICOLAS");


	t_list* instrucciones = list_create();
	list_add(instrucciones,inst1);

	t_instruction *inst2 ;
	inst2 = malloc(sizeof(t_instruction));
	inst2->instruccion = IO;

	list_add(instrucciones,inst2);

	t_registros registros;

	ejecutar_set("AX", "XXXX", 4, &registros);
	ejecutar_set("BX", "1XX1", 4, &registros);
	ejecutar_set("CX", "XXX2", 4, &registros);
	ejecutar_set("DX", "XXX3", 4, &registros);
	ejecutar_set("EAX", "XXXXXXX4", 4, &registros);
	ejecutar_set("EBX", "XXXXXXX5", 4, &registros);
	ejecutar_set("ECX", "XXXXXXX6", 4, &registros);
	ejecutar_set("EDX", "XXXXXXX7", 4, &registros);
	ejecutar_set("RAX", "XXXXXXXXXXX8", 4, &registros);
	ejecutar_set("RBX", "XXXXXXXXXXX9", 4, &registros);
	ejecutar_set("RCX", "XXXXXXXXXX10", 4, &registros);
	ejecutar_set("RDX", "XXXXXXXXXX11", 4, &registros);



	t_segmento *segmento1;
	segmento1 = malloc(sizeof(t_segmento));
	segmento1->id = 1;
	segmento1->direccion_base = 0;
	segmento1->tamanio_segmentos = 137;

	t_segmento *segmento2;
		segmento2 = malloc(sizeof(t_segmento));
		segmento2->id = 2;
		segmento2->direccion_base = 100;
		segmento2->tamanio_segmentos = 19;

	t_list* segmentos = list_create();
	list_add(segmentos, segmento1);
	list_add(segmentos, segmento2);

	t_cde cde_recibido = {
				1,
				instrucciones,
				0,
				registros,
				segmentos
	};





	//printf("%d", dir_fisica);
	//Instruction* contexto_instruccion = list_get(cde_recibido.lista_de_instrucciones, cde_recibido.program_counter);
	//switch_instruccion(contexto_instruccion, cde_recibido);

	/*int tamanio = tamanioRegistro("RAX");
	int dir_fisica = calcular_dir_fisica(130, cde_recibido,tamanio);
	if (dir_fisica == -1){
		printf("Segmentation fault");
	}else {printf("%d", dir_fisica);}*/






	//finalizar_modulo();*/
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
	if(strcmp(registro,"AX") == 0 || strcmp(registro,"BX") == 0 || strcmp(registro,"CX") == 0 || strcmp(registro,"DX") == 0)
		return sizeof("XXXX");
	else if(strcmp(registro,"EAX") == 0 || strcmp(registro,"EBX") == 0 || strcmp(registro,"ECX") == 0 || strcmp(registro,"EDX") == 0)
			return sizeof("XXXXXXXX");
		else if(strcmp(registro,"RAX") == 0 || strcmp(registro,"RBX") == 0 || strcmp(registro,"RCX") == 0 || strcmp(registro,"RDX") == 0)
				return sizeof("XXXXXXXXXXXXXXXX");
	else return -1;
}

void levantar_modulo(){
	logger = iniciar_logger();
	config = iniciar_config();
	levantar_config();
	//establecer_conexiones();
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
	server_fd = iniciar_servidor(logger,config);
	kernel_fd = esperar_cliente(server_fd, logger);
	if (kernel_fd == -1)
	{
		log_info(logger, "No se conecto el kernel");
	}
	else{
		//sem_post(&sem_conexion);
		log_info(logger, "Se conecto el kernel");
		sem_post(&prueba1cpu);
	}

}

void switch_instruccion(t_instruction* instruccion, t_cde cde){
	t_buffer* buffer = crear_buffer_nuestro();
	switch(instruccion->instruccion){
		case SET:
			//int tamanio = tamanioRegistro(instruccion->string1);
			printf("entre a set");
			ejecutar_set(instruccion->string1,instruccion->string2,4, &(cde.registros_cpu));
			t_registros registros = cde.registros_cpu;
			for(int i=0;i<16;i++){
				printf("%c",registros.RAX[i]);
			}
			cde.program_counter = cde.program_counter + 1;
			//Instruction* contexto_instruccion = list_get(cde->lista_de_instrucciones, cde->program_counter);
			//switch_instruccion(contexto_instruccion, cde);
		break;
		case MOV_IN:
			int tamanio = tamanioRegistro(instruccion->string1);
			printf("%d\n",tamanio);
			int dir_fisica = calcular_dir_fisica(130, cde,tamanio);
			if (dir_fisica == -1){
				printf("Segmentation fault");
				//mando a kernel el contexto de ejecucion
			}else {printf("%d\n", dir_fisica);/*Aca mando a memoria la info*/

				char* a_guardar = "HOLA-SOY-NICOLAS"; // = lo que me llega de memoria
				ejecutar_set(instruccion->string1, a_guardar,4, &(cde.registros_cpu));
				t_registros registros2 = cde.registros_cpu;
				for(int i=0;i<16;i++){
					printf("%c",registros2.RAX[i]);
				}
			}
		break;
		case MOV_OUT:
			tamanio = tamanioRegistro(instruccion->string1);
			printf("%d\n",tamanio);
			dir_fisica = calcular_dir_fisica(130, cde,tamanio);
			if (dir_fisica == -1){
				printf("Segmentation fault");
				//mando a kernel el contexto de ejecucion
			}else {
				printf("%d\n", dir_fisica);
				//hago la funcion para enviar la info

			}

		break;
		case YIELD:
			printf("entre a yield");
			cde.program_counter = cde.program_counter + 1;
			//buffer_write_cde(buffer, cde);
			//ejecutar_yield();
		break;
		case EXIT:
			cde.program_counter = cde.program_counter + 1;
			//buffer_write_cde(buffer, cde);
			//ejecutar_exit();
		break;
		case IO:
			printf("entre a IO");
			cde.program_counter = cde.program_counter + 1;
			//buffer_write_cde(buffer, cde);
			//ejecutar_io(tiempo);
		break;
		case SIGNAL:
			cde.program_counter = cde.program_counter + 1;
			//buffer_write_cde(buffer, cde);
			//ejecutar_signal(recurso);
		break;
		case WAIT:
			cde.program_counter = cde.program_counter + 1;
			//buffer_write_cde(buffer, cde);
			//ejecutar_wait(recurso);
		break;
		case F_OPEN:
			cde.program_counter = cde.program_counter + 1;
			buffer_write_cde(buffer, cde);
			//ejecutar_fopen();
		break;
		case F_CLOSE:
			cde.program_counter = cde.program_counter + 1;
			//buffer_write_cde(buffer, cde);
			//ejecutar_fclose();
		break;
		case F_SEEK:
			cde.program_counter = cde.program_counter + 1;
			//buffer_write_cde(buffer, cde);
			//ejecutar_fseek();
		break;
		case F_READ:
			cde.program_counter = cde.program_counter + 1;
			//buffer_write_cde(buffer, cde);
			//ejecutar_fread();
		break;
		case F_WRITE:
			cde.program_counter = cde.program_counter + 1;
			//buffer_write_cde(buffer, cde);
			//ejecutar_fwrite();
		break;
		case F_TRUNCATE:
			cde.program_counter = cde.program_counter + 1;
			//buffer_write_cde(buffer, cde);
			//ejecutar_ftruncate();
		break;
		case CREATE_SEGMENT:
			cde.program_counter = cde.program_counter + 1;
			//buffer_write_cde(buffer, cde);
			//ejecutar_createsegment();
		break;
		case DELETE_SEGMENT:
			cde.program_counter = cde.program_counter + 1;
			//buffer_write_cde(buffer, cde);
			//ejecutar_deletesegment();
		break;
	}
}

void ejecutar_set(char registro[], char valor[], int tamanio, t_registros *registros){
	if (strcmp(registro,"AX") == 0) {strcpy(registros->AX, valor);}
	if (strcmp(registro,"BX") == 0) {strcpy(registros->BX, valor);}
	if (strcmp(registro,"CX") == 0) {strcpy(registros->CX, valor);}
	if (strcmp(registro,"DX") == 0) {strcpy(registros->DX, valor);}
	if (strcmp(registro,"EAX") == 0) {strcpy(registros->EAX, valor);}
	if (strcmp(registro,"EBX") == 0) {strcpy(registros->EBX, valor);}
	if (strcmp(registro,"ECX") == 0) {strcpy(registros->ECX, valor);}
	if (strcmp(registro,"EDX") == 0) {strcpy(registros->EDX, valor);}
	if (strcmp(registro,"RAX") == 0) {strcpy(registros->RAX, valor);}
	if (strcmp(registro,"RBX") == 0) {strcpy(registros->RBX, valor);}
	if (strcmp(registro,"ECX") == 0) {strcpy(registros->RCX, valor);}
	if (strcmp(registro,"RDX") == 0) {strcpy(registros->RDX, valor);}
}

void sacar_de_registro(char registro[], int dir_fisica, int tamanio, t_registros *registros, int pid){
	if (strcmp(registro,"AX") == 0)  { }
	if (strcmp(registro,"BX") == 0)  { }
	if (strcmp(registro,"CX") == 0)  { }
	if (strcmp(registro,"DX") == 0)  { }
	if (strcmp(registro,"EAX") == 0) { }
	if (strcmp(registro,"EBX") == 0) { }
	if (strcmp(registro,"ECX") == 0) { }
	if (strcmp(registro,"EDX") == 0) { }
	if (strcmp(registro,"RAX") == 0) { }
	if (strcmp(registro,"RBX") == 0) { }
	if (strcmp(registro,"ECX") == 0) { }
	if (strcmp(registro,"RDX") == 0) { }
}


int calcular_dir_fisica(int dir_logica, t_cde cde,int tamanio){
	int tam_max_segmento = config_cpu.tam_max_segmento;
	int num_seg = floor(dir_logica/tam_max_segmento);
	int desplaz_segmento = dir_logica % tam_max_segmento;

	t_segmento* segmento = list_get(cde.tabla_segmentos, num_seg);

	if (segmento->tamanio_segmentos < desplaz_segmento + tamanio){return -1;}

	return segmento->direccion_base + desplaz_segmento;

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

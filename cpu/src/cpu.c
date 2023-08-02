#include "../includes/cpu.h"


int main(void){

	levantar_modulo();
	
	iniciar_modulo();



	while(1);
	return 0;
}

// SUBPROGRAMAS
// UTILS INICIAR MODULO -----------------------------------------------------------------
void levantar_modulo(){
	logger = iniciar_logger();
	config = iniciar_config();
	levantar_config();

	inicializar_semaforos();
	establecer_conexiones();
}

void inicializar_semaforos(){
	sem_init(&espero_cde, 0, 1);
	sem_init(&leer_siguiente_instruccion, 0 , 0);
}

void iniciar_modulo(){
	pthread_t conexion_kernel;
	pthread_create(&conexion_kernel, NULL, (void *) recibir_cde, NULL);
	pthread_detach(conexion_kernel);
	
	/*
	pthread_t ejecucion;
	pthread_create(&ejecucion, NULL, (void *) ejecutar_proceso, NULL);
	pthread_detach(ejecucion);
	*/
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
// FIN UTILS INICIAR MODULO -------------------------------------------------------------


// UTILS CONEXIONES ---------------------------------------------------------------------
void conectarse_con_memoria(){
	socket_memoria = crear_conexion(config_cpu.ip_memoria, config_cpu.puerto_memoria);
	log_info(logger,"Conectado con memoria");
}

void establecer_conexiones(){
	/*
	pthread_t conexion_memoria;
	pthread_create(&conexion_memoria, NULL, (void *) conectarse_con_memoria, NULL);
	pthread_detach(conexion_memoria);
	*/

	server_fd = iniciar_servidor(logger,config_cpu.puerto_escucha);

	kernel_fd = esperar_cliente(server_fd, logger);
	
	if (kernel_fd >= 0){
		log_info(logger, "Conectado con kernel");
		if (recibir_handshake(kernel_fd) == HANDSHAKE){
			enviar_handshake(kernel_fd);
			log_info(logger, "Handshake con CPU realizado.");

			sem_post(&espero_cde);
			log_info(logger, "Posteo(espero_cde)");
		}
	}else
		log_info(logger, "Error al conectar con kernel");
}

void recibir_cde(){
	while(1){
		sem_wait(&espero_cde);
		t_buffer* buffer = recibir_buffer(kernel_fd);
		cde_en_ejecucion = 	buffer_read_cde(buffer);
		log_info(logger, "Proceso en ejecucion: %d", cde_en_ejecucion.pid);
		cde_en_ejecucion.pid++;

		enviar_cde();
		//sem_post(&leer_siguiente_instruccion);
	}
}

void enviar_cde(){
	t_buffer* buffer = crear_buffer_nuestro();
	buffer_write_cde(buffer, cde_en_ejecucion);
	log_info(logger, "A punto de enviar el cde a kernel");
	enviar_buffer(buffer, kernel_fd);
	log_info(logger, "Envie el cde a kernel");
	sem_post(&espero_cde);
}
// UTILS FIN CONEXIONES -----------------------------------------------------------------


// UTILS INSTRUCCIONES (GENERAL) --------------------------------------------------------
void ejecutar_proceso(){
	while(1){
		sem_wait(&leer_siguiente_instruccion);
		int indice = cde_en_ejecucion.program_counter;
		t_instruction* instruccion_actual = (cde_en_ejecucion.lista_de_instrucciones, indice);
		evaluar_instruccion(instruccion_actual);
		cde_en_ejecucion.program_counter++;
	}
}




void evaluar_instruccion(t_instruction* instruccion){
	t_buffer* buffer = crear_buffer_nuestro();
	switch(instruccion->instruccion){
		case SET:
			log_info(logger, "PID: %d - EJECUTANDO : SET - PARAMETROS: ( %s , %s )", cde_en_ejecucion.pid, instruccion->string1, instruccion->string2);
			ejecutar_set(instruccion->string1,instruccion->string2, cde_en_ejecucion.registros_cpu);
			t_registros registros = cde_en_ejecucion.registros_cpu;
			log_info(logger, "%s", registros.RAX);	
			
			sem_post(&leer_siguiente_instruccion);
		break;
		case MOV_IN:
			log_info(logger, "PID: %d - EJECUTANDO : MOVE IN - PARAMETROS: ( %s , %d )", cde_en_ejecucion.pid, instruccion->string1, instruccion->numero1);
			ejecutar_move_in(instruccion->string1, instruccion->numero1);
			t_registros registros2 = cde_en_ejecucion.registros_cpu;
			log_info(logger, "%s", registros2.RAX);
			
			sem_post(&leer_siguiente_instruccion);
		break;
		case MOV_OUT:
			log_info(logger, "PID: %d - EJECUTANDO : MOVE OUT - PARAMETROS: ( %d , %s )", cde_en_ejecucion.pid, instruccion->numero1, instruccion->string1);
			ejecutar_move_out(instruccion->string1, instruccion->numero1);
			
			sem_post(&leer_siguiente_instruccion);
		break;
		case YIELD:
			log_info(logger, "PID: %d - EJECUTANDO : YIELD - PARAMETROS: ()", cde_en_ejecucion.pid);
			enviar_cde();
		break;
		case EXIT:
			log_info(logger, "PID: %d - EJECUTANDO : EXIT - PARAMETROS: ()", cde_en_ejecucion.pid);
			enviar_cde();	
		break;
		case IO:
			log_info(logger, "PID: %d - EJECUTANDO : IO - PARAMETROS: ()", cde_en_ejecucion.pid);
		
			enviar_cde();
		break;
		case SIGNAL:
			log_info(logger, "PID: %d - EJECUTANDO : SIGNAL - PARAMETROS: ()", cde_en_ejecucion.pid);

			enviar_cde();
		break;
		case WAIT:
			log_info(logger, "PID: %d - EJECUTANDO : WAIT - PARAMETROS: ()", cde_en_ejecucion.pid);
			
			enviar_cde();
		break;
		case F_OPEN:
			log_info(logger, "PID: %d - EJECUTANDO : F_OPEN - PARAMETROS: ()", cde_en_ejecucion.pid);

			enviar_cde();
		break;
		case F_CLOSE:
			log_info(logger, "PID: %d - EJECUTANDO : F_CLOSE - PARAMETROS: ()", cde_en_ejecucion.pid);

			enviar_cde();
		break;
		case F_SEEK:
			log_info(logger, "PID: %d - EJECUTANDO : F_SEEK - PARAMETROS: ()", cde_en_ejecucion.pid);

			enviar_cde();
		break;
		case F_READ:
			log_info(logger, "PID: %d - EJECUTANDO : F_READ - PARAMETROS: ()", cde_en_ejecucion.pid);

			enviar_cde();
		break;
		case F_WRITE:
			log_info(logger, "PID: %d - EJECUTANDO : F_WRITE - PARAMETROS: ()", cde_en_ejecucion.pid);

			enviar_cde();
		break;
		case F_TRUNCATE:
			log_info(logger, "PID: %d - EJECUTANDO : F_TRUNCATE - PARAMETROS: ()", cde_en_ejecucion.pid);

			enviar_cde();
		break;
		case CREATE_SEGMENT:
			log_info(logger, "PID: %d - EJECUTANDO : CREATE_SEGMENT - PARAMETROS: ()", cde_en_ejecucion.pid);

			enviar_cde();
		break;
		case DELETE_SEGMENT:
			log_info(logger, "PID: %d - EJECUTANDO : DELETE_SEGMENT - PARAMETROS: ()", cde_en_ejecucion.pid);

			enviar_cde();
		break;
	}
}
// FIN UTILS INSTRUCCIONES (GENERAL) ----------------------------------------------------


// UTILS INSTRUCCIONES (PARTICULAR) -----------------------------------------------------
void ejecutar_set(char* registro, char* valor, t_registros registros){
	usleep(config_cpu.retardo);
	if (strcmp(registro,"AX") == 0)  {strcpy(registros.AX, valor);}
	if (strcmp(registro,"BX") == 0)  {strcpy(registros.BX, valor);}
	if (strcmp(registro,"CX") == 0)  {strcpy(registros.CX, valor);}
	if (strcmp(registro,"DX") == 0)  {strcpy(registros.DX, valor);}
	if (strcmp(registro,"EAX") == 0) {strcpy(registros.EAX, valor);}
	if (strcmp(registro,"EBX") == 0) {strcpy(registros.EBX, valor);}
	if (strcmp(registro,"ECX") == 0) {strcpy(registros.ECX, valor);}
	if (strcmp(registro,"EDX") == 0) {strcpy(registros.EDX, valor);}
	if (strcmp(registro,"RAX") == 0) {strcpy(registros.RAX, valor);}
	if (strcmp(registro,"RBX") == 0) {strcpy(registros.RBX, valor);}
	if (strcmp(registro,"ECX") == 0) {strcpy(registros.RCX, valor);}
	if (strcmp(registro,"RDX") == 0) {strcpy(registros.RDX, valor);}
}




int tamanioRegistro(char* registro){
	if(strcmp(registro,"AX") == 0 || strcmp(registro,"BX") == 0 || strcmp(registro,"CX") == 0 || strcmp(registro,"DX") == 0)
		return 4;
	else if(strcmp(registro,"EAX") == 0 || strcmp(registro,"EBX") == 0 || strcmp(registro,"ECX") == 0 || strcmp(registro,"EDX") == 0)
			return 8;
		else if(strcmp(registro,"RAX") == 0 || strcmp(registro,"RBX") == 0 || strcmp(registro,"RCX") == 0 || strcmp(registro,"RDX") == 0)
				return 16;
	else return -1;
}

/*void sacar_de_registro(char registro[], int dir_fisica, t_registros *registros, int pid){
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
}*/


int calcular_dir_fisica(int dir_logica, int tamanio){
	int tam_max_segmento = config_cpu.tam_max_segmento;
	int num_seg = floor(dir_logica/tam_max_segmento);
	int desplaz_segmento = dir_logica % tam_max_segmento;

	t_segmento* segmento = list_get(cde_en_ejecucion.tabla_segmentos, num_seg);

	if (segmento->tamanio_segmentos < desplaz_segmento + tamanio){return -1;}

	return segmento->direccion_base + desplaz_segmento;

}

void ejecutar_move_in(char* registro, uint32_t dir_logica){
	int tamanio = tamanioRegistro(registro);
	printf("%d\n",tamanio);
	int dir_fisica = calcular_dir_fisica(dir_logica, tamanio);
	if (dir_fisica == -1){
		log_info(logger, "PID: %d - Error SEG_FAULT - Segmento:", cde_en_ejecucion.pid);
		//mando a kernel el contexto de ejecucion
	}else {
		printf("%d\n", dir_fisica);/*Aca mando a memoria la info*/

		char* a_guardar = "HOLA-SOY-NICOLAS"; // = lo que me llega de memoria
		ejecutar_set(registro, a_guardar, cde_en_ejecucion.registros_cpu);
	}
}

void ejecutar_move_out(char* registro, uint32_t dir_logica){
	int tamanio = tamanioRegistro(registro);
	printf("%d\n",tamanio);
	int dir_fisica = calcular_dir_fisica(dir_logica,tamanio);
	if (dir_fisica == -1){
		printf("Segmentation fault");
		//mando a kernel el contexto de ejecucion
	}else {
		printf("%d\n", dir_fisica);
		//hago la funcion para enviar la info
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
// FIN UTILS INSTRUCCIONES --------------------------------------------------------------

	/*t_instruction *inst1 ;
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
*/

#include "../includes/cpu.h"

t_registros inicializar_registros(){
	t_registros registros;

	for(int i=0;i<4;i++){
		registros.AX[i] = 'x';
		registros.BX[i] = 'x';
		registros.CX[i] = 'x';
		registros.DX[i] = 'x';
	}
	
	for(int i=0;i<8;i++){
		registros.EAX[i] = 'x';
		registros.EBX[i] = 'x';
		registros.ECX[i] = 'x';
		registros.EDX[i] = 'x';
	}

	for(int i=0;i<16;i++){
		registros.RAX[i] = 'x';
		registros.RBX[i] = 'x';
		registros.RCX[i] = 'x';
		registros.RDX[i] = 'x';
	}

	return registros;
}

int main(void){
	levantar_modulo();

	iniciar_modulo();


	while(1);
	return 0;
}


// UTILS INICIAR MODULO -----------------------------------------------------------------
void levantar_modulo(){
	logger = iniciar_logger();
	config = iniciar_config();
	levantar_config();

	inicializar_semaforos();
	establecer_conexiones();
}

void inicializar_semaforos(){
	sem_init(&necesito_enviar_cde, 0, 0);
	sem_init(&leer_siguiente_instruccion, 0 , 0);

	sem_init(&bin1_envio_cde, 0, 0);
	sem_init(&bin2_recibir_cde, 0, 1);
}

void iniciar_modulo(){
	pthread_t recibir_cde_kernel;
	pthread_create(&recibir_cde_kernel, NULL, (void *) recibir_cde, NULL);
	pthread_detach(recibir_cde_kernel);

	pthread_t envio_cde_kernel;
	pthread_create(&envio_cde_kernel, NULL, (void *) enviar_cde, NULL);
	pthread_detach(envio_cde_kernel);

	pthread_t ejecucion;
	pthread_create(&ejecucion, NULL, (void *) ejecutar_proceso, NULL);
	pthread_detach(ejecucion);
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
			log_info(logger, "Handshake con Kernel realizado.");

		}
	}else
		log_info(logger, "Error al conectar con kernel");
}

void recibir_cde(){
	while(1){
		sem_wait(&bin2_recibir_cde);
		log_info(logger, "A punto de recibir buffer.");
		t_buffer* buffer = recibir_buffer(kernel_fd);
		log_info(logger, "Recibi buffer.");
		cde_en_ejecucion = 	buffer_read_cde(buffer);
		log_info(logger, "CDE recibido, PC %d", cde_en_ejecucion->program_counter);

		destruir_buffer_nuestro(buffer);
		log_info(logger, "Destrui buffer recibido");

		sem_post(&leer_siguiente_instruccion);
		sem_post(&bin1_envio_cde);
	}
}

void enviar_cde(){
	while(1){
		sem_wait(&necesito_enviar_cde);
		sem_wait(&bin1_envio_cde);
		t_buffer* buffer = crear_buffer_nuestro();
		buffer_write_cde(buffer, *cde_en_ejecucion);
		log_info(logger, "A punto de enviar el cde a kernel, PC: %d", cde_en_ejecucion->program_counter);
		enviar_buffer(buffer, kernel_fd);
		log_info(logger, "Envie el cde a kernel");
		
		destruir_buffer_nuestro(buffer);
		log_info(logger, "Destrui buffer enviado");

		
		destruir_cde(cde_en_ejecucion);
		
		sem_post(&bin2_recibir_cde);
	}
}
// UTILS FIN CONEXIONES -----------------------------------------------------------------


// UTILS INSTRUCCIONES (GENERAL) --------------------------------------------------------
void ejecutar_proceso(){
	while(1){
		sem_wait(&leer_siguiente_instruccion);
		int indice = cde_en_ejecucion->program_counter;
		log_info(logger, "Evaluando instruccion Nro: %d  de %d", indice, cde_en_ejecucion->pid);
		t_instruction* instruccion_actual = list_get(cde_en_ejecucion->lista_de_instrucciones, indice);
		evaluar_instruccion(instruccion_actual);
		cde_en_ejecucion->program_counter++;
	}
}

void evaluar_instruccion(t_instruction* instruccion){
	switch(instruccion->instruccion){
		case SET:
			log_info(logger, "PID: %d - EJECUTANDO : SET - PARAMETROS: ( %s , %s )", cde_en_ejecucion->pid, instruccion->string1, instruccion->string2);
			ejecutar_set(instruccion->string1,instruccion->string2, cde_en_ejecucion->registros_cpu);			
			sem_post(&leer_siguiente_instruccion);
		break;
		case MOV_IN:
			log_info(logger, "PID: %d - EJECUTANDO : MOVE IN - PARAMETROS: ( %s , %d )", cde_en_ejecucion->pid, instruccion->string1, instruccion->numero1);
			ejecutar_move_in(instruccion->string1, instruccion->numero1);			
			sem_post(&leer_siguiente_instruccion);
		break;
		case MOV_OUT:
			log_info(logger, "PID: %d - EJECUTANDO : MOVE OUT - PARAMETROS: ( %d , %s )", cde_en_ejecucion->pid, instruccion->numero1, instruccion->string1);
			ejecutar_move_out(instruccion->string1, instruccion->numero1);
			sem_post(&leer_siguiente_instruccion);
		break;
		case YIELD:
			log_info(logger, "PID: %d - EJECUTANDO : YIELD - PARAMETROS: ()", cde_en_ejecucion->pid);
			sem_post(&necesito_enviar_cde);
			break;
		case EXIT:
			log_info(logger, "PID: %d - EJECUTANDO : EXIT - PARAMETROS: ()", cde_en_ejecucion->pid);
			sem_post(&necesito_enviar_cde);
		break;
		case IO:
			log_info(logger, "PID: %d - EJECUTANDO : IO - PARAMETROS: ()", cde_en_ejecucion->pid);
		
			sem_post(&necesito_enviar_cde);
		break;
		case SIGNAL:
			log_info(logger, "PID: %d - EJECUTANDO : SIGNAL - PARAMETROS: ()", cde_en_ejecucion->pid);

			sem_post(&necesito_enviar_cde);
		break;
		case WAIT:
			log_info(logger, "PID: %d - EJECUTANDO : WAIT - PARAMETROS: ()", cde_en_ejecucion->pid);
			
			sem_post(&necesito_enviar_cde);
		break;
		case F_OPEN:
			log_info(logger, "PID: %d - EJECUTANDO : F_OPEN - PARAMETROS: ()", cde_en_ejecucion->pid);

			sem_post(&necesito_enviar_cde);
		break;
		case F_CLOSE:
			log_info(logger, "PID: %d - EJECUTANDO : F_CLOSE - PARAMETROS: ()", cde_en_ejecucion->pid);

			sem_post(&necesito_enviar_cde);
		break;
		case F_SEEK:
			log_info(logger, "PID: %d - EJECUTANDO : F_SEEK - PARAMETROS: ()", cde_en_ejecucion->pid);

			sem_post(&necesito_enviar_cde);
		break;
		case F_READ:
			log_info(logger, "PID: %d - EJECUTANDO : F_READ - PARAMETROS: ()", cde_en_ejecucion->pid);

			sem_post(&necesito_enviar_cde);
		break;
		case F_WRITE:
			log_info(logger, "PID: %d - EJECUTANDO : F_WRITE - PARAMETROS: ()", cde_en_ejecucion->pid);

			sem_post(&necesito_enviar_cde);
		break;
		case F_TRUNCATE:
			log_info(logger, "PID: %d - EJECUTANDO : F_TRUNCATE - PARAMETROS: ()", cde_en_ejecucion->pid);

			sem_post(&necesito_enviar_cde);
		break;
		case CREATE_SEGMENT:
			log_info(logger, "PID: %d - EJECUTANDO : CREATE_SEGMENT - PARAMETROS: ()", cde_en_ejecucion->pid);

			sem_post(&necesito_enviar_cde);
		break;
		case DELETE_SEGMENT:
			log_info(logger, "PID: %d - EJECUTANDO : DELETE_SEGMENT - PARAMETROS: ()", cde_en_ejecucion->pid);

			sem_post(&necesito_enviar_cde);
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

int calcular_dir_fisica(int dir_logica, int tamanio){
	int tam_max_segmento = config_cpu.tam_max_segmento;
	int num_seg = floor(dir_logica/tam_max_segmento);
	int desplaz_segmento = dir_logica % tam_max_segmento;

	t_segmento* segmento = list_get(cde_en_ejecucion->tabla_segmentos, num_seg);

	if (segmento->tamanio_segmentos < desplaz_segmento + tamanio){return -1;}

	return segmento->direccion_base + desplaz_segmento;
}

void ejecutar_move_in(char* registro, uint32_t dir_logica){
	int tamanio = tamanioRegistro(registro);
	printf("%d\n",tamanio);
	int dir_fisica = calcular_dir_fisica(dir_logica, tamanio);
	if (dir_fisica == -1){
		log_info(logger, "PID: %d - Error SEG_FAULT - Segmento:", cde_en_ejecucion->pid);
		//mando a kernel el contexto de ejecucion
	}else {
		printf("%d\n", dir_fisica);/*Aca mando a memoria la info*/

		char* a_guardar = "HOLA-SOY-NICOLAS"; // = lo que me llega de memoria
		ejecutar_set(registro, a_guardar, cde_en_ejecucion->registros_cpu);
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
// FIN UTILS INSTRUCCIONES --------------------------------------------------------------
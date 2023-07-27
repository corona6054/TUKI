#include "serializacion.h"

t_paquete* crear_paquete(op_code codigo, t_buffer* buffer)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = codigo;
	paquete->buffer = buffer;
	return paquete;
}

void enviar_paquete(t_paquete* paquete, int socket_cliente)
{
	int tamanio_paquete_serializado = paquete->buffer->size + sizeof(uint8_t) + sizeof(uint32_t);
	void* paquete_serializado = serializar_paquete(paquete, tamanio_paquete_serializado);

	send(socket_cliente, paquete_serializado, tamanio_paquete_serializado, 0);

	free(paquete_serializado);
}

void* serializar_paquete(t_paquete* paquete, int tam_paquete)
{
	// Esta funcion, mete al paquete en un stream para que se pueda enviar
	void* paquete_serializado = malloc(tam_paquete);
	int desplazamiento = 0;

	// Codigo de operacion
	memcpy(paquete_serializado + desplazamiento, &(paquete->codigo_operacion), sizeof(uint8_t));
	desplazamiento+= sizeof(int);

	// Tamanio del stream
	memcpy(paquete_serializado + desplazamiento, &(paquete->buffer->size), sizeof(uint32_t));
	desplazamiento+= sizeof(int);

	// Stream en si
	memcpy(paquete_serializado + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento+= paquete->buffer->size;

	return paquete_serializado;
}


t_buffer* recibir_buffer(int socket)
{
	// Esta funcion desarma el paquete y arma el buffer que luego sera deserializado
	t_buffer* buffer = crear_buffer_nuestro();

	// Recibo el codigo de operacion
	recv(socket, &(buffer -> codigo), sizeof(uint8_t), MSG_WAITALL);

	// Recibo el tamanio del buffer y reservo espacio en memoria
	recv(socket, &(buffer -> size), sizeof(uint32_t), MSG_WAITALL);
	buffer -> stream = malloc(buffer -> size);

	// Recibo stream del buffer
	recv(socket, buffer -> stream, buffer -> size, MSG_WAITALL);

	return buffer;
}

t_buffer* crear_buffer_nuestro(){
	t_buffer* b = malloc(sizeof(t_buffer));
	b->size = 0;
	b->stream = NULL;
	b->offset = 0;
	return b;
}

// UINT32
void buffer_write_uint32(t_buffer* buffer, uint32_t entero){
	buffer->stream = realloc(buffer->stream, buffer->size + sizeof(uint32_t));

	memcpy(buffer->stream + buffer->size, &entero, sizeof(uint32_t));
	buffer->size += sizeof(uint32_t);
}

uint32_t buffer_read_uint32(t_buffer *buffer){
	uint32_t entero;

	memcpy(&entero, buffer->stream + buffer->offset, sizeof(uint32_t));
	buffer->offset += sizeof(uint32_t);

	return entero;
}

// UINT8
void buffer_write_uint8(t_buffer* buffer, uint8_t entero){
	buffer->stream = realloc(buffer->stream, buffer->size + sizeof(uint8_t));

	memcpy(buffer->stream + buffer->size, &entero, sizeof(uint8_t));
	buffer->size += sizeof(uint8_t);
}

uint8_t buffer_read_uint8(t_buffer *buffer){
	uint8_t entero;

	memcpy(&entero, buffer->stream + buffer->offset, sizeof(uint8_t));
	buffer->offset += sizeof(uint8_t);

	return entero;
}

//STRING
void buffer_write_string(t_buffer* buffer, char* cadena){
	uint32_t tam = 0;

	while(cadena[tam] != NULL)
		tam++;

	buffer_write_uint32(buffer,tam);

	buffer->stream = realloc(buffer->stream, buffer->size + tam);

	memcpy(buffer->stream + buffer->size, cadena , tam);
	buffer->size += tam;
}


char* buffer_read_string(t_buffer *buffer){
	uint32_t tam = buffer_read_uint32(buffer);
	char* cadena = malloc(tam + 1);

	memcpy(cadena, buffer->stream + buffer->offset, tam);
	buffer->offset += tam;

	cadena[tam] = '\0';

	// free(aux);

	return cadena;
}

//INSTRUCTION
void buffer_write_Instruction(t_buffer* buffer, Instruction instruccion){
	buffer_write_uint8(buffer, instruccion.instruccion);
	buffer_write_uint32(buffer, instruccion.numero1);
	buffer_write_uint32(buffer, instruccion.numero2);
	buffer_write_string(buffer, instruccion.string1);
	buffer_write_string(buffer, instruccion.string2);
}

// Tiene que ser puntero para poder despues manejarlo en las listas
Instruction* buffer_read_Instruction(t_buffer* buffer){
	Instruction* inst = malloc(sizeof(Instruction));

	inst->instruccion = buffer_read_uint8(buffer);
	inst->numero1 = buffer_read_uint32(buffer);
	inst->numero2 = buffer_read_uint32(buffer);
	strcpy(inst->string1, buffer_read_string(buffer));
	strcpy(inst->string2, buffer_read_string(buffer));

	return inst;
}

// LISTA DE INSTRUCCIONES
void buffer_write_lista_instrucciones(t_buffer* buffer, t_list* lista_de_instrucciones){

	
	buffer -> codigo = LISTA_INSTRUCCIONES;
	// Hay que ver el orden en que escribimos y leemos
	// Primero guardo el tamanio de la lista, que lo necesito para cuando lea la lista
	uint32_t cant_instrucciones = list_size(lista_de_instrucciones);
	buffer_write_uint32(buffer, cant_instrucciones);

	for(int i = 0; i < cant_instrucciones; i++){
		Instruction* inst = list_get(lista_de_instrucciones,i);
		buffer_write_Instruction(buffer, *inst);
	}
}

t_list* buffer_read_lista_instrucciones(t_buffer* buffer){
	t_list* lista_instrucciones = list_create();

	uint32_t cant_instrucciones = buffer_read_uint32(buffer);

	for(int i = 0; i < cant_instrucciones; i++){
		Instruction* instr = buffer_read_Instruction(buffer);
		list_add(lista_instrucciones, instr);
	}

	return lista_instrucciones;
}


//REGISTROS
void buffer_write_Registros(t_buffer* buffer, Registros registros){
	buffer_write_string(buffer, registros.AX);
	buffer_write_string(buffer, registros.BX);
	buffer_write_string(buffer, registros.CX);
	buffer_write_string(buffer, registros.DX);

	buffer_write_string(buffer, registros.EAX);
	buffer_write_string(buffer, registros.EBX);
	buffer_write_string(buffer, registros.ECX);
	buffer_write_string(buffer, registros.EDX);

	buffer_write_string(buffer, registros.RAX);
	buffer_write_string(buffer, registros.RBX);
	buffer_write_string(buffer, registros.RCX);
	buffer_write_string(buffer, registros.RDX);
}

Registros buffer_read_Registros(t_buffer* buffer){
	Registros regis;

	strcpy(regis.AX, buffer_read_string(buffer));
	strcpy(regis.BX, buffer_read_string(buffer));
	strcpy(regis.CX, buffer_read_string(buffer));
	strcpy(regis.DX, buffer_read_string(buffer));

	strcpy(regis.EAX, buffer_read_string(buffer));
	strcpy(regis.EBX, buffer_read_string(buffer));
	strcpy(regis.ECX, buffer_read_string(buffer));
	strcpy(regis.EDX, buffer_read_string(buffer));

	strcpy(regis.RAX, buffer_read_string(buffer));
	strcpy(regis.RBX, buffer_read_string(buffer));
	strcpy(regis.RCX, buffer_read_string(buffer));
	strcpy(regis.RDX, buffer_read_string(buffer));

	return regis;
}

// SEGMENTOS
void buffer_write_segmento(t_buffer* buffer,Segmento segm){
	buffer_write_uint32(buffer, segm.id);
	buffer_write_uint32(buffer, segm.direccion_base);
	buffer_write_uint32(buffer, segm.tamanio_segmentos);
}


Segmento* buffer_read_segmento(t_buffer* buffer){
	Segmento* seg = malloc(sizeof(Segmento));

	seg->id = buffer_read_uint32(buffer);
	seg->direccion_base = buffer_read_uint32(buffer);
	seg->tamanio_segmentos = buffer_read_uint32(buffer);

	return seg;
}

// TABLA DE SEGMENTOS
void buffer_write_tabla_segmentos(t_buffer* buffer, t_list* tabla_segmentos){

	// Hay que ver el orden en que escribimos y leemos

	// Primero guardo la cantidad de nodos de la lista, que lo necesito para cuando lea la lista
	uint32_t cant_segmentos = list_size(tabla_segmentos);
	buffer_write_uint32(buffer, cant_segmentos);

	for(int i = 0; i < cant_segmentos; i++){
		Segmento* segm = list_get(tabla_segmentos, i);
		buffer_write_segmento(buffer, *segm);
	}
}

t_list* buffer_read_tabla_segmentos(t_buffer* buffer){
	t_list* tabla_segmentos = list_create();

	uint32_t cant_segmentos = buffer_read_uint32(buffer);

	for(int i = 0; i < cant_segmentos; i++){
		Segmento* segm = buffer_read_segmento(buffer);
		list_add(tabla_segmentos, segm);
	}

	return tabla_segmentos;
}




// CONTEXTO DE EJECUCION
void buffer_write_cde(t_buffer* buffer, contexto_de_ejecucion cde){
	buffer -> codigo = CONTEXTOEJEC;

	buffer_write_uint32(buffer, cde.pid);
	buffer_write_lista_instrucciones(buffer, cde.lista_de_instrucciones);
	buffer_write_uint32(buffer, cde.program_counter);
	buffer_write_Registros(buffer, cde.registrosCpu);
	buffer_write_tabla_segmentos(buffer, cde.tabla_segmentos);
}

contexto_de_ejecucion buffer_read_cde(t_buffer* buffer){
	contexto_de_ejecucion cde;

	cde.pid = buffer_read_uint32(buffer);
	cde.lista_de_instrucciones = buffer_read_lista_instrucciones(buffer);
	cde.program_counter = buffer_read_uint32(buffer);;
	cde.registrosCpu = buffer_read_Registros(buffer);
	cde.tabla_segmentos = buffer_read_tabla_segmentos(buffer);

	return cde;
}


// UTILS DE LECTURA

void mostrar_instrucciones(t_list* lista_instrucciones){
	
	printf("Instructions:\n");
    for (int i = 0; i < list_size(lista_instrucciones); i++)
    {
        Instruction* instruccion = list_get(lista_instrucciones, i);
		printf("Codigo de instruccion: %d ", instruccion->instruccion);
        
		if (instruccion->numero1 != 0)
            printf("Numero1: %d ", instruccion->numero1);
        if (instruccion->numero2 != 0)
            printf("Numero2: %d ", instruccion->numero2);
        if (strcmp(instruccion->string1, "") != 0)
            printf("String1: %s ", instruccion->string1);
        if (strcmp(instruccion->string2, "") != 0)
            printf("String2: %s ", instruccion->string2);
        printf("\n");
    }
}



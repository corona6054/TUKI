#include "serializacion.h"


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
	char* aux = malloc(tam);

	memcpy(aux, buffer->stream + buffer->offset, tam);
	buffer->offset += tam;

	char* cadena = malloc(tam);

	// esto no funciona
	for (int i = 0; i < tam; i++)
		cadena[i] = aux[i];

	free(aux);

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





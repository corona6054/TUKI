#include "../includes/consola.h"


int main(int argc, char** argv){
	sem_init(&sem_conexion, 0, 0);

	char* config_path=argv[1];
	char* instruccion_path=argv[2];
	levantar_modulo(config_path);
	crearLista(instruccion_path);
	log_info(logger,"Instrucciones leidas");

	sem_wait(&sem_conexion);
	enviarLista();

	finalizar_modulo();
	return 0;
}


// SUBPROGRAMAS

void serializeInstruction(Instruction* instruction, void* stream, int offset) {
	memcpy(stream + offset, &instruction->instruccion, sizeof(u_int32_t));
	offset += sizeof(u_int32_t);
	memcpy(stream + offset, &instruction->numero1, sizeof(u_int32_t));
	offset += sizeof(u_int32_t);
	memcpy(stream + offset, &instruction->numero2, sizeof(u_int32_t));
	offset += sizeof(u_int32_t);
	memcpy(stream + offset, &instruction->string1, sizeof(char[15]));
	offset += sizeof(char[15]);
	memcpy(stream + offset, &instruction->string2, sizeof(char[15]));
	offset += sizeof(char[15]);
}

int enviarLista(){
	t_paquete* paquete = crear_paquete();
    int instr_size = sizeof(u_int32_t) * 3 + sizeof(char[15]) * 2;
    void* serialized=malloc(instr_size);
    int offset=0;
	for (int i = 0; i < instructionCount; i++){
	            serializeInstruction(&instructions[i], serialized,offset);
				agregar_a_paquete(paquete,serialized,instr_size);
	        }
		enviar_paquete(paquete,socket_kernel);
		log_info(logger,"Paquete enviado");
		eliminar_paquete(paquete);
        free(serialized);
        return 1;
}


void levantar_modulo(char* config_path){
	logger = iniciar_logger();
	config = iniciar_config(config_path);
	levantar_config();
	establecer_conexiones();
}

void finalizar_modulo(){
	log_destroy(logger);
	config_destroy(config);
	close(socket_kernel);
	return;
}

t_log* iniciar_logger(void)
{
	t_log* nuevo_logger;
	nuevo_logger = log_create("consola.log", "Consola", true, LOG_LEVEL_INFO);

	if (nuevo_logger == NULL)
	{
		printf("Error al crear el logger\n");
		exit(1);
	}

	return nuevo_logger;
}

t_config* iniciar_config(char* config_path)
{
	t_config* nuevo_config;

	nuevo_config = config_create(config_path);

	if (nuevo_config == NULL){
		printf("Error al crear el nuevo config\n");
		exit(2);
	}

	return nuevo_config;
}

void levantar_config(){
	config_consola.ip_kernel = config_get_string_value(config,"IP_KERNEL");
	config_consola.puerto_kernel = config_get_int_value(config,"PUERTO_KERNEL");
}

void establecer_conexiones()
{
	pthread_t conexion_kernel;
	pthread_create(&conexion_kernel, NULL, (void *) conectarse_con_kernel, NULL);
	pthread_detach(conexion_kernel);
}

void conectarse_con_kernel(){
	socket_kernel = crear_conexion(config_consola.ip_kernel, config_consola.puerto_kernel);
	log_info(logger,"Socket kernel :%d",socket_kernel);
	if (socket_kernel >= 0){
		sem_post(&sem_conexion);
		log_info(logger,"Conectado con kernel");
	}
	else{
		log_info(logger,"Error al conectar con kernel");
	}
}

InstructionType getNextInstruction(FILE *file)
{
    char instruction[20];
    fscanf(file, "%s", instruction);

    if (strcmp(instruction, "SET") == 0)
    {
        return SET;
    }
    else if (strcmp(instruction, "MOV_OUT") == 0)
    {
        return MOV_OUT;
    }
    else if (strcmp(instruction, "WAIT") == 0)
    {
        return WAIT;
    }
    else if (strcmp(instruction, "I/O") == 0)
    {
        return IO;
    }
    else if (strcmp(instruction, "SIGNAL") == 0)
    {
        return SIGNAL;
    }
    else if (strcmp(instruction, "MOV_IN") == 0)
    {
        return MOV_IN;
    }
    else if (strcmp(instruction, "F_OPEN") == 0)
    {
        return F_OPEN;
    }
    else if (strcmp(instruction, "YIELD") == 0)
    {
        return YIELD;
    }
    else if (strcmp(instruction, "F_TRUNCATE") == 0)
    {
        return F_TRUNCATE;
    }
    else if (strcmp(instruction, "F_SEEK") == 0)
    {
        return F_SEEK;
    }
    else if (strcmp(instruction, "CREATE_SEGMENT") == 0)
    {
        return CREATE_SEGMENT;
    }
    else if (strcmp(instruction, "F_WRITE") == 0)
    {
        return F_WRITE;
    }
    else if (strcmp(instruction, "F_READ") == 0)
    {
        return F_READ;
    }
    else if (strcmp(instruction, "DELETE_SEGMENT") == 0)
    {
        return DELETE_SEGMENT;
    }
    else if (strcmp(instruction, "F_CLOSE") == 0)
    {
        return F_CLOSE;
    }
    else if (strcmp(instruction, "EXIT") == 0)
    {
        return EXIT;
    }
    else
    {
        return INVALID;
    }
}
void readIntegerFromFile(FILE *file, int index)
{
    int number;
    fscanf(file, "%d", &number);
    if (index == 1)
        instructions[instructionCount].numero1 = number;
    else
        instructions[instructionCount].numero2 = number;
}
void readNextWordFromFile(FILE *file, int index)
{
    char buffer[15];
    fscanf(file, "%s", buffer);
    if (index == 1)
        strcpy(instructions[instructionCount].string1, buffer);
    else
        strcpy(instructions[instructionCount].string2, buffer);
}

void parseInstructions(FILE *file)
{
    strtok(file, " ");
    int instruccion_actual;
    while (1)
    {
        instruccion_actual = getNextInstruction(file);
        instructions[instructionCount].instruccion = instruccion_actual;
        switch (instruccion_actual)
        {
        case F_READ:
            readNextWordFromFile(file, 1);
            readIntegerFromFile(file, 1);
            readIntegerFromFile(file, 2);
            break;
        case F_WRITE:
            readNextWordFromFile(file, 1);
            readIntegerFromFile(file, 1);
            readIntegerFromFile(file, 2);
            break;
        case SET:
            readNextWordFromFile(file, 1);
            readNextWordFromFile(file, 2);
            break;
        case MOV_IN:
            readNextWordFromFile(file, 1);
            readIntegerFromFile(file, 1);
            break;
        case MOV_OUT:
            readIntegerFromFile(file, 1);
            readNextWordFromFile(file, 1);
            break;
        case F_TRUNCATE:
            readNextWordFromFile(file, 1);
            readIntegerFromFile(file, 1);
            break;
        case F_SEEK:
            readNextWordFromFile(file, 1);
            readIntegerFromFile(file, 1);

            break;
        case CREATE_SEGMENT:
            readIntegerFromFile(file, 1);
            readIntegerFromFile(file, 2);
            break;
        case IO:
            readIntegerFromFile(file, 1);

            break;
        case WAIT:
            readNextWordFromFile(file, 1);
            break;
        case SIGNAL:
            readNextWordFromFile(file, 1);

            break;
        case F_OPEN:
            readNextWordFromFile(file, 1);
            break;
        case F_CLOSE:
            readNextWordFromFile(file, 1);
            break;
        case DELETE_SEGMENT:
            readIntegerFromFile(file, 1);
            break;
        case YIELD:

            break;
        case EXIT:
            instructionCount++;
            return;

            break;

        default:
            break;
        }
        instructionCount++;
    }
}

void match(FILE *file, const char *expected)
{
    char input[MAX_PARAMETER_LENGTH];
    fscanf(file, "%s", input);
    if (strcmp(input, expected) != 0)
    {
        error();
    }
}

void error()
{
    printf("Syntax error\n");
    exit(1);
}

int crearLista(char* filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Failed to open file.\n");
        return 1;
    }
    parseInstructions(file);
    fclose(file);
    /*
    printf("Instructions:\n");
    for (int i = 0; i < instructionCount; i++)
    {
        printf("%d ", instructions[i].instruccion);
        if (instructions[i].numero1 != 0)
            printf("%d ", instructions[i].numero1);
        if (instructions[i].numero2 != 0)
            printf("%d ", instructions[i].numero2);
        if (strcmp(instructions[i].string1, "") != 0)
            printf("%s ", instructions[i].string1);
        if (strcmp(instructions[i].string2, "") != 0)
            printf("%s ", instructions[i].string2);
        printf("\n");
    }
	*/
    return 0;
}
#include "../includes/file_system.h"

typedef struct {
	uint32_t  block_size;
	uint32_t block_count;
}superBloque;

typedef struct {
	char*  file_name;
	uint32_t file_size;
	uint32_t  direct_pointer;
	uint32_t indirect_pointer;
	}FCB;

	int bitarray_fd;
	char *bitarray;
	off_t file_size;

	int crearEstructuras();
	int cerrarEstructuras();

int main(void){

	levantar_modulo();
	crearEstructuras();

	cerrarEstructuras();

	finalizar_modulo();
	return 0;
}

// SUBPROGRAMAS

int crearEstructuras(){
	t_config* superbloque_config;
	superBloque superbloque;
	superbloque_config = config_create(config_file_system.path_superbloque);
	superbloque.block_size= (int)config_get_int_value(config,"BLOCK_SIZE");
	superbloque.block_count= (int)config_get_int_value(config,"BLOCK_COUNT");
	bitarray_fd = open(config_file_system.path_bitmap, O_RDWR);
	if (bitarray_fd) printf("Error opening bitmap");
    file_size = lseek(bitarray_fd, 0, SEEK_END);
    bitarray = mmap(NULL, file_size, PROT_READ | PROT_WRITE, MAP_SHARED, bitarray_fd, 0);
    if (bitarray == MAP_FAILED) printf("Error mapeando");
    return 0;
}
int cerrarEstructuras(){
	int sync = msync(bitarray, file_size, MS_SYNC);
	    if(sync == -1) printf("Error syncing the file");
	    int unmap = munmap(bitarray, file_size);
	    if(unmap == -1) printf("Error syncing the file");
	    close(bitarray_fd);
	    return 0;
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
	nuevo_logger = log_create("fileSystem.log", "FileSystem", 1, LOG_LEVEL_INFO);

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

	nuevo_config = config_create("fileSystem.config");

	if (nuevo_config == NULL){
		printf("Error al crear el nuevo config\n");
		exit(2);
	}

	return nuevo_config;
}

void levantar_config(){
	config_file_system.ip_memoria = config_get_string_value(config,"IP_MEMORIA");
	config_file_system.puerto_memoria = config_get_int_value(config,"PUERTO_MEMORIA");
	config_file_system.puerto_escucha = config_get_int_value(config,"PUERTO_ESCUCHA");
	config_file_system.path_superbloque = config_get_string_value(config,"PATH_SUPERBLOQUE");
	config_file_system.path_bitmap = config_get_string_value(config,"PATH_BITMAP");
	config_file_system.path_bloques = config_get_string_value(config,"PATH_BLOQUES");
	config_file_system.path_fcb = config_get_string_value(config,"PATH_FCB");
	config_file_system.retardo_acceso_bloque = config_get_int_value(config,"RETARDO_ACCESO_BLOQUE");
}

void conectarse_con_memoria(){
	socket_memoria = crear_conexion(config_file_system.ip_memoria, config_file_system.puerto_memoria);
	log_info(logger,"Conectado con memoria");
}

void establecer_conexiones()
{
	pthread_t conexion_memoria;
	pthread_create(&conexion_memoria, NULL, (void *) conectarse_con_memoria, NULL);
	pthread_detach(conexion_memoria);

	server_fd = abrir_servidor(logger,config);
	kernel_fd = esperar_cliente(server_fd, logger);

}

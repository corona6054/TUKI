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
	int archivobloques_fd;
	char *archivobloques_pointer;
	char *bitarray_pointer;
	off_t bitarray_size;
	off_t archivobloques_size;
	t_bitarray * bitarray;
	t_list* fcb_list;
	char buscado[64];
	superBloque superbloque;


	int crearEstructuras();
	int cerrarEstructuras();
	int crearArchivo(char* nombre);
	int abrirArchivo(char* nombre);
	bool igualBuscado(void * ptr);

int main(void){

	levantar_modulo();
	crearEstructuras();
	crearArchivo("hola\n");
	abrirArchivo("hola\n");
	cerrarEstructuras();

	finalizar_modulo();
	return 0;
}

// SUBPROGRAMAS

int truncarArchivo(char* nombre, int size){
	strcpy(buscado,nombre);
	FCB* seleccionado = (FCB*)list_remove_by_condition(fcb_list,igualBuscado);
	int cant_bloques = (size +63)/superbloque.block_size;
	seleccionado->file_size= cant_bloques * superbloque.block_size;
	int bloq_actual=0;
	/*
	for(int i = 0;i<bitarray_size;i++){
		if(bitarray_test_bit(bitarray_pointer,i)==1){

		}
		}
		*/

}


bool igualBuscado(void * ptr){
	FCB* seleccionado = (FCB*) ptr;
	if (strcmp(seleccionado->file_name,buscado)==0){
		return 1;
	} else return 0;

}
int abrirArchivo(char* nombre){
	strcpy(buscado,nombre);
	bool resultado = list_any_satisfy(fcb_list,igualBuscado);
	if(resultado==1){
    log_info(logger,"Abrir Archivo: %s", nombre);}
	else{
	    log_info(logger,"Archivo no existe");
	}
	return resultado;

}
int crearArchivo(char* nombre){
	FCB *fcb_nuevo;
	fcb_nuevo=malloc(sizeof(FCB));
	fcb_nuevo->file_name= malloc(sizeof(nombre));
    char file_path[256];
    strcpy(fcb_nuevo->file_name, nombre);
	fcb_nuevo->file_size=0;
    snprintf(file_path, sizeof(file_path), "%s/%s.dat", config_file_system.path_fcb, nombre);
	FILE * fcb_file = fopen(file_path,"w");
	 if (!fcb_file) {
	        printf("Error creating the file.\n");
	        return-1;
	    }
    fprintf(fcb_file, "NOMBRE_ARCHIVO=%s\n", fcb_nuevo->file_name);
       fprintf(fcb_file, "TAMANIO_ARCHIVO=%u\n", fcb_nuevo->file_size);
       fprintf(fcb_file, "PUNTERO_DIRECTO=%u\n", fcb_nuevo->direct_pointer);
       fprintf(fcb_file, "PUNTERO_INDIRECTO=%u\n", fcb_nuevo->indirect_pointer);
       list_add(fcb_list,fcb_nuevo);
       log_info(logger,"Crear archivo: %s", nombre);
	return 0;

}
int crearEstructuras(){
	//superbloque
	t_config* superbloque_config;
	superbloque_config = config_create(config_file_system.path_superbloque);
	superbloque.block_size= (uint32_t)config_get_int_value(superbloque_config,"BLOCK_SIZE");
	superbloque.block_count= (uint32_t)config_get_int_value(superbloque_config,"BLOCK_COUNT");
	//bitarray
	bitarray_fd = open(config_file_system.path_bitmap, O_RDWR);
	if (bitarray_fd==-1) {
		FILE * bitarray_file = fopen(config_file_system.path_bitmap,"w");
	    size_t num_bytes = superbloque.block_count / 8;
	    unsigned char* buffer = (unsigned char*)malloc(num_bytes);
	    for (size_t i = 0; i < num_bytes; i++) {
	           buffer[i] = 0xFF;
	       }
	    fwrite(buffer, sizeof(unsigned char), num_bytes, bitarray_file);
	    free(buffer);
	    fclose(bitarray_file);
	    bitarray_fd = open(config_file_system.path_bitmap, O_RDWR);
	}
    bitarray_size = lseek(bitarray_fd, 0, SEEK_END);
    bitarray_pointer = mmap(NULL, bitarray_size, PROT_READ | PROT_WRITE, MAP_SHARED, bitarray_fd, 0);
    if (bitarray_pointer == MAP_FAILED) printf("Error mapeando bitarray");
    bitarray =bitarray_create(bitarray_pointer,bitarray_size);

	//fcb
    char file_path[256];
    t_config* config_fcb;
    FCB *read_fcb;
    fcb_list = list_create();
    DIR* directory = opendir(config_file_system.path_fcb);
    struct dirent* entry;
    while ((entry = readdir(directory)) != NULL) {
            if (entry->d_type == DT_REG) { // Check if it's a regular file
                read_fcb=(FCB*)malloc(sizeof(FCB));
                read_fcb->file_name = malloc(sizeof(config_get_string_value(config_fcb,"NOMBRE_ARCHIVO")));
            	snprintf(file_path, sizeof(file_path), "%s/%s", config_file_system.path_fcb, entry->d_name);
                config_fcb= config_create(file_path);
                read_fcb->file_name = config_get_string_value(config_fcb,"NOMBRE_ARCHIVO");
                read_fcb->file_size = (uint32_t)config_get_int_value(config_fcb,"TAMANIO_ARCHIVO");
                read_fcb->direct_pointer = (uint32_t)config_get_int_value(config_fcb,"PUNTERO_DIRECTO");
                read_fcb->indirect_pointer = (uint32_t)config_get_int_value(config_fcb,"PUNTERO_INDIRECTO");
                list_add(fcb_list,read_fcb);
            }
    }
    closedir(directory);

    //archivo de bloques

    archivobloques_fd = open(config_file_system.path_bloques, O_RDWR);
    	if (archivobloques_fd==-1) {
    		FILE * archivobloques_file = fopen(config_file_system.path_bloques,"w");
    	    size_t total_size = superbloque.block_count * superbloque.block_size;
    	    unsigned char* buffer = (unsigned char*)malloc(total_size);
    	    fwrite(buffer, sizeof(unsigned char), total_size, archivobloques_file);
    	    free(buffer);
    	    fclose(archivobloques_file);
    	    archivobloques_fd = open(config_file_system.path_bloques, O_RDWR);
    	}
    	archivobloques_size = lseek(archivobloques_fd, 0, SEEK_END);
    	archivobloques_pointer = mmap(NULL, archivobloques_size, PROT_READ | PROT_WRITE, MAP_SHARED, bitarray_fd, 0);
        if (archivobloques_pointer == MAP_FAILED) printf("Error mapeando archivobloques");
    	log_info(logger,"Estructuras creadas");


    return 0;
}

int cerrarEstructuras(){
	int sync = msync(bitarray_pointer, bitarray_size, MS_SYNC);
	    if(sync == -1) printf("Error syncing bitarray");
	    int unmap = munmap(bitarray_pointer, bitarray_size);
	    if(unmap == -1) printf("Error unmapping bitarray");
	    close(bitarray_fd);

	    sync = msync(archivobloques_pointer, archivobloques_size, MS_SYNC);
	    	    if(sync == -1) printf("Error syncing archivobloques");
	    	     unmap = munmap(archivobloques_pointer, archivobloques_size);
	    	    if(unmap == -1) printf("Error unmapping archivobloques");
	    	    close(archivobloques_fd);
	    log_info(logger,"Estructuras terminadas");

	    return 0;
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

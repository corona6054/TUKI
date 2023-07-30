#ifndef INCLUDES_KERNEL_H_
#define INCLUDES_KERNEL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <commons/config.h>
#include <commons/memory.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
//#include "serializacion.h"
#include "comunicacion.h"

typedef struct{
    int socket;
    int socket_anterior;
} t_conexiones;

typedef struct{
	char* ip_memoria;
	int puerto_memoria;
	char* ip_cpu;
	int puerto_cpu;
	char* ip_file_system;
	int puerto_file_system;
	int puerto_escucha;
	char* algoritmo;
	int est_inicial;
	int alpha_hrrn;
	int grado_max_multi;
	t_list* recursos_compartidos;
	t_list* instancias;
}Config_kernel;

// Variables globales
Config_kernel config_kernel;
t_log* logger;
t_config* config;


int socket_memoria;
int socket_file_system;
int socket_cpu;

int server_fd;

int consola_fd;


// SEMAFOROS
// Mutex
pthread_mutex_t mutex_new;
pthread_mutex_t mutex_ready;
pthread_mutex_t mutex_blocked;
pthread_mutex_t mutex_exec;
pthread_mutex_t mutex_exit;

// Contadores
sem_t* cont_exit;
sem_t* conexion_consola;


int pid = 0;

int ejecutando = 0;

t_queue* procesosNew;
t_queue* procesosReady;
t_queue* procesosBlocked;
t_queue* procesosExec;
t_queue* procesosExit;

// Prototipos funciones
void levantar_modulo();
void finalizar_modulo();

t_log* iniciar_logger(void);
t_config* iniciar_config(void);
void levantar_config();

void conectarse_con_memoria();
void conectarse_con_cpu();
void conectarse_con_file_system();

void establecer_conexiones();

void manejar_clientes(int);
t_list* recibir_paquete(int);

void planificadorLargoPlazo();
void planificadorCortoPlazo();
t_registros inicializar_registros();
t_pcb* crear_pcb(t_list*, int);

void planificacionFIFO();
void planificacionHRRN();

void serializar_cde(contexto_de_ejecucion);
int tamanio_cde_serializado(Cde_serializado);

void deserializar_cde();

void actualizar_instancias_recurso(char*, int);
int obtener_instancia_recurso(char*);

void enviarDeReadyAExecFIFO();
void enviarDeBlockAReadyFIFO();
void mandar_a_ready();
void terminar_procesos();
void recepcionar_proceso(int);

t_pcb* crear_pcb(t_list* lista_instrucciones, int socket);
t_cde* crear_cde(t_list* lista_instrucciones);

//Agregar y retirar pcbs de las funciones seguramente
t_pcb* retirar_pcb_de(t_queue* cola, pthread_mutex_t* mutex);
void agregar_pcb_a(t_queue* cola, t_pcb* pcb_a_agregar, pthread_mutex_t* mutex);

#endif /* INCLUDES_KERNEL_H_ */

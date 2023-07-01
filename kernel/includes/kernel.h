#ifndef INCLUDES_KERNEL_H_
#define INCLUDES_KERNEL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/config.h>
#include <commons/memory.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include "utils_server.h"
#include "utils_cliente.h"
#include "serializacion.h"

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

sem_t* prueba1;
sem_t* m_nuevos;
sem_t* m_ready;
sem_t* m_bloqueados;
sem_t* m_finalizados;
sem_t* m_exec;

int pid = 0;

int ejecutando = 0;

t_list* procesosNuevos;
t_list*	procesosReady;
t_list*	procesosBloqueados;
t_list*	procesosFinalizados;
t_list* procesosEjecutando;

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

void planificadorLargoPlazo(t_conexiones*);
void planificadorCortoPlazo();
Registros inicializar_registros();
pcb crear_pcb(t_list*);

void planificacionFIFO();
void planificacionHRRN();

void serializar_cde(contexto_de_ejecucion);
int tamanio_cde_serializado(Cde_serializado);

void deserializar_cde();

int actualizar_instancias_recurso(char*, int);
int obtener_instancia_recurso(char*);

#endif /* INCLUDES_KERNEL_H_ */

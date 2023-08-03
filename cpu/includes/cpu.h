#ifndef INCLUDES_CPU_H_
#define INCLUDES_CPU_H_

#include <stdio.h>
#include <stdlib.h>
#include "commons/config.h"
#include "commons/log.h"
#include <pthread.h>
#include <stdint.h>
#include <semaphore.h>
#include <math.h>
#include "serializacion.h"
#include "comunicacion.h"


typedef struct{
	int retardo;
	char *ip_memoria;
	int puerto_memoria;
	int puerto_escucha;
	int tam_max_segmento;
}Config_cpu;


// Variables globales
// Semaforos
sem_t leer_siguiente_instruccion; // Se va usar como un pseudo-semaforo (medio bool)

sem_t necesito_enviar_cde;

//Binarios
sem_t bin1_envio_cde;
sem_t bin2_recibir_cde;


Config_cpu config_cpu;
t_log* logger;
t_config* config;


int kernel_fd;
int socket_memoria;

int server_fd;

t_cde* cde_en_ejecucion;

// UTILS INICIAR MODULO -----------------------------------------------------------------
void levantar_modulo();
void finalizar_modulo();
void inicializar_semaforos();
void iniciar_modulo();
t_log* iniciar_logger(void);
t_config* iniciar_config(void);
void levantar_config();
// FIN UTILS INICIAR MODULO -------------------------------------------------------------

// UTILS CONEXIONES ---------------------------------------------------------------------
void conectarse_con_memoria();
void establecer_conexiones();
void crear_conexion_con_kernel();
void recibir_cde();
void enviar_cde();
// UTILS FIN CONEXIONES -----------------------------------------------------------------

// UTILS INSTRUCCIONES (GENERAL) --------------------------------------------------------
void ejecutar_proceso();
void evaluar_instruccion(t_instruction* instruccion);
// FIN UTILS INSTRUCCIONES (GENERAL) ----------------------------------------------------

// UTILS INSTRUCCIONES (PARTICULAR) -----------------------------------------------------
void ejecutar_set(char*, char*, t_registros);
void ejecutar_move_in(char*, uint32_t); 
void ejecutar_move_out(char*, uint32_t);

int calcular_dir_fisica(int dir_logica,int tamanio);
int tamanioRegistro(char *registro);
void sacar_de_registro(char registro[],int dir_fisica, int tamanio, t_registros *registros, int pid);
// FIN UTILS INSTRUCCIONES (PARTICULAR) -------------------------------------------------



#endif /* INCLUDES_CPU_H_ */

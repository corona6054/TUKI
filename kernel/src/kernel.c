#include "../includes/kernel.h"

int main(void){
	logger = log_create("/home/utnso/Documents/tp-2023-1c-aLaGrandeLePuseCuca/kernel/kernel.log", "Kernel", true, LOG_LEVEL_INFO);

	int server_fd = iniciar_servidor();
	log_info(logger, "Servidor listo para recibir al cliente");
	int cliente_fd = esperar_cliente(server_fd);
	log_info(logger, "Servidor escuchando al cliente");

	return 0;
}

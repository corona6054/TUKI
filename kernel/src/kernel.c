#include "../includes/kernel.h"

int main(void){
	t_log* logger = log_create("kernel.log", "Kernel Servidor", 1, LOG_LEVEL_INFO);

	int server_fd = iniciar_servidor();
	log_info(logger, "Servidor listo para recibir al cliente");
	int cliente_fd = esperar_cliente(server_fd);

	//saludar("Carlos");
	return 0;
}

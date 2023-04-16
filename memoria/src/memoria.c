#include "../includes/memoria.h"

int main(void){
	t_log* logger;
	logger = log_create("../memoria.log", "Memoria", 1, LOG_LEVEL_INFO);

	int server_fd = iniciar_servidor();
	log_info(logger, "Servidor listo para recibir al cliente");
	int cliente_fd = esperar_cliente(server_fd);
	log_info(logger, "Servidor escuchando al cliente");

	return 0;
}

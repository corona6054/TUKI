#include "../includes/kernel.h"

int main(void){
	logger = log_create("kernel.log", "Kernel Servidor", 1, LOG_LEVEL_DEBUG);

	return 0;
}

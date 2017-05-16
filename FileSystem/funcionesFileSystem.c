/*
 * funcionesFileSystem.c
 *
 *  Created on: 16/5/2017
 *      Author: utnso
 */

#include "funcionesFileSystem.h"

int pack[2];
char package[PACKAGESIZE];
t_package pkg;

void* atenderKernel(void* _parameters) {
	int status = 1;
	structParaKernel* kernel = (structParaKernel*) _parameters;
	int serverSocket = kernel->socket;
	t_log* logger = kernel->log;

	printf("Esperando mensaje del Kernel.\n");
	if(recibir(serverSocket, &pkg, logger)){
		//ERROR
		close(serverSocket);
		return EXIT_FAILURE;
	}
	printf("Mensaje recibido del kernel: %s\n", pkg.data);
	free(pkg.data);

	return NULL;
}

void liberar_memoria(t_log* logger,configFileSystem* config) {
	free(logger);
	free(config);
}

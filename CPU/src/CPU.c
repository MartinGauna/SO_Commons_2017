/*
 ============================================================================
 Name        : CPU.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <commons/log.h>
#include <commons/string.h>
#include <features.h>
#include <netinet/in.h>
#include "sockets.h"
#include "configCPU.h"

#define PACKAGESIZE 1024

int main(void) {

    t_configCPU* cpu = leerConfigCPU();
    char *IP = cpu->IP_KERNEL;
    char* PUERTO = string_itoa(cpu->PUERTO_KERNEL);
    printf("Anda? Entonces debe devolver 127.0.0.1. Retorna : %s \n", IP);

	int serverSocket;

	create_client(&serverSocket , IP, PUERTO);

	printf("Conectado al Kernel. \n");

	char package[PACKAGESIZE];
	int status = 1;

	while (status != 0){
		status = recv(serverSocket, (void*) package, PACKAGESIZE, 0);
		if (status != 0) printf("%s", package);
	}

	close(serverSocket);

	return 0;
}

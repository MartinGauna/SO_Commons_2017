/*
 ============================================================================
 Name        : Memoria.c
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
#include <features.h>
#include <netinet/in.h>
#include "sockets.h"
#include "configMemoria.h"

#define PUERTO "5100"
#define BACKLOG 5
#define PACKAGESIZE 1024

int main(){

	int listenningSocket;
	int serverSocket;

	create_server(&listenningSocket, PUERTO);

	accept_connection(listenningSocket, &serverSocket);

	printf("Servidor creado\n");

    t_configMemoria* memoria = leerConfigMemoria(memoria);
    printf("Anda? Entonces debe devolver 5003. Retorna : %d \n \n", memoria->PUERTO2);

	char package[PACKAGESIZE];
	int status = 1;

	handShakeServidor(serverSocket,"memoria","kernel");
	printf("Kernel conectado. Esperando mensajes:\n");

	while (status != 0){
		status = recv(serverSocket, (void*) package, PACKAGESIZE, 0);
		if (status != 0) printf("%s", package);
	}

	close(serverSocket);
	close(listenningSocket);

	return 0;
}

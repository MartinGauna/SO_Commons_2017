/*
 ============================================================================
 Name        : FileSystem.c
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
#include "configFileSystem.h"

#define PUERTO "5004"
#define BACKLOG 5
#define PACKAGESIZE 1024



int main(){

	int listenningSocket;
	int serverSocket;

	create_server(&listenningSocket, PUERTO);

	accept_connection(listenningSocket, &serverSocket);

	printf("Servidor creado\n");

    t_configFileSystem* fileSystem = leerConfigFS(fileSystem);
    printf("Anda? Entonces debe devolver 5003. Retorna : %d \n \n", fileSystem->PUERTO2);

	char package[PACKAGESIZE];
	int status = 1;
	char handCliente[10];

	handShakeServidor(serverSocket,"fileSys",handCliente);
	printf("%s conectado. Esperando mensajes:\n",handCliente);


	while (status != 0){
		status = recv(serverSocket, (void*) package, PACKAGESIZE, 0);
		if (status != 0) printf("%s", package);
	}

	close(serverSocket);
	close(listenningSocket);

	return 0;
 }

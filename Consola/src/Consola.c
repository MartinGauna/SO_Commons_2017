/*
 ============================================================================
 Name        : Consola.c
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
#include "configConsola.h"

#define PACKAGESIZE 1024

int main(void) {

    t_configConsola* consola = leerConfigConsola();
    char *IP = consola->IP_KERNEL;
    char* PUERTO = string_itoa(consola->PUERTO_KERNEL);
    printf("Anda? Entonces debe devolver 127.0.0.1. Retorna : %s \n", IP);

	int serverSocket;

	create_client(&serverSocket , IP, PUERTO);

	int enviar = 1;
	char message[PACKAGESIZE];
	char handServer[10];
	printf("Conectado al Kernel. Escriba 'exit' para salir\n");


//	handShakeCliente(serverSocket,handServer,"consola");
//	printf("Me comunique con %s \n",handServer);

	while(enviar){
		fgets(message, PACKAGESIZE, stdin);
		if (!strcmp(message,"exit\n")) enviar = 0;
		if (enviar) send(serverSocket, message, strlen(message) + 1, 0);
	}

	close(serverSocket);

	return 0;
}

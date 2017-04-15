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
#include <features.h>
#include <netinet/in.h>
#include "sockets.h"
#include "configConsola.h"

#define IP "127.0.0.1"
#define PUERTO "5010"
#define PACKAGESIZE 1024

int main(void) {

    t_configConsola* consola = leerConfig(consola);
    printf("Anda? Entonces debe devolver 5010. Retorna : %d \n \n", consola->PUERTO_KERNEL);

	int serverSocket;

	create_client(&serverSocket , IP, PUERTO);

	int enviar = 1;
	char message[PACKAGESIZE];

	printf("Conectado al Kernel. Escriba 'exit' para salir\n");

	while(enviar){
		fgets(message, PACKAGESIZE, stdin);
		if (!strcmp(message,"exit\n")) enviar = 0;
		if (enviar) send(serverSocket, message, strlen(message) + 1, 0);
	}

	close(serverSocket);

	return 0;
}

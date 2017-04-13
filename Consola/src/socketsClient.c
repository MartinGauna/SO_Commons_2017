/*
 * socketsClient.c
 *
 *  Created on: 11/4/2017
 *      Author: utnso
 */
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include "socketsClient.h"

t_cliente newClient(char* ip, int puerto){

	t_cliente* newCliente = malloc(sizeof(t_cliente));
	newSocket(&((*newCliente).socketCliente));

    if(((*newCliente).destino.sin_addr.s_addr = inet_addr(ip)) == INADDR_NONE) {
    	printf("Error al encontrar IP. Verifique que la IP sea la correcta");
    	exit(1);
    }
	memset(&((*newCliente).destino.sin_zero), '\0', 8);
	(*newCliente).destino.sin_family = AF_INET;
	(*newCliente).destino.sin_port = htons(puerto);

    return *newCliente;

}

void newSocket(int* socketNuevo) {

	if((*socketNuevo = socket(AF_INET,SOCK_STREAM,0)) == -1) {
		perror("No se pudo crear el socket, el programa finalizará.");
		exit(1);
	}
}

int connectServer(t_cliente unCliente) {

	if((connect(&(unCliente.socketCliente), &(unCliente.destino), sizeof(struct sockaddr))) == -1) {
		perror("Error de coneccion.");
		return -1;
	}

	return 0;
}

int sendMessageToServer(int socketDestino, char* message) {

	if((send(socketDestino, message, strlen(message), 0)) == -1) {
		perror("Error al enviar el mensaje.");
		close(socketDestino);
		exit(0);
		return -1;
	}

	return 0;
}

char* waitServerResponse(int socketServer) {

	int messageReceived;
	char longitud[256];
	char* buffer = malloc(sizeof(longitud));
	messageReceived = recv(socketServer, buffer, sizeof(longitud)-1, 0);

	if(messageReceived == 0) {
		free(buffer);
		return "El server ha cerrado tu conexion contigo";
		//tiro error(0) y cierro el socketServer?
	}
	else {
		if(messageReceived == -1) {
			free(buffer);
			return "Error al recibir el mensaje.";
			//exit(1);
		}
		else {
			return buffer;
		}
	}
}

char* waitResponseServerTam(int socketServer, int* longMsg) {

	int messageReceived;
	char longitud[256];
	char* buffer = (char*)malloc(sizeof(*longMsg + 1));
	messageReceived = recv(socketServer, buffer, sizeof(longitud)-1, 0);

	if(messageReceived == 0) {
		buffer = (char*)realloc(buffer, strlen("El server ha cerrado conexion contigo."));
		strcpy(buffer, "El server ha cerrado conexion contigo.");
	}
	else {
		if(messageReceived == -1) {
			buffer = (char*)realloc(buffer, strlen("Error al recibir el mensaje."));
			strcpy(buffer, "Error al recibir el mensaje.");
		}
		else {
			buffer = (char*)realloc(buffer, messageReceived);
			*longMsg = messageReceived;
		}
	}
	return buffer;
}

char* chattingWithProcess(int socketProcess, char* message) {

	sendMessageToServer(socketProcess, message);
	return waitServerResponse(socketProcess);
}

/*
char* handShakeClient(int socketServer, char* message) {

	char* msgReturn = malloc(sizeof(message));
	int longMsg = strlen(message);
	msgReturn = waitResponseServerTam(socketServer, &longMsg);

	if((strcmp(msgReturn, "El server ha cerrado conexion contigo.") ||
	    strcmp(msgReturn, "Error al recibir el mensaje.")) == 0) {
		perror(msgReturn);
		close(socketServer);
		return msgReturn;
	}

	if(sendMessageToServer(socketServer, message) == -1) {
		msgReturn = (char*)realloc(msgReturn, strlen("Error al enviar el mensaje."));
		strcpy(msgReturn, "Error al enviar el mensaje.");
		perror(msgReturn);
		close(socketServer);
		return msgReturn;
	}

	return msgReturn;

}
*/
/*
char* handShakeClient(int *socket, char * mensaje)
{
	char buffer[30];
		//Enviar mensaje
	write(* socket, mensaje, 30);
	fprintf(stderr, "%s \n",mensaje);
		//Recibir respuesta
	read(* socket, buffer, 30);
	fprintf(stderr, "buffer %s \n",buffer);

	mensaje = buffer;
	return mensaje;
}

int inicializarSocketCliente(int *socket)
{
	char buffer[30];

	fprintf(stderr, "iniciando consola_cliente... \n");

	while((write((* socket), "intento de envio", 30)) == -1)
	{
		(* socket) = Abrir_cliente("127.0.0.1"); //hardcodeame ésta. Estaria bueno poner un numero de intentos
		fprintf(stderr, "CONSOLA: Error al intentar conectar con el Kernel, se realizara un "
				"nuevo intento en 3 segundos. \n");
		sleep(3);
	}
	fprintf(stderr, "iniciado correctamente \n");

	read(* socket, buffer, 30); //no entendi que hace
	fprintf(stderr, "Leo: %s\n", buffer);

	return 0;
}
*/



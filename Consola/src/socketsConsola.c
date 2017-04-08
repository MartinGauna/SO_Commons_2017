/*
 * socketsConsola.c
 *
 *  Created on: 7/4/2017
 *      Author: utnso
 */
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

t_cliente newClient(char* ip, int puerto){

	t_cliente* newCliente = malloc(sizeof(struct cliente));
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










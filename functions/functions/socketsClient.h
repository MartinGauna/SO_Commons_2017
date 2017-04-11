/*
 * socketsClient.h
 *
 *  Created on: 11/4/2017
 *      Author: utnso
 */

#ifndef FUNCTIONS_SOCKETSCLIENT_H_
#define FUNCTIONS_SOCKETSCLIENT_H_

#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

typedef struct {
	int socketCliente;
	struct sockaddr_in destino;
} t_cliente;

//Crea un nuevo cliente
t_cliente newClient(char* ip, int puerto);

//Crea un nuevo socket
void newSocket(int* socketNuevo);

//Conectarse a un sevidor
int connectServer(t_cliente unCliente);

//Enviar un mensaje al servidor. Devuelve 0 en caso de ser correcto y -1 en caso de fallar
int sendMessageToServer(int socketDestino, char* message);

//Espera respuesta del servidor
char* waitServerResponse(int socketServer);

//Espera respuesta del servidor sabiendo el tamaño del mensaje
char* waitResponseServerTam(int socketServer, int* longMsg);

//Conversar con el servidor
char* chattingWithProcess(int socketProcess, char* message);

//Hacer un handshake con el servidor
char* handShakeClient(int socketServer, char* message);

#endif /* FUNCTIONS_SOCKETSCLIENT_H_ */

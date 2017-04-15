#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#define BACKLOG 5

void create_server(int* listenningSocket, char* port){
	struct addrinfo hints;
	struct addrinfo *serverInfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;

	getaddrinfo(NULL, port, &hints, &serverInfo);

	*listenningSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);

	bind(*listenningSocket,serverInfo->ai_addr, serverInfo->ai_addrlen);
	freeaddrinfo(serverInfo);

	listen(*listenningSocket, BACKLOG);

}

int accept_connection(int listenningSocket, int* socketCliente){

	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);

	*socketCliente = accept(listenningSocket, (struct sockaddr *) &addr, &addrlen);

	return *socketCliente;
}

void create_client(int* serverSocket, char* ip, char* port){

	struct addrinfo hints;
	struct addrinfo *serverInfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	getaddrinfo(ip, port, &hints, &serverInfo);

	*serverSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);

	connect(*serverSocket, serverInfo->ai_addr, serverInfo->ai_addrlen);

	freeaddrinfo(serverInfo);
}

void handShakeServidor(int serverSocket,const char * servidor,char * cliente){


	recv(serverSocket,cliente, sizeof(char)*10, 0);
		printf("me llego  %s \n", cliente);
	send(serverSocket, (void*)servidor, sizeof(char)*10, 0);
		printf("enviado %s \n",servidor);
}



void handShakeCliente(int clienteSocket, char * servidor,const char * cliente){



	send(clienteSocket, (void *)cliente, sizeof(char)*10, 0);//mesnaje a kernel
		printf("envio %s %d\n",cliente,clienteSocket);
	recv(clienteSocket,servidor ,sizeof(char)*10, 0);
		printf("recibo  %s \n",servidor);
}

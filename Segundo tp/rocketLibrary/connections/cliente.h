#ifndef HEADERS_POKECLIENTECONECTIONS_H_
#define HEADERS_POKECLIENTECONECTIONS_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <commons/string.h>

int libconnections_conectar_a_servidor(char * IP, char * PUERTO, int * sock_int);
void *get_in_addr(struct sockaddr *sa);

enum ERROR_CONEXION {
	GETTADDRINFO, SOCKET, BIND, CONNECT
};

#endif

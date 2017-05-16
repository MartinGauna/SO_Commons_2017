/*
 * socket.h
 *
 *  Created on: 7/4/2017
 *      Author: utnso
 */

#ifndef SOCKET_H_
#define SOCKET_H_

#include <commons/log.h>
#include <commons/string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>

#define packageHeaderSize sizeof(uint32_t) + sizeof(uint16_t);

#define BACKLOG 40;

enum codigoID {CONSOLA_HSK = 1894, CPU_HSK, KERNEL_HSK, MEMORIA_HSK, FILESYSTEM_HSK, HOLA};

typedef struct {
	uint16_t code;
	uint32_t size;
	char*	data;
}t_package;

int escuchar(int puerto, int* socket, t_log* logger);
int aceptar(int socket,int* newSocket, t_log* logger);
int cargarSoket(int iPuerto,const char* ip, int* pSocket, t_log* logger);
int enviarHandshake (int socket, uint16_t codigoMio,uint16_t codigoOtro, t_log* logger);
int recibirHandshake (int socket, uint16_t codigoMio, uint16_t* codigoOtro, t_log* logger);
uint32_t packageSize(uint32_t size);
char* compress(int code, char* data, uint32_t size, t_log* logger);
int enviar(int socket, uint16_t code, char* data, uint32_t size, t_log* logger);
int recibir(int socket,t_package* mensaje, t_log* logger);
int recvPkg(int socket, char** buffer, uint32_t size, t_log* logger);

#endif /* SOCKET_H_ */

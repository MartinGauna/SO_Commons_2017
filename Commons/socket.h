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
/*
 * Codigos de operacion para mandar en el header de los mensajes.
 */
enum codigoID {	CONSOLA_HSK = 1894,
				CPU_HSK,
				KERNEL_HSK,
				MEMORIA_HSK,
				FILESYSTEM_HSK,

				//CONSOLA
				INICIAR_PROG,
				FINALIZAR_PROG,

				//KERNEL
				ACEPTAR_PROG,
				RECHAZAR_PROG,
				SOLICITAR_MEM,
				SOLICITAR_PAGINA,
				LIBERAR_MEM,

				ABRIR_ARCHIVO,
				LEER_ARCHIVO,
				ESCRIBIR_ARCHIVO,
				CERRAR_ARCHIVO,



				//MEMORIA
				TAM_PAG,
				MEMORIA_PROG,

				//FILE SYSTEM



				HOLA};
/*
 * Estructura de los mensajes
 * code: Codigo del mensaje, esto indica que es lo que operacion quiero hacer.
 * size: tamaño del contenido a mandar en data
 * data: lo que quiero enviar en el mensaje, el payload.
 */
typedef struct {
	uint16_t code;
	uint32_t size;
	char*	data;
}t_package;

/**
 * @NAME: escuchar
 * @DESC: crea un socket y se encarga de hacer un liste sobre el mismo.
 * @PARAMS: {int} 	puerto Numero de puerto
 * 			{int *}	socket Puntero en el que se almacenara el socket, necesita tener la memoria asignada.
 */
int escuchar(int puerto, int* socket, t_log* logger);

/**
 * @NAME: escuchar
 * @DESC: Acepta una conexion sobre un socket que esta haciendo un listen. por lo que hay que llamar a escuchar antes que a esta funcion.
 * @PARAMS: {int} 	socket Socket que esta escuchando
 * 			{int *}	newSocket Nuevo sokcet generado al hacer el accept.
 */
int aceptar(int socket,int* newSocket, t_log* logger);
int cargarSoket(int iPuerto,const char* ip, int* pSocket, t_log* logger);
int enviarHandshake (int socket, uint16_t codigoMio,uint16_t codigoOtro, t_log* logger);
int recibirHandshake (int socket, uint16_t codigoMio, uint16_t* codigoOtro, t_log* logger);
uint32_t packageSize(uint32_t size);
char* compress(int code, char* data, uint32_t size, t_log* logger);
int enviar(int socket, uint16_t code, char* data, uint32_t size, t_log* logger);
int recibir(int socket,t_package* mensaje, t_log* logger);
int recvPkg(int socket, char** buffer, uint32_t size, t_log* logger);
int highestFD(int , int );

#endif /* SOCKET_H_ */

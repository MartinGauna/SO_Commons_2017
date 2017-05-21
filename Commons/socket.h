/*
 * socket.h
 *
 *  Created on: 7/4/2017
 *      Author: Martin Gauna
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
				PID_INEXISTENTE,
				MATAR_PROCESOS,
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
/**
 * @NAME: cargarSoket
 * @DESC: Crea un socket que se puede usar tanto para listen, si no se le pasa la ip, como para connect,
 * 		  si le paso la ip a la que me quiero conectar. La memoria de la variables puntero debe estar previamente asignada.
 * @PARAMS: {int} 	iPuerto puesto que se le asigna al socket.
 * 			{int*}	ip ip a la que me quiero conectar, o si es null el socket se puede usar para hacer listen.
 * 			{int*}	pSocket Socket creado dentro de la funcion.
 * 			{t_log*} Logger.
 */
int cargarSoket(int iPuerto,const char* ip, int* pSocket, t_log* logger);
/**
 * @NAME: enviarHandshake
 * @DESC: Envia un codigo de handshake,luego recibe la respuesta y chequea que sea la esperada.
 * 		  NOTA IMPORTANTE: no tengo que llamar a recibirHandshake,
 * @PARAMS: {int} 	socket por el que realizo la comunicación.
 * 			{uint16_t}	codigoMio Codigo del programa que llama a la función.
 * 			{uint16_t}	codigoOtro Codigo del programa  al que me quiero conectar.
 * 			{t_log*} Logger.
 */
int enviarHandshake (int socket, uint16_t codigoMio,uint16_t codigoOtro, t_log* logger);
/**
 * @NAME: recibirHandshake
 * @DESC: Recibe un handshake y manda correspondiente la respuesta.
 * 		  NOTA IMPORTANTE: no tengo que llamar a enviarHandshake antes de usar esta funcion.
 * @PARAMS: {int} 	socket por el que realizo la comunicación.
 * 			{uint16_t}	codigoMio Codigo del programa que llama a la función.
 * 			{uint16_t}	codigoOtro Se guarda el codigo de la persona que mandó el handshake.
 * 			{t_log*} Logger.
 */
int recibirHandshake (int socket, uint16_t codigoMio, uint16_t* codigoOtro, t_log* logger);
/**
 * @NAME: packageSize
 * @DESC: Devuelve el tamaño del tipo t_package junto con el tamaño del contenido de data.
 * @PARAMS: {uint32_t} size Tiene que ser el tamaño del contenido de data.
 */
uint32_t packageSize(uint32_t size);
/**
 * @NAME: compress
 * @DESC: Se encarga de crear un bloque de memoria los diferentes datos de un mensaje en un solo bloque de memoria.
 * 		  NOTA: Funcion de uso interna.
 * @PARAMS: {int} 	code Código a enviar en el paquete.
 * 			{char*}	data Datos a enviar.
 * 			{uint32_t} size Tamaño del contenido de 'data'.
 * 			{t_log*} Logger.
 */
char* compress(int code, char* data, uint32_t size, t_log* logger);
/**
 * @NAME: enviar
 * @DESC: Se encar de enviar un mensaje, con el codigo y los datos con las que es llamado.
 * 		  NOTA: el proceso al que le envio los datos tiene que estar esperando con un recibir.
 * @PARAMS: {int} 	socket Código a enviar en el paquete.
 * 			{uint16_t}	code Codigo de operación.
 * 			{char*} data datos que se quieren enviar. El payload.
 * 			{uint32_t} size Tamaño de lo que voy enviar, tamaño del payload.
 * 			{t_log*} Logger.
 */
int enviar(int socket, uint16_t code, char* data, uint32_t size, t_log* logger);
/**
 * @NAME: recibir
 * @DESC: Recibe un mensaje y lo guarda en el segundo parametro, el t_package*.
 * 		  NOTA: Se queda a la espera de que el otro proceso realize un enviar.
 * @PARAMS: {int} 	socket Socket por el que se hace la comunicación.
 * 			{t_package*} mensaje donde se guarda el mensaje recibido.
 * 			{char*} data datos que se quieren enviar. El payload.
 * 			{t_log*} Logger.
 */
int recibir(int socket,t_package* mensaje, t_log* logger);

/**
 * @NAME: recvPkg
 * @DESC: Se encar de hacer el send de forma recursiva hasta que se lea el mensaje completo.
 * 		  NOTA: Funcion de uso interna, no usar.
 * @PARAMS: {int} socket Socket por el que se hace la comunicación.
 * 			{char**} buffer Puntero al char* donde se va a guardar el mensaje.
 * 			{uint32_t} size Tamaño de lo que voy recibir, tamaño del payload.
 * 			{t_log*} Logger.
 */
int recvPkg(int socket, char** buffer, uint32_t size, t_log* logger);

/**
 * @NAME: highestFD
 * @DESC: Compara 2 file descriptors y retorna el valor del mayor más 1 para ser almacenado en el select.
 * @PARAMS: {int} fd File descriptor más grande guardado en el select.
 * 			{int} nfd Nuevo file descriptor.
 */
int highestFD(int , int );

#endif /* SOCKET_H_ */

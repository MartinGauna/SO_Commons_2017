#ifndef NETWORKHANDLER_H_
#define NETWORKHANDLER_H_

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <commons/log.h>
#include <commons/string.h>
#include <pthread.h>
#include </usr/include/netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <commons/log.h>
#include <errno.h>

#define LOG_INFO_PATH "info_logs"
#define LIBRARY_NAME "rocketLibrary"
#define LOG_ERROR_PATH "error_logs"
#define LOG_LEVEL LOG_LEVEL_INFO

int librocket_levantarServidorConMultiplesHilos(char* PUERTO,
		void* (*funcionParaElHilo)(void sockfd));
int librocket_enviarMensaje(int, const void*, size_t, t_log*);
int librocket_levantarServidorMultiplexado(char* PUERTO,
		void (*funcionParaConexionEntrante)(int mSocket,
				struct sockaddr_in remoteaddr), fd_set* setSockets,
		void (*funcionParaMensajeRecibido)(int mSocket));
int librocket_levantarServidorSimple(char * PUERTO,
		int maximo_conexiones_pendientes, void (*function)(int mSocket));
int librocket_recibirMensaje(int, void*, size_t, t_log*);
void loguearUltimoError(t_log* errorLogger, int lineNumber, char* fileName);
int librocket_levantarServidorMultiplexado(char* PUERTO,
		void (*funcionParaConexionEntrante)(int mSocket,
				struct sockaddr_in remoteaddr), fd_set* master,
		void (*funcionParaMensajeRecibido)(int mSocket));
void * hiloManejador(void* sockfd);
void funcionReceptoraDeConexion(int sockfd);
int librocket_levantarServidorMultiplexCon3Puertos(char* PUERTO1, char* PUERTO2,const char* PUERTO3,
		void (*funcionParaConexionEntrante)(int mSocket, char* puerto),
		fd_set* master, void (*funcionParaMensajeRecibido)(int mSocket));
void funcionCreadoraDeHilos(int sockfd, void* (*funcionParaElHilo)(void sockfd));
int librocket_levantarServidorDelKernel(char* PUERTO1, char* PUERTO2,
		const char* PUERTO3, int socketInotify,
		void (*funcionParaConexionEntrante)(int mSocket, char* puerto),
		fd_set* master, void (*funcionParaMensajeRecibido)(int mSocket));

typedef enum RESULTADO_MENSAJE_RECIBIDO {

	CONEXION_CERRADA = 0, NO_SE_PUDO_RECIBIR = -1, RECIBIDO_CORRECTAMENTE = 1

} RESULTADO_MENSAJE_RECIBIDO;

extern int errno;

#endif

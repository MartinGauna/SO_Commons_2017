#ifndef FUNCIONESKERNEL_H

#include "../Commons/configuracion.h"
#include "../Commons/socket.h"
#include <commons/log.h>
#include <commons/config.h>
#include <pthread.h>

#define TRUE  1;
#define FALSE  0;
#define PACKAGESIZE 1024

typedef struct FDSockets {
	   int fd;
	   char nombre[20];
	   int activo;
} FDSocket;

typedef struct{
	int socketCliente;
	fd_set master;
	int socketFS;
	int socketM;
	//FDSocket fdSocket[100];
	int fdmax;
}structParaCliente;
void handShakeServidor(int serverSocket,const char * servidor,char * cliente);

void liberar_memoria(t_log* logger, configKernel* config);
void closeConections(int socketCPU, int socketFS, int socketMemoria, int socketConsola);
void printConfig(configKernel* conf);
void* atenderCliente(void* _parameters);

#endif

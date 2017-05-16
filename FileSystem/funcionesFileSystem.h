
#ifndef TP_2017_1C_LOSPANCHOS_FILESYSTEM_H
#define TP_2017_1C_LOSPANCHOS_FILESYSTEM_H

#include "../Commons2/configuracion.h"
#include "../Commons2/socket.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/txt.h>
#include <features.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#define PACKAGESIZE 1024

typedef struct  {
	int socket;
	t_log* log;
} structParaKernel;

void liberar_memoria(t_log* logger,configFileSystem* config);
void* atenderKernel(void* _parameters);

#endif




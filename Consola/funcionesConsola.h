/*
 * funcionesConsola.h
 *
 *  Created on: 16/5/2017
 *      Author: utnso
 */

#ifndef FUNCIONESCONSOLA_H_
#define FUNCIONESCONSOLA_H_

#include <stdio.h>
#include "../Commons/configuracion.h"
#include "../Commons/socket.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/txt.h>
#include <features.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <pthread.h>

typedef struct {
	int serverSocket;
	FILE* file;
} t_archivoThread;

t_log *logger;
t_log* testlog;

void handShakeCliente(int clienteSocket, char * servidor,const char * cliente);
void consola_imprimir_encabezado();
void consola_imprimir_menu();
void liberar_memoria(t_log* logger,configConsole* config);
void iniciarPrograma(FILE* archivo, int serverSocket);
void* iniciarProgramaPorThread(void* _parameters);

#endif /* FUNCIONESCONSOLA_H_ */

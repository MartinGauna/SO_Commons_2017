//
// Created by Martin Gauna on 4/2/17.
//

#ifndef TP_2017_1C_LOSPANCHOS_KERNEL_H
#define TP_2017_1C_LOSPANCHOS_KERNEL_H

#include "../Commons/configuracion.h"
#include "../Commons/socket.h"
#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/list.h>


extern t_log* logger;

/*
 * Estructura que guarda los datos necesarios para identificar al proceso y todas las conecciones de las que este es parte
 */
typedef struct {
	int consolaId;
	int consolaSocket;
	int cpuSocket;
	int pid;
	char* ansisop;
}t_proceso;

/*
 * Libera la memoria que fue reviamente alocada en el programa.
 */
void liberar_memoria(t_log* logger, configKernel* config);
void closeConections(int socketCPU, int socketFS, int socketMemoria, int socketConsola);
void printConfig(configKernel* conf);
int aceptarConexion(int socketListen,fd_set* readSet,int* nfd, t_dictionary* cpus, t_dictionary* consolas,int* pidCount, t_log* logger);


int atenderConsola(int socket, int* pidCount);
int crearPidSock(int* pidCount);

#endif //TP_2017_1C_LOSPANCHOS_KERNEL_H

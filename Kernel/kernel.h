//
// Created by Martin Gauna on 4/2/17.
//

#ifndef TP_2017_1C_LOSPANCHOS_KERNEL_H
#define TP_2017_1C_LOSPANCHOS_KERNEL_H

#include <stdio.h>
#include <stdlib.h>

#include "../Commons/configuracion.h"
#include "../Commons/socket.h"
#include "../Commons/PCB.h"
#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>

#define STDIN 0
#define PID_STR_COUNT 20+1

extern t_log* logger;
extern t_list* newQueue;
extern t_queue* readyQueue;
extern t_queue* finishQueue;

/*
 * Estructura que guarda los datos necesarios para identificar al proceso y todas las conecciones de las que este es parte
 */
typedef struct {
	int consolaId;
	int consolaSocket;
	int cpuSocket;
	t_pcb pcb;
}t_proceso;

/*
 * Libera la memoria que fue reviamente alocada en el programa.
 */
void liberar_memoria(t_log* logger, configKernel* config);
void closeConections(int socketCPU, int socketFS, int socketMemoria, int socketConsola);
void printConfig(configKernel* conf);
int aceptarConexion(int socketListen,fd_set* readSet,int* nfd, t_dictionary* cpus, t_dictionary* consolas,int* pidCount, t_log* logger);


int atenderConsola(int socket, int* pidCount);
t_proceso* crearProceso(int socket, int* pidCount, char* ansisop);
int crearPidSock(int* pidCount);
int atenderConsolaKernel();

#endif //TP_2017_1C_LOSPANCHOS_KERNEL_H

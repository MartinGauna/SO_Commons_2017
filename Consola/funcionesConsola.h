/*
 * funcionesConsola.h
 *
 *  Created on: 16/5/2017
 *      Author: utnso
 */

#ifndef FUNCIONESCONSOLA_H_
#define FUNCIONESCONSOLA_H_

#include "../Commons/configuracion.h"
#include "../Commons/socket.h"
#include <commons/log.h>
#include <commons/config.h>
#include <stdio.h>

typedef struct {
	int serverSocket;
	char* path;
} t_archivoThread;

void consola_imprimir_encabezado();
void consola_imprimir_menu();
void liberar_memoria(t_log* logger,configConsole* config);
void iniciarPrograma(char* path, int serverSocket);
void* iniciarProgramaPorThread(void* _parameters);

#endif /* FUNCIONESCONSOLA_H_ */
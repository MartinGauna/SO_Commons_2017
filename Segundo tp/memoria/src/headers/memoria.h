#ifndef MEMORIA_H_
#define MEMORIA_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <commons/log.h>
#include <stdbool.h>
#include <connections/networkHandler.h>
#include <structs/structs.h>
#include <pthread.h>
#include "threads/threads.h"

#define LOG_PATH "memoria.log"

void cargarConfig(char**);
void liberarMemoria();
void* servidorMemoria(int sockfd);
void funcionKernel(int socket);
void funcionCpu(int socket);
void iniciarMemoria(int argc, char* argv[]);
void inicializarMemoria();
void crearEstructuras();
void imprimirAyuda();
void imprimirConfig();

t_memoria_config* memoria_config;
void* memoria;

/**
 *  (indice: mas usada a menos usada)
 * 	int32_t pid;
	int32_t paginaId;
	char* pagina;
 */

t_list* cache;
t_list* contadorAuxCache;
t_list* tipoDePaginas;

/**
 *  (indice: frame)
	int32_t pid;
	int32_t paginaId;
 */
t_list* tablaPaginas;
t_log* infoLogger;
t_estructurasMemoria* estructurasMemoria;

#endif

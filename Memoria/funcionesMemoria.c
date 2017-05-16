/*
 * funcionesMemoria.c
 *
 *  Created on: 16/5/2017
 *      Author: utnso
 */
#include "funcionesMemoria.h"
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
#include <commons/collections/list.h>
#include <features.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <commons/collections/dictionary.h>

pthread_mutex_t memoriaLibre;
pthread_mutex_t semaforoMemoria;
int entradasCache;
t_list* caches;
t_list* marcosLibres;
t_log* log_memoria;
t_dictionary* programas_ejecucion; // liberar al desalojar
t_dictionary* programas_paraClock; // liberar al desalojar
int* idProcesoActual;
void* memoriaPrincipal;

void consolaMem_imprimir_encabezado(){
	printf("**** BIENVENIDO A LA CONSOLA MEMORIA****\n");
	printf("\n");
}
void consolaMem_imprimir_menu(){
	printf("Por favor seleccione la opcion correspondiente:\n");
	printf("\n");
	printf("1) Retardo\n");
	printf("2) Dump\n");
	printf("3) Flush\n");
	printf("4) Size\n");
}

void liberar_memoria(t_log* logger,configMemoria* config) {
	free(logger);
	free(config);
}

void* inicializarMemoria(structMemoria* structMemoria) {

	int cantidadDeMarcos = structMemoria->configMemoria.marcos;
	int tamanio = structMemoria->configMemoria.marcoSize;

	entradasCache = structMemoria->configMemoria.entradasCache;

	printf("Entradas Cache: %d \n", entradasCache);

    caches = list_create();

//	//inicializarCache(caches,entradasCache);
//	log_memoria = log_create("../logs/logMemoria.txt","MEMORIA",0,LOG_LEVEL_TRACE);

	pthread_mutex_init(&semaforoMemoria, NULL);
	pthread_mutex_init(&memoriaLibre, NULL);
//	log_trace(log_memoria,"Creando memoria");

	marcosLibres = list_create();
//	programas_ejecucion = dictionary_create();
	idProcesoActual = malloc(sizeof(int));
	int i = 1;

//	memoriaPrincipal = malloc(cantidadDeMarcos * tamanio);
////	int tamanio = cantidadDeMarcos * structMemoria->configMemoria.MARCO_SIZE;
////	log_trace(log_memoria,"Memoria inicializada con un tamanio: %d",tamanio);
//
//	for(i = 0;i<cantidadDeMarcos;i++){
//		int* tempFrame = malloc(sizeof(int));
//		*tempFrame = i;
//		list_add(marcosLibres,tempFrame);
//	}
////	log_trace(log_memoria,"Marcos libres cargados: %d:",i);
////
//	return memoriaPrincipal;
	return NULL;
}

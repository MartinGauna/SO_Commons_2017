#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <commons/log.h>
#include <stdbool.h>
#include <connections/networkHandler.h>
#include <structs/structs.h>
#include "headers/testGeneral.h"
#include <commons/collections/list.h>
#include "headers/dumps.h"

void pruebaMalloc(){
	t_list* lista = list_create();
	int h = 5;
	list_add(lista, &h);
	int* puntero = (int*)list_get(lista, 0);
	printf("%d", *puntero);

}

void testEscribirSucesivo(t_estructurasMemoria* estructurasMemoria){

	t_direccion_logica* punteroMolesto = malloc(sizeof(t_direccion_logica));
	punteroMolesto->pid=1;
	punteroMolesto->paginaId=0;
	punteroMolesto->tamanio = 5;
	punteroMolesto->offset = 0;

	int varr = 8;
	int* escritura = &varr;

//	escribirDato(estructurasMemoria, escritura, punteroMolesto);
	dumpContenido(estructurasMemoria, 1, NULL);
//	leerDato(estructurasMemoria, leerAca, punteroMolesto);
	punteroMolesto->offset = 4;
	escribirDato(estructurasMemoria, escritura, punteroMolesto);
	dumpContenido(estructurasMemoria, 1, NULL);
}

void testEscribirYLeer(t_estructurasMemoria* estructurasMemoria){

	t_direccion_logica* direccionLogica = malloc(sizeof(t_direccion_logica));
	direccionLogica->pid=2;
	direccionLogica->paginaId=8;
	direccionLogica->offset = 5;
	direccionLogica->tamanio = 4;

	const char* escritura = "DANI";
	char* lectura = malloc(direccionLogica->tamanio);

	//Escribo
	printf("Escribiendo en memoria... esto tarda %d segundos\n", estructurasMemoria->memoria_config->retardoMemoria);
	escribirDato(estructurasMemoria, escritura, direccionLogica);

	//Leo
	printf("Leyendo de memoria (pero la pagina esta cacheada, no debe tardar nada)\n");
	leerDato(estructurasMemoria, &lectura, direccionLogica);

	printf("Mostrando lo leido... \n");
	int i;
	for (i=0;i<direccionLogica->tamanio;i++){
		printf("%c", *(lectura + i));
	}

	const char* escritura2 = " CAPO";
	direccionLogica->offset = 9;
	direccionLogica->tamanio=5;

	//Escribo
	printf("\nEscribiendo en memoria... (Pagina cacheada, no tarda)\n");
	escribirDato(estructurasMemoria, escritura2, direccionLogica);

	//Leo
	printf("Leyendo de memoria (pero la pagina esta cacheada, no debe tardar nada)\n");
	leerDato(estructurasMemoria, &lectura, direccionLogica);

	printf("Mostrando lo leido... \n");
	for (i=0;i<direccionLogica->tamanio;i++){
		printf("%c", *(lectura + i));
	}

	printf("\n");

}

void testCache(t_estructurasMemoria* estructurasMemoria){
	crearMemoriaDummy(estructurasMemoria);
	char* punteroAEnviar;
	t_direccion_logica* direccionLogica = malloc(sizeof(t_direccion_logica));
	direccionLogica->pid=2;
	direccionLogica->paginaId=8;
	direccionLogica->offset = 5;
	direccionLogica->tamanio = 1;

	printf("Lee una pagina a memoria sin cache... esperando el retardo de %d segundos\n", estructurasMemoria->memoria_config->retardoMemoria);
	leerDato(estructurasMemoria, punteroAEnviar, direccionLogica);
	printf("Lee una pagina a cache... (sin retardo)\n");
	leerDato(estructurasMemoria, punteroAEnviar, direccionLogica);
	printf("Lectura finalizada\n");
}


void probar_listas(){
	t_list* lista = list_create();
	registroCacheMemoria* nodo1 = malloc(sizeof(registroCacheMemoria));
	registroCacheMemoria* nodo2 = malloc(sizeof(registroCacheMemoria));
	registroCacheMemoria* nodo3 = malloc(sizeof(registroCacheMemoria));
	registroCacheMemoria* nodo4 = malloc(sizeof(registroCacheMemoria));
	registroCacheMemoria* nodo5 = malloc(sizeof(registroCacheMemoria));

	nodo1->paginaId = 1;
	nodo2->paginaId = 2;
	nodo3->paginaId = 3;
	nodo4->paginaId = 4;
	nodo5->paginaId = 5;

	list_add(lista, nodo1);
	list_add(lista, nodo2);
	list_add(lista, nodo3);
	list_add(lista, nodo4);
	list_add(lista, nodo5);

	registroCacheMemoria* r = list_remove(lista, 2);
	list_add_in_index(lista, 0, r);


	int i;
	for (i=0;i<list_size(lista);i++){
		registroCacheMemoria* d = (registroCacheMemoria*) list_get(lista, i);
		printf("%d", d->paginaId);
	}

	printf("%d", r->paginaId);

}

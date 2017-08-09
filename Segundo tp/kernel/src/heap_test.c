#include "headers/kernel.h"
#include "headers/capaMemoria.h"
#include "headers/conexionesKernelCpu.h"
#include "headers/manejo_heap.h"

void ver_pagina(int nro_pagina, int pid);
void test_sin_memoria();


void test2() {

	/*

	t_bloque* bloque_1 = (t_bloque*) malloc(sizeof(t_bloque));
	t_bloque* bloque_2 = (t_bloque*) malloc(sizeof(t_bloque));
	t_bloque* bloque_3 = (t_bloque*) malloc(sizeof(t_bloque));
	t_bloque* bloque_4 = (t_bloque*) malloc(sizeof(t_bloque));
	t_bloque* bloque_5 = (t_bloque*) malloc(sizeof(t_bloque));
	t_bloque* bloque_6 = (t_bloque*) malloc(sizeof(t_bloque));
	t_bloque* bloque_7 = (t_bloque*) malloc(sizeof(t_bloque));

	bloque_1->isUsed = true;
	bloque_1->size = 10;

	bloque_2->isUsed = false;
	bloque_2->size = 10;

	bloque_3->isUsed = false;
	bloque_3->size = 10;

	bloque_4->isUsed = false;
	bloque_4->size = 10;

	bloque_5->isUsed = true;
	bloque_5->size = 10;

	bloque_6->isUsed = false;
	bloque_6->size = 10;

	bloque_7->isUsed = false;
	bloque_7->size = 10;

	nodo_tabla_heap* pagina = (nodo_tabla_heap*) malloc(
			sizeof(nodo_tabla_heap));
	pagina->lista_de_bloques = list_create();

	list_add(pagina->lista_de_bloques, bloque_1);
	list_add(pagina->lista_de_bloques, bloque_2);
	list_add(pagina->lista_de_bloques, bloque_3);
	list_add(pagina->lista_de_bloques, bloque_4);
	list_add(pagina->lista_de_bloques, bloque_5);
	list_add(pagina->lista_de_bloques, bloque_6);
	list_add(pagina->lista_de_bloques, bloque_7);

	int a = list_size(pagina->lista_de_bloques);

	tablaHeap = list_create();
	list_add(tablaHeap, pagina);

	compactar(0);

	*/


}

void test_3(){


	asignar_paginas_heap(32, 1, 1);


	t_bloque* bloque_1 = (t_bloque*) malloc(sizeof(t_bloque));
	t_bloque* bloque_2 = (t_bloque*) malloc(sizeof(t_bloque));
	t_bloque* bloque_3 = (t_bloque*) malloc(sizeof(t_bloque));
	t_bloque* bloque_4 = (t_bloque*) malloc(sizeof(t_bloque));
	t_bloque* bloque_5 = (t_bloque*) malloc(sizeof(t_bloque));
	t_bloque* bloque_6 = (t_bloque*) malloc(sizeof(t_bloque));
	t_bloque* bloque_7 = (t_bloque*) malloc(sizeof(t_bloque));
	t_bloque* bloque_8 = (t_bloque*) malloc(sizeof(t_bloque));
	t_bloque* bloque_9 = (t_bloque*) malloc(sizeof(t_bloque));
	t_bloque* bloque_10 = (t_bloque*) malloc(sizeof(t_bloque));

	bloque_1->isUsed = true;
	bloque_1->size = 10;

	bloque_2->isUsed = false;
	bloque_2->size = 10;

	bloque_3->isUsed = true;
	bloque_3->size = 10;

	bloque_4->isUsed = false;
	bloque_4->size = 10;

	bloque_5->isUsed = false;
	bloque_5->size = 10;

	bloque_6->isUsed = false;
	bloque_6->size = 10;

	bloque_7->isUsed = true;
	bloque_7->size = 10;

	bloque_8->isUsed = false;
	bloque_8->size = 10;

	bloque_9->isUsed = false;
	bloque_9->size = 10;

	bloque_10->isUsed = false;
	bloque_10->size = 10;

	nodo_tabla_heap* pagina = (nodo_tabla_heap*) malloc(sizeof(nodo_tabla_heap));
	pagina->lista_de_bloques = list_create();

	list_add(pagina->lista_de_bloques, bloque_1);
	list_add(pagina->lista_de_bloques, bloque_2);
	list_add(pagina->lista_de_bloques, bloque_3);
	list_add(pagina->lista_de_bloques, bloque_4);

	list_add(pagina->lista_de_bloques, bloque_5);
	list_add(pagina->lista_de_bloques, bloque_6);
	list_add(pagina->lista_de_bloques, bloque_7);
	list_add(pagina->lista_de_bloques, bloque_8);
	list_add(pagina->lista_de_bloques, bloque_9);

	tablaHeap = list_create();
	list_add(tablaHeap, pagina);

}

void test_paginas_heap(){



	asignar_paginas_heap(32, 1, 1);
	asignar_paginas_heap(10, 1, 1);

	// nodo_tabla_heap * pagina = list_get(tablaHeap, 0);
	//t_bloque * bloque2 = list_get(pagina->lista_de_bloques, 1);



}
void test_sin_memoria_2() {

	//TEST ASIGNAR PAGINAS HEAP!
	kernel_config = malloc(sizeof(t_kernel_config));
	kernel_config->pageSize = 100;

	tablaHeap = list_create();
	colaListos = list_create();
	colaEjecutando = list_create();
	colaBloqueados = list_create();
	colaNuevos = list_create();

	t_pcb* pcb_1 = malloc(sizeof(t_pcb));
	//t_pcb* pcb_2=malloc(sizeof(t_pcb));
	//t_pcb* pcb_3=malloc(sizeof(t_pcb));

	list_add(colaListos, pcb_1);
	//list_add(colaListos, pcb_2);
	//list_add(colaListos, pcb_3);

	pcb_1->PID = 1;
	pcb_1->cantidad_paginas_totales = 3;
	/*pcb_2->PID=2;
	 pcb_2->cantidad_paginas_totales=1;
	 pcb_3->PID=3;
	 pcb_3->cantidad_paginas_totales=4;
	 */

	t_bloque* bloque_1 = (t_bloque*) malloc(sizeof(t_bloque));
	t_bloque* bloque_2 = (t_bloque*) malloc(sizeof(t_bloque));

	bloque_1->isUsed = true;
	bloque_1->size = 50;
	bloque_1->offset = 5;
	bloque_1->nro_bloque = 0;
	bloque_1->fin = 55;

	bloque_2->isUsed = false;
	bloque_2->size = 40;
	bloque_2->offset = 60;
	bloque_2->nro_bloque = -1;
	bloque_2->fin = 100;

	nodo_tabla_heap* pagina = (nodo_tabla_heap*) malloc(
			sizeof(nodo_tabla_heap));
	pagina->lista_de_bloques = list_create();
	pagina->PID = 1;
	pagina->numeroPagina = 3;

	list_add(pagina->lista_de_bloques, bloque_1);
	list_add(pagina->lista_de_bloques, bloque_2);

	tablaHeap = list_create();
	list_add(tablaHeap, pagina);

	asignar_paginas_heap(30, 1, 1);
	//Sobra un bloque de 5
	asignar_paginas_heap(10, 1, 1);
	asignar_paginas_heap(20, 1, 1);

	asignar_paginas_heap(50, 1, 1);

	/*
	 asignar_paginas_heap(10, 2, 1);
	 asignar_paginas_heap(10, 2, 1);
	 asignar_paginas_heap(10, 2, 1);
	 asignar_paginas_heap(10, 2, 1);
	 asignar_paginas_heap(10, 2, 1);
	 asignar_paginas_heap(10, 2, 1);

	 */

	//ver_pagina(1,2);
	//asignar_paginas_heap(90, 3, 1);
	//TEST LIBERAR BLOQUE HEAP
	t_direccion_logica puntero;
	puntero.pid = 1;
	puntero.paginaId = 4;
	puntero.tamanio = 10;
	puntero.offset = 5;

	t_direccion_logica puntero_2;
		puntero_2.pid = 1;
		puntero_2.paginaId = 4;
		puntero_2.tamanio = 50;
		puntero_2.offset = 45;

	/*t_direccion_logica puntero_2;
	 puntero_2.pid = 2;
	 puntero_2.paginaId = 1;
	 puntero_2.tamanio = 10;
	 puntero_2.offset = 20;
	 */

	t_puntero puntero_a = posicionMemoria_a_puntero(puntero,
			kernel_config->pageSize);

	t_puntero puntero_b = posicionMemoria_a_puntero(puntero_2,
				kernel_config->pageSize);

	//t_puntero puntero_b = posicionMemoria_a_puntero(puntero_2,kernel_config->pageSize);

	liberar_bloque_heap(puntero_a, 1, 1);
	liberar_bloque_heap(puntero_b, 1, 1);

	informar_paginas_heap(1);
	//informar_paginas_heap(2);
	//informar_paginas_heap(3);

}


void test_sin_memoria_3(){

	//TEST ASIGNAR PAGINAS HEAP!
		kernel_config = malloc(sizeof(t_kernel_config));
		kernel_config->pageSize = 100;

		tablaHeap = list_create();
		colaListos = list_create();
		colaEjecutando = list_create();
		colaBloqueados = list_create();
		colaNuevos = list_create();

		t_pcb* pcb_1=malloc(sizeof(t_pcb));
	//	t_pcb* pcb_2=malloc(sizeof(t_pcb));
	//	t_pcb* pcb_3=malloc(sizeof(t_pcb));

		list_add(colaListos, pcb_1);
	//	list_add(colaListos, pcb_2);
	//	list_add(colaListos, pcb_3);

		pcb_1->PID=1;
		pcb_1->cantidad_paginas_totales=3;
	//	pcb_2->PID=2;
	//	pcb_2->cantidad_paginas_totales=1;
	//	pcb_3->PID=3;
	//	pcb_3->cantidad_paginas_totales=4;


		asignar_paginas_heap(50, 1, 1);
		asignar_paginas_heap(10, 1, 1);


	//	asignar_paginas_heap(20, 1, 1);
	//
	//	asignar_paginas_heap(10, 1, 1);
	//
	//	asignar_paginas_heap(10, 2, 1);
	//	asignar_paginas_heap(10, 2, 1);
	//	asignar_paginas_heap(10, 2, 1);
	//	asignar_paginas_heap(10, 2, 1);
	//	asignar_paginas_heap(10, 2, 1);
	//	asignar_paginas_heap(10, 2, 1);

		//ver_pagina(1,2);

		//asignar_paginas_heap(90, 3, 1);

		//TEST LIBERAR BLOQUE HEAP


		t_direccion_logica puntero_1;
		puntero_1.pid = 1;
		puntero_1.paginaId = 3;
		puntero_1.tamanio = 50;
		puntero_1.offset =5;

	//	t_direccion_logica puntero_2;
	//	puntero_2.pid = 2;
	//	puntero_2.paginaId = 1;
	//	puntero_2.tamanio = 10;
	//	puntero_2.offset = 20;

		t_puntero puntero_a = posicionMemoria_a_puntero(puntero_1,kernel_config->pageSize);

	//	t_puntero puntero_b = posicionMemoria_a_puntero(puntero_2,kernel_config->pageSize);

		liberar_bloque_heap(puntero_a, 1,1);

		asignar_paginas_heap(10, 1, 1);


		nodo_tabla_heap * pagina_struct = buscar_pagina(3, 1);
		int i=0;

		for(; i< list_size(pagina_struct->lista_de_bloques); i++){

			t_bloque * bloque_ver=list_get(pagina_struct->lista_de_bloques, i);

			int a=123;

		}



	//	liberar_bloque_heap(puntero_b, 2, 1);


		//informar_paginas_heap(1);
		//informar_paginas_heap(2);
		//informar_paginas_heap(3);


}

void test_sin_memoria() {

	//TEST ASIGNAR PAGINAS HEAP!
	kernel_config = malloc(sizeof(t_kernel_config));
	kernel_config->pageSize = 100;

	tablaHeap = list_create();
	colaListos = list_create();
	colaEjecutando = list_create();
	colaBloqueados = list_create();
	colaNuevos = list_create();

	t_pcb* pcb_1=malloc(sizeof(t_pcb));
	t_pcb* pcb_2=malloc(sizeof(t_pcb));
	t_pcb* pcb_3=malloc(sizeof(t_pcb));

	list_add(colaListos, pcb_1);
	list_add(colaListos, pcb_2);
	list_add(colaListos, pcb_3);

	pcb_1->PID=1;
	pcb_1->cantidad_paginas_totales=3;
	pcb_2->PID=2;
	pcb_2->cantidad_paginas_totales=1;
	pcb_3->PID=3;
	pcb_3->cantidad_paginas_totales=4;


	asignar_paginas_heap(50, 1, 1);
	asignar_paginas_heap(10, 1, 1);
	asignar_paginas_heap(20, 1, 1);

	asignar_paginas_heap(10, 1, 1);

	asignar_paginas_heap(10, 2, 1);
	asignar_paginas_heap(10, 2, 1);
	asignar_paginas_heap(10, 2, 1);
	asignar_paginas_heap(10, 2, 1);
	asignar_paginas_heap(10, 2, 1);
	asignar_paginas_heap(10, 2, 1);

	ver_pagina(1,2);

	asignar_paginas_heap(90, 3, 1);

	//TEST LIBERAR BLOQUE HEAP


	t_direccion_logica puntero_1;
	puntero_1.pid = 2;
	puntero_1.paginaId = 1;
	puntero_1.tamanio = 10;
	puntero_1.offset =5;

	t_direccion_logica puntero_2;
	puntero_2.pid = 2;
	puntero_2.paginaId = 1;
	puntero_2.tamanio = 10;
	puntero_2.offset = 20;

	t_puntero puntero_a = posicionMemoria_a_puntero(puntero_1,kernel_config->pageSize);

	t_puntero puntero_b = posicionMemoria_a_puntero(puntero_2,kernel_config->pageSize);

	liberar_bloque_heap(puntero_a, 2,1);
	liberar_bloque_heap(puntero_b, 2, 1);


	//informar_paginas_heap(1);
	//informar_paginas_heap(2);
	//informar_paginas_heap(3);




}

void ver_pagina(int nro_pagina, int pid){

	int size = list_size(tablaHeap);

	int i=0;

	for(; i<size; i++){

		nodo_tabla_heap * pagina = list_get(tablaHeap, i);

		if(pagina->PID==pid && pagina->numeroPagina==nro_pagina){

			int j =0;

			for(; j< list_size(pagina->lista_de_bloques); j++){

			//	t_bloque* bloque = list_get(pagina->lista_de_bloques,j);


			}



		}
	}


}

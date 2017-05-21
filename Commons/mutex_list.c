/*
 * mutex_list.c
 *
 *  Created on: 21/5/2017
 *      Author: utnso
 */

#include "mutex_list.h"

void agregar_elem(t_list* lista, pthread_mutex_t mutex, void* elem) {

	pthread_mutex_lock(&mutex);
	list_add(lista, elem);
	pthread_mutex_unlock(&mutex);

}

void quitar_elem(t_list* lista, pthread_mutex_t mutex, void* elem, void* contenedor) {

	pthread_mutex_lock(&mutex);
	contenedor = list_get(lista, elem);
	pthread_mutex_unlock(&mutex);

}

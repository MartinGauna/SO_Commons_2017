/*
 * mutex_list.h
 *
 *  Created on: 21/5/2017
 *      Author: utnso
 */

#ifndef MUTEX_LIST_H_
#define MUTEX_LIST_H_

#include <commons/collections/list.h>
#include <pthread.h>

void agregar_elem(t_list* lista,pthread_mutex_t mutex, void* elem);
void quitar_elem(t_list* lista,pthread_mutex_t mutex, void* elem, void* contenedor);


#endif /* MUTEX_LIST_H_ */

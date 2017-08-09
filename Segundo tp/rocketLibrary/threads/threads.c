/*
 * threads.c
 *
 *  Created on: 15/9/2016
 *      Author: utnso
 */

#include <pthread.h>
#include <commons/log.h>
#include "threads.h"

//tiene el mismo retorno que pthread_create()
//0 en caso de exito y un numero si fallo
int crearHilo(pthread_t * punteroAlhilo, void * (*funcionDelHilo)(void*),
		void* parametrosDeFuncion) {
	t_log * logger = log_create("errores_de_hilos", "rocketLibrary", false,
			LOG_LEVEL_ERROR);
	int result = pthread_create(punteroAlhilo, NULL, funcionDelHilo,
			parametrosDeFuncion);
	if (result != 0)
		loguearUltimoError(logger, __LINE__, __FILE__);

	return result;

}

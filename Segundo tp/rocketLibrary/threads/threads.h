/*
 * threads.h
 *
 *  Created on: 15/9/2016
 *      Author: utnso
 */

#ifndef THREADS_THREADS_H_
#define THREADS_THREADS_H_

int crearHilo(pthread_t * punteroAlhilo,void * (*funcionDelHilo) (void*),
			void* parametrosDeFuncion);


#endif /* THREADS_THREADS_H_ */

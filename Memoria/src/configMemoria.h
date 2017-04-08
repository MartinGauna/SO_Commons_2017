/*
 * configMemoria.h
 *
 *  Created on: 8/4/2017
 *      Author: utnso
 */

#ifndef CONFIGMEMORIA_H_
#define CONFIGMEMORIA_H_

typedef struct {
	int PUERTO;
	int MARCOS;
	int MARCO_SIZE;
	int ENTRADAS_CACHE;
	int CACHE_X_PROC;
	char* REEMPLAZO_CACHE;//wtf aparece en el ejemplo pero no en la tabla
	int RETARDO_MEMORIA;
} t_configMemoria;

#endif /* CONFIGMEMORIA_H_ */

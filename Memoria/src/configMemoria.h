/*
 * configMemoria.h
 *
 *  Created on: 8/4/2017
 *      Author: utnso
 */

#ifndef CONFIGMEMORIA_H_
#define CONFIGMEMORIA_H_

typedef struct {
	int PUERTO2; //lo mismo que fs otra variable llamada puerto hace ruido, por eso puerto2
	int MARCOS;
	int MARCO_SIZE;
	int ENTRADAS_CACHE;
	int CACHE_X_PROC;
	char* REEMPLAZO_CACHE;//wtf aparece en el ejemplo pero no en la tabla
	int RETARDO_MEMORIA;
} t_configMemoria;

t_configMemoria* leeConfigMemoria();

#endif /* CONFIGMEMORIA_H_ */

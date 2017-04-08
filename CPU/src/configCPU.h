/*
 * configCPU.h
 *
 *  Created on: 8/4/2017
 *      Author: utnso
 */

#ifndef CONFIGCPU_H_
#define CONFIGCPU_H_

typedef struct {
	int PUERTO;
	int MARCOS;
	int MARCO_SIZE;
	int ENTRADAS_CACHE;
	int CACHE_X_PROC;
	int REEMPLAZO_CACHE;
	int RETARDO_MEMORIA;
} t_configCPU;

t_configCPU leerArchConfigCPU(char* path);


#endif /* CONFIGCPU_H_ */

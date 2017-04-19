/*
 * configCPU.h
 *
 *  Created on: 19/4/2017
 *      Author: utnso
 */

#ifndef CONFIGCPU_H_
#define CONFIGCPU_H_

#include <commons/config.h>

typedef struct {
	char* IP_KERNEL;
	int PUERTO_KERNEL;
} t_configCPU;

//Lee el archivo de configuracion que se pasa por parametro de la consola y crea el struct correspondiente
t_configCPU* leerConfigCPU();

#endif /* CONFIGCPU_H_ */

/*
 * configKernel.c
 *
 *  Created on: 8/4/2017
 *      Author: utnso
 */

#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <functions/othersFunctions.h>
#include "configKernel.h"

t_config *Config = NULL;
char* path = NULL;

t_configKernel leerArchConfigKernel(char* unPath) {

	path = unPath;
	Config = config_create(path);
	t_configKernel* punteroConfig = malloc((sizeof(int) + sizeof(char*)));
	t_configKernel configuracion = *punteroConfig;
	setInt(&configuracion.PUERTO_PROG, "PUERTO_PROG");
	setInt(&configuracion.PUERTO_CPU, "PUERTO_CPU");
	setChar(&configuracion.IP_MEMORIA, "IP_MEMORIA");
	setInt(&configuracion.PUERTO_MEMORIA, "PUERTO_MEMORIA");
	setChar(&configuracion.IP_FS, "IP_FS");
	setInt(&configuracion.PUERTO_FS, "PUETO_FS");
	setInt(&configuracion.QUANTUM, "QUANTUM");
	setInt(&configuracion.QUANTUM_SLEEP, "QUANTUM_SLEEP");
	setChar(&configuracion.ALGORITMO, "ALGORITMO");
	setInt(&configuracion.GRADO_MULTIPROG, "GRADO_MULTIPROG");
	//setChar(&configuracion.SEM_IDS, "SEM_IDS");
	//setInt(&configuracion.SEM_INIT, "SEM_INIT");
	//setChar(&configuracion.SHARED_VARS, "SHARED_VARS");
	setInt(&configuracion.STACK_SIZE, "STACK_SIZE");
	config_destroy(Config);
	free(punteroConfig);

	return configuracion;
}

void setInt(int* intSet, char* parametroBuscado) {
	if(config_has_property(Config, parametroBuscado)) {
		*intSet = config_get_string_value(Config, parametroBuscado);
		printf("%s = %s \n", parametroBuscado, *intSet);
	}
	else {
		printf("No se encontro %s \n", parametroBuscado);
	}
}

void setChar(char** charSet, char* parametroBuscado) {
	if(config_has_property(Config, parametroBuscado)) {
		*charSet = strdup(config_get_string_value(Config, parametroBuscado));
		printf("%s = %s \n", parametroBuscado, *charSet);
	}
	else {
		printf("No se encontro %s \n", parametroBuscado);
	}
}

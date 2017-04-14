/*
 * configMemoria.c
 *
 *  Created on: 8/4/2017
 *      Author: utnso
 */

#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include "configMemoria.h"

t_config *Config = NULL;
char* path = NULL;

t_configMemoria leeArchConfigMemoria(char* unPath) {

	path = unPath;
	Config = config_create(path);
	t_configMemoria* punteroConfig = malloc((sizeof(int)*6 + sizeof(char*)));
	t_configMemoria configuracion = *punteroConfig;
	setInt(&configuracion.PUERTO, "PUERTO");
	setInt(&configuracion.MARCOS, "MARCOS");
	setInt(&configuracion.MARCO_SIZE, "MARCOS_SIZE");
	setInt(&configuracion.ENTRADAS_CACHE, "ENTRADAS_CACHE");
	setInt(&configuracion.CACHE_X_PROC, "CACHE_X_PROC");
	setChar(&configuracion.REEMPLAZO_CACHE,"REEMPLAZO_CACHE");
	setInt(&configuracion.RETARDO_MEMORIA, "RETARDO_MEMORIA");
	config_destroy(Config);
	free(punteroConfig);

	return configuracion;
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

void setInt(int* intSet, char* parametroBuscado) {
	if(config_has_property(Config, parametroBuscado)) {
		*intSet = config_get_string_value(Config, parametroBuscado);
		printf("%s = %d \n", parametroBuscado, *intSet);
	}
	else {
		printf("No se encontro %s \n", parametroBuscado);
	}
}


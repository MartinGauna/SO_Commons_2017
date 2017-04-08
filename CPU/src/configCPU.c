/*
 * configCPU.c
 *
 *  Created on: 8/4/2017
 *      Author: utnso
 */

#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "configCPU.h"

t_config *Config = NULL;
char* path = NULL;

t_configCPU leeArchConfigConsola(char* unPath) {

	path = unPath;
	Config = config_create(path);
	t_configCPU* punteroConfig = malloc((sizeof(int)*7));
	t_configCPU configuracion = *punteroConfig;
	setInt(&configuracion.PUERTO,"PUERTO");
	setInt(&configuracion.MARCOS,"MARCOS");
	setInt(&configuracion.MARCO_SIZE,"MARCO_SIZE");
	setInt(&configuracion.ENTRADAS_CACHE,"ENTRADAS_CACHE");
	setInt(&configuracion.CACHE_X_PROC,"CACHE_X_PROC");
	setInt(&configuracion.REEMPLAZO_CACHE,"REEMPLAZO_CACHE");
	setInt(&configuracion.RETARDO_MEMORIA,"RETARDO_MEMORIA");
	config_destroy(Config);
	free(punteroConfig);

	return configuracion;
}

void setInt(char** intSet, char* parametroBuscado) {
	if(config_has_property(Config, parametroBuscado)) {
		*intSet = strdup(config_get_string_value(Config, parametroBuscado));
		printf("%s = %s \n", parametroBuscado, *intSet);
	}
	else {
		printf("No se encontro %s \n", parametroBuscado);
	}
}

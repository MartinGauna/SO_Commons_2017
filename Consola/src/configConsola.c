/*
 * configConsola.c
 *
 *  Created on: 6/4/2017
 *      Author: utnso
 */

#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include "configConsola.h"

t_config *Config = NULL;
char* path = NULL;

t_configConsola leeArchConfigConsola(char* unPath) {

	path = unPath;
	Config = config_create(path);
	t_configConsola* punteroConfig = malloc((sizeof(int) + sizeof(char*)));
	t_configConsola configuracion = *punteroConfig;
	setChar(&configuracion.IP_KERNEL,"IP_KERNEL");
	setInt(&configuracion.PUERTO_KERNELL, "PUERTO_KERNEL");
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

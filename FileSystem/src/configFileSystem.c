/*
 * configFileSystem.c
 *
 *  Created on: 8/4/2017
 *      Author: utnso
 */

#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include "configFileSystem.h"

t_config *Config = NULL;
char* path = NULL;

t_configFileSystem leerArchConfigFileSystem(char* unPath) {

	path = unPath;
	Config = config_create(path);
	t_configFileSystem* punteroConfig = malloc((sizeof(int) + sizeof(char*)));
	t_configFileSystem configuracion = *punteroConfig;
	setChar(&configuracion.PUNTO_MONTAJE,"PUNTO_MONTAJE");
	setInt(&configuracion.PUERTO, "PUERTO");
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
		printf("%s = %s \n", parametroBuscado, *intSet);
	}
	else {
		printf("No se encontro %s \n", parametroBuscado);
	}
}

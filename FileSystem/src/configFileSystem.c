/*
 * configFileSystem.c
 *
 *  Created on: 8/4/2017
 *      Author: utnso
 */

#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <functions/othersFunctions.h>
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

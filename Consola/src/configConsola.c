/*
 * configConsola.c
 *
 *  Created on: 6/4/2017
 *      Author: utnso
 */

#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <functions/othersFunctions.h>
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

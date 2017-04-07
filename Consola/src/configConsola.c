/*
 * configConsola.c
 *
 *  Created on: 6/4/2017
 *      Author: utnso
 */

#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include "structConsola.h"

t_config * Config = NULL;

t_regConfig leeArchConfigConsola() {

	char * path = "../archivoConfiguracionConsola.cfg";
	Config = config_create(path);
	t_regConfig* punteroConfig = malloc(sizeof(int) + sizeof(char*));
	t_regConfig configuracion = *punteroConfig;
	setIP(&configuracion.IP_KERNEL,"IP_KERNEL");
	setPUERTO(&configuracion.PUERTO_KERNELL, "PUERTO_KERNEL");
	config_destroy(Config);
	free(punteroConfig);

	return configuracion;
}

void setIP(char** ipSet, char* parametroBuscado) {
	if(config_has_property(Config, parametroBuscado)) {
		*ipSet = strdup(config_get_string_value(Config, parametroBuscado));
		printf("%s = %s \n", parametroBuscado, *ipSet);
	}
	else {
		printf("No se encontro %s \n", parametroBuscado);
	}
}

void setPUERTO(int* puertoSet, char* parametroBuscado) {
	if(config_has_property(Config, parametroBuscado)) {
		*puertoSet = config_get_string_value(Config, parametroBuscado);
		printf("%s = %s \n", parametroBuscado, *puertoSet);
	}
	else {
		printf("No se encontro %s \n", parametroBuscado);
	}
}

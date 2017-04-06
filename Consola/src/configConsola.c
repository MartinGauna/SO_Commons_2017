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

void setValorInt(int* valorSet, char* valorBuscado);
void setValorChar(char** valorSet, char* valorBuscado);

t_config * Config = NULL;
t_regConfig getConfigParams() {

	char * path = "../archivoConfiguracionConsola.cfg";
	Config = config_create(path);
	t_regConfig* punteroConfig = malloc(sizeof(int) + sizeof(char*));
	t_regConfig configuracion = *punteroConfig;
	setValorChar(&configuracion.IP_KERNEL,"IP_KERNEL");
	setValorInt(&configuracion.PUERTO_KERNELL, "PUERTO_KERNEL");
	config_destroy(Config);

	return configuracion;
}

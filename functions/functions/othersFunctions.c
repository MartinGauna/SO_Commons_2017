/*
 * othersFunctions.c
 *
 *  Created on: 11/4/2017
 *      Author: utnso
 */

#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include "othersFunctions.h"

t_config *Config = NULL;
char* path = NULL;

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

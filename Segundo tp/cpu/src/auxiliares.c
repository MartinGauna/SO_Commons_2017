#include "headers/auxiliares.h"

char* limpiarNombre(char* nombreALimpiar) {

	char* nombreLimpio = string_new();

	int32_t tamanio = 0;

	while (nombreALimpiar[tamanio] != '\0' && nombreALimpiar[tamanio] != '\n') {

		char* caracter = malloc(sizeof(char) * 2);
		caracter[0] = nombreALimpiar[tamanio];
		caracter[1] = '\0';

		string_append(&nombreLimpio, caracter);

		free(caracter);
		tamanio++;

	}

	nombreLimpio[tamanio] = '\0';

	return nombreLimpio;

}


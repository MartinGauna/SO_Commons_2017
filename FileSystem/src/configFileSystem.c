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
#include "string.h"


t_configFileSystem* leerConfigFS() {

	t_config* archivo = config_create("/home/utnso/workspace/Probanding/FileSystem/src/archivoConfiguracionFileSystem.cfg");
    t_configFileSystem* datosFS = malloc((sizeof(int) + sizeof(char*)));

	if(archivo == NULL) {
    	perror("Archivo no encontrado \n");
    }
    else {
    	printf("Datos de archivo de configuracion del File System: \n");

    	datosFS->PUERTO = config_get_int_value(archivo, "PUERTO");
    	printf("PUERTO = %d \n", datosFS->PUERTO);


		char* punto = string_new();
		string_append(&punto, config_get_string_value(archivo, "PUNTO_MONTAJE"));
		datosFS->PUNTO_MONTAJE = punto;
		printf("PUNTO_MONTAJE = %s \n \n", datosFS->PUNTO_MONTAJE);

		config_destroy(archivo);
    }

	return datosFS;
}


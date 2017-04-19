/*
 * configMemoria.c
 *
 *  Created on: 8/4/2017
 *      Author: utnso
 */

#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include "configMemoria.h"

t_configMemoria* leerConfigMemoria() {

	t_config* archivo = config_create("/home/utnso/workspace/Probanding/Memoria/src/archivoConfiguracionMemoria.cfg");
    t_configMemoria* datosMemoria = malloc((sizeof(int) + sizeof(char*)));

	if(archivo == NULL) {
    	perror("Archivo no encontrado \n");
    }
    else {
    	printf("Datos de archivo de configuracion de la Memoria: \n");

    	datosMemoria->PUERTO = config_get_int_value(archivo, "PUERTO");
    	printf("PUERTO = %d \n", datosMemoria->PUERTO);

    	datosMemoria->MARCOS = config_get_int_value(archivo, "MARCOS");
    	printf("MARCOS = %d \n", datosMemoria->MARCOS);

    	datosMemoria->MARCO_SIZE = config_get_int_value(archivo, "MARCO_SIZE");
    	printf("MARCO_SIZE = %d \n", datosMemoria->MARCO_SIZE);

    	datosMemoria->ENTRADAS_CACHE = config_get_int_value(archivo, "ENTRADAS_CACHE");
    	printf("ENTRADAS_CACHE = %d \n", datosMemoria->ENTRADAS_CACHE);

    	datosMemoria->CACHE_X_PROC = config_get_int_value(archivo, "CACHE_X_PROC");
    	printf("CACHE_X_PROC = %d \n", datosMemoria->CACHE_X_PROC);

		char* reecach = string_new();
		string_append(&reecach, config_get_string_value(archivo, "REEMPLAZO_CACHE"));
		datosMemoria->REEMPLAZO_CACHE = reecach;
		printf("IP Kernel = %s \n", datosMemoria->REEMPLAZO_CACHE);

		datosMemoria->RETARDO_MEMORIA = config_get_int_value(archivo, "RETARDO_MEMORIA");
		printf("RETARDO_MEMORIA = %d \n \n", datosMemoria->RETARDO_MEMORIA);

		config_destroy(archivo);
    }

	return datosMemoria;
}


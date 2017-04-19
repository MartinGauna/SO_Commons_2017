/*
 * configCPU.c
 *
 *  Created on: 19/4/2017
 *      Author: utnso
 */
#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include "configCPU.h"

t_configCPU* leerConfigCPU() {

	t_config* archivo = config_create("/home/utnso/workspace/Probanding/Consola/src/archivoConfiguracionConsola.cfg");
    t_configCPU* datosConsola = malloc((sizeof(int) + sizeof(char*)));


	if(archivo == NULL) {
    	perror("Archivo no encontrado \n");
    }
    else {
    	char* ip = string_new();
    	printf("Datos de archivo de configuracion de la Consola: \n");

    	datosConsola->PUERTO_KERNEL = config_get_int_value(archivo, "PUERTO_KERNEL");
    	printf("Puerto Kernel = %d \n", datosConsola->PUERTO_KERNEL);


		string_append(&ip, config_get_string_value(archivo, "IP_KERNEL"));
		datosConsola->IP_KERNEL = ip;
		printf("IP Kernel = %s \n", datosConsola->IP_KERNEL);

		config_destroy(archivo);
    }

	//PORQUE SI HAGO FREE(IP) BORRA LO QUE HAY DENTRO DE datosConsola->IP_KERNEL

	return datosConsola;
}


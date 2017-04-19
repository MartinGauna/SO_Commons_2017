/*
 * configConsola.c
 *
 *  Created on: 6/4/2017
 *      Author: utnso
 */

#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include "configConsola.h"

t_configConsola* leerConfigConsola() {

	t_config* archivo = config_create("/home/utnso/workspace/Probanding/Consola/src/archivoConfiguracionConsola.cfg");
    t_configConsola* datosConsola = malloc((sizeof(int) + sizeof(char*)));

	if(archivo == NULL) {
    	perror("Archivo no encontrado \n");
    }
    else {
    	printf("Datos de archivo de configuracion de la Consola: \n");

    	datosConsola->PUERTO_KERNEL = config_get_int_value(archivo, "PUERTO_KERNEL");
    	printf("Puerto Kernel = %d \n", datosConsola->PUERTO_KERNEL);


		char* ip = string_new();
		string_append(&ip, config_get_string_value(archivo, "IP_KERNEL"));
		datosConsola->IP_KERNEL = ip;
		printf("IP Kernel = %s \n \n", datosConsola->IP_KERNEL);

		config_destroy(archivo);
    }

	return datosConsola;
}

/*void setearUnInt(int datos, char* parametroBuscado, t_config* arch) {
	datos = config_get_int_value(arch, parametroBuscado);
    printf("Puerto Kernel = %d \n", datos);
}
Con esta funcion podriamos probar un if con config_has_property para saber si el parametro buscado
esta en el archivo y si no tira error. El problema se da cuando en la consola quiero mostrar los valores
me da un valor basura en vez de lo que hay dentro, seguro mal manejo de puteros*/

/*
 * configKernel.c
 *
 *  Created on: 8/4/2017
 *      Author: utnso
 */

#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/string.h>
#include <string.h>
#include "configKernel.h"

t_configKernel* leerConfigKernel() {

	t_config* archivo = config_create("/home/utnso/workspace/Probanding/Kernel/src/archivoConfiguracionKernel.cfg");
    t_configKernel* datosKernel = malloc((sizeof(int) + sizeof(char*)));

	if(archivo == NULL) {
    	perror("Archivo no encontrado \n");
    }
    else {
        printf("Datos de archivo de configuracion del Kernel: \n");

    	datosKernel->PUERTO_PROG = config_get_int_value(archivo, "PUERTO_PROG");
    	printf("PUERTO_PROG = %d \n", datosKernel->PUERTO_PROG);

    	datosKernel->PUERTO_CPU = config_get_int_value(archivo, "PUERTO_CPU");
    	printf("PUERTO_CPU = %d \n", datosKernel->PUERTO_CPU);

    	char* ipMem = string_new();
		string_append(&ipMem, config_get_string_value(archivo, "IP_MEMORIA"));
		datosKernel->IP_MEMORIA = ipMem;
		printf("IP_MEMORIA = %s \n", datosKernel->IP_MEMORIA);
		free(ipMem);

		datosKernel->PUERTO_MEMORIA = config_get_int_value(archivo, "PUERTO_MEMORIA");
		printf("PUERTO_MEMORIA = %d \n", datosKernel->PUERTO_MEMORIA);

		char* ipFs = string_new();
		string_append(&ipFs, config_get_string_value(archivo, "IP_FS"));
		datosKernel->IP_FS = ipFs;
		printf("IP_FS = %s \n", datosKernel->IP_FS);
		free(ipFs);

		datosKernel->PUERTO_FS = config_get_int_value(archivo, "PUERTO_FS");
		printf("PUERTO_FS = %d \n", datosKernel->PUERTO_FS);

		datosKernel->QUANTUM = config_get_int_value(archivo, "QUANTUM");
		printf("QUANTUM = %d \n", datosKernel->QUANTUM);

		datosKernel->QUANTUM_SLEEP = config_get_int_value(archivo, "QUANTUM_SLEEP");
	    printf("QUANTUM_SLEEP = %d \n", datosKernel->QUANTUM_SLEEP);

	    char* algoritmo = string_new();
	    string_append(&algoritmo, config_get_string_value(archivo, "ALGORITMO"));
	    datosKernel->ALGORITMO = algoritmo;
	    printf("ALGORITMO = %s \n", datosKernel->ALGORITMO);
	    free(algoritmo);

		datosKernel->GRADO_MULTIPROG = config_get_int_value(archivo, "GRADO_MULTIPROG");
	    printf("GRADO_MULTIPROG = %d \n", datosKernel->GRADO_MULTIPROG);

	    //setChar(&configuracion.SEM_IDS, "SEM_IDS");

	   /* char** array;
	    array = config_get_array_value(archivo, "SEM_IDS"); <-rompe aca
	    datosKernel->SEM_IDS = array;
	    printf("SEM_IDS = ");
	    int k = 0;
	    int flag = 1;
	    do {
	    	if(array[k] != NULL) {
	    		printf("%s ", array[k]);
	    		printf(" ,");
	    	}
	    	else {
	    		flag = 0;
	    		printf(" \n");
	    	}
	    } while (flag);*/
	    //setInt(&configuracion.SEM_INIT, "SEM_INIT");
	    //setChar(&configuracion.SHARED_VARS, "SHARED_VARS");

		datosKernel->STACK_SIZE = config_get_int_value(archivo, "STACK_SIZE");
	    printf("STACK_SIZE = %d \n \n", datosKernel->STACK_SIZE);

		config_destroy(archivo);
    }

	return datosKernel;
}

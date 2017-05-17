/*
 * funcionesKernel.c
 *
 *  Created on: 16/5/2017
 *      Author: utnso
 */

#include "funcionesKernel.h"

int nbytes;
t_package pack;
int recorrerfd;

void liberar_memoria(t_log* logger, configKernel* config) {
	free(logger);
	free(config);
}

void closeConections(int socketCPU, int socketFS, int socketMemoria, int socketConsola){
	close(socketCPU);
	close(socketFS);
	close(socketMemoria);
	close(socketConsola);
}

void printConfig(configKernel* conf){
	puts("Kernel.\n");
	printf("ALGORITMO: %s\n",conf->algoritmo);
	printf("GRADO MULTIPROGRAMACION: %d\n",conf->gradoMultiprog);
	printf("IP_FILESYSTEM: %s\n",conf->ipFS);
	printf("IP_MEMORIA: %s\n",conf->ipMemoria);
	printf("PUERTO_CPU: %d\n",conf->puertoCPU);
	printf("PUERTO_FS: %d\n",conf->puertoFS);
	printf("PUERTO_MEMORIA: %d\n",conf->puertoMemoria);
	printf("PUERTO_PROG: %d\n",conf->puerto_prog);
	printf("QUANTUM: %d\n",conf->quantum);
	printf("QUANTUM SLEEP: %d\n",conf->quantumSleep);
	printf("SEM IDS: %s\n",conf->semIds);
	printf("SEM INITS%s\n",conf->semInits);
	printf("SHARED_VARS: %s\n",conf->sharedVars);
	printf("STACK_SIZE: %d\n",conf->stackSize);
}

void* atenderCliente(void* _parameters){

	structParaCliente* estructura = (structParaCliente*) _parameters;

	t_log* testlog = log_create("log", "Kernel", 0, LOG_LEVEL_TRACE);
	int i = estructura->socketCliente;
	fd_set master = estructura->master;
	int sFS = estructura->socketFS;
	int sM = estructura->socketM;
	//FDSocket fdSocket[100];
    //fdSocket = estructura->fdSocket;
	int fdmax = estructura->fdmax;

	nbytes = recibir(i, &pack, testlog);

	if (nbytes  <= 0) {
		//Si la conexion se cerro
		if (nbytes == 0) {
			log_trace(testlog, "El socket %d se desconecto", i);
		} else {
			log_trace(testlog, "Error al recibir informacion del socket");
		}
		close(i);
		FD_CLR(i, &master); // eliminar del conjunto maestro
	}else {
		// tenemos datos de algun cliente
		// y aca hacer un swich que depende del header que lea es lo que vaya a hacer
		// entonces ejecuta el debido hilo //
		if (nbytes != 0){

			if(pack.code == HOLA){
				printf("La consola me manda un archivo! \n"); // toda esta pelotudez es para probar q los datos se enviar correctamente
				printf("el id es: %d \n",pack.code);
				printf("el tamaño es: %d \n",pack.size);
				printf("el mensaje es: %s \n",pack.data);

				log_info(testlog,"Imprimo msj: %s", pack.data);
			}

			if(pack.code == 17) {
				printf("La consola me manda mensajes! \n");
				printf("el id es: %d \n",pack.code);
				printf("el tamaño es es: %d \n",pack.size);
				printf("el mensaje es %s \n",pack.data);

				log_info(testlog,"Imprimo msj: %s", pack.data);

			}

			//Envia a Memoria
			if(enviar(sM, pack.code, pack.data, pack.size, testlog)) {
				printf("No se pudo enviar correctamente el stream \n");
				return EXIT_FAILURE;
			}

			//Envia a File System
			if(enviar(sFS, pack.code, pack.data, pack.size, testlog)) {
				printf("No se pudo enviar correctamente el stream \n");
				return EXIT_FAILURE;
			}

			//Envia a todas las CPU
			/*for(recorrerfd = 1; recorrerfd<fdmax+1; recorrerfd++) {
				if(strcmp(fdSocket[recorrerfd].nombre,"cpu")==0)
					if(enviar(fdSocket[recorrerfd].fd, pack.data, pack.size, testlog)) {
					printf("No se pudo enviar correctamente el stream \n");
					return EXIT_FAILURE;
				}
			}*/

		}
	}

	return 0;
}

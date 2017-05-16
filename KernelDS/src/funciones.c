/*
 * funciones.c
 *
 *  Created on: 30/4/2017
 *      Author: utnso
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/txt.h>
#include <features.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include "sockets.h"
#include "configKernel.h"
#include "funciones.h"

int nbytes;
int pack[2];
char package[PACKAGESIZE];
int recorrerfd;

void* consolaKernel(void* unused) {

	int enviar = 1;

    char message[PACKAGESIZE];

	while(enviar) {

		fgets(message, PACKAGESIZE, stdin);

    	if(strcmp(message, FINALIZAR_PROGRAMA) == 0) {
    		printf("Use comando finalizar programa \n");
    		enviar = -1;
        }
	}

	return NULL;
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

	nbytes =  recv(i, &pack[0], sizeof(int), 0);
	nbytes += recv(i, &pack[1], sizeof(int), 0);
	nbytes += recv(i, package, pack[1], 0);

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

			if(pack[0] == 1){
				printf("La consola me manda un archivo! \n");
				printf("el id es: %d \n",pack[0]);
				printf("el tamaño es: %d \n",pack[1]);
				printf("el mensaje es: %s \n",package);

				log_info(testlog,"Imprimo msj: %s", package);
			}

			if(pack[0] == 17) {
				printf("La consola me manda mensajes! \n");
				printf("el id es: %d \n",pack[0]);
				printf("el tamaño es es: %d \n",pack[1]);
				printf("el mensaje es %s \n",package);

				log_info(testlog,"Imprimo msj: %s", package);

			}

			//Envia a Memoria
			if(enviarStream(sM,pack[0],sizeof(int),&(pack[1])) == -1) {
				printf("No se pudo enviar correctamente el stream \n");
				exit(0);
			}
			send(sM, package, pack[1], 0);

			//Envia a File System
			if(enviarStream(sFS,pack[0],sizeof(int),&(pack[1])) == -1) {
				printf("No se pudo enviar correctamente el stream \n");
				exit(0);
			}
			send(sFS, package, pack[1], 0);

			//Envia a todas las CPU
			/*for(recorrerfd = 1; recorrerfd<fdmax+1; recorrerfd++) {
				if(strcmp(fdSocket[recorrerfd].nombre,"cpu")==0)
					send(fdSocket[recorrerfd].fd, package, strlen(package) + 1, 0);
			}*/

		}
	}

	return 0;
}

int* leerHeader(int socketARecibir){
	int* header;
	int bytesRecibidos = -1;
	int denuevo;
	do{
		denuevo = 0;
		header = malloc(sizeof(int));
		bytesRecibidos = recv(socketARecibir,header,sizeof(int),0);
		if(bytesRecibidos==-1){
			printf("ERROR\n");
			denuevo = 1;
			free(header);
		}
	}while(denuevo);
	if(bytesRecibidos<=0){
		*header = -1;
		return header;
	}
	else{
		return header;
	}
}

void* recibirStream(int socketDondeRecibe, int tamanioEstructuraARecibir) {

	int bytesRecibidos;
	void* recibido = malloc(tamanioEstructuraARecibir);
	void* tempRcv;
	int denuevo;
	do{
		denuevo = 0;
		tempRcv = malloc(tamanioEstructuraARecibir);
		bytesRecibidos = recv(socketDondeRecibe,tempRcv,tamanioEstructuraARecibir,0);
		if(bytesRecibidos==-1)
		{
			denuevo = 1;
			free(tempRcv);
		}
	}while(denuevo);
	//errno = 0;
	/*if(bytesRecibidos == -1){
		perror("error en recibir stream");
		exit(-1);
	}*/
	memcpy(recibido,tempRcv,tamanioEstructuraARecibir);
	if(bytesRecibidos<=0)
	{
		free(tempRcv);
		free(recibido);
		return NULL;
	}
	else
	{
		free(tempRcv);
		return recibido;
	}
}

int enviarStream(int socketDestino,int header, int tamanioMensaje, void* mensaje){

	if(send(socketDestino,&header,sizeof(int),0)==-1){
		printf("No se pudo enviar el header \n");
		return -1;
	}

	if(send(socketDestino,mensaje,tamanioMensaje,0)==-1){
		printf("No se pudo enviar el tamaño del mensaje \n");
		return -1;
	}
	return 0;
}

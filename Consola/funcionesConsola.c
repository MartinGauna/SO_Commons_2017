/*
 * funcionesConsola.c
 *
 *  Created on: 16/5/2017
 *      Author: utnso
 */

#include "configuracion.h"
#include "socket.h"
#include "funcionesConsola.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/txt.h>
#include <features.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <pthread.h>

t_archivoThread arch;
int* tamanio;

void handShakeCliente(int clienteSocket, char * servidor,const char * cliente){
	send(clienteSocket, (void *)cliente, sizeof(char)*11, 0);//mesnaje a kernel
		printf("envio %s %d\n",cliente,clienteSocket);
	recv(clienteSocket,servidor ,sizeof(char)*11, 0);
		printf("recibo  %s \n",servidor);
}

void liberar_memoria(t_log* logger,configConsole* config) {
	free(logger);
	free(config);
}


void consola_imprimir_encabezado(){
	printf("*********** BIENVENIDO A LA CONSOLA ***********\n");
	printf("Para consultar los comandos escriba 'help'");
	printf("\n");
}

void consola_imprimir_menu(){
	printf("Por favor seleccione la opcion correspondiente:\n");
	printf("\n");
	printf("1) iniciar programa\n");
	printf("2) finalizar programa\n");
	printf("3) desconectar consola\n");
	printf("4) limpiar mensajes\n");
	printf("5) enviar mensajes\n");
}

void* iniciarProgramaPorThread(void* _parameters) {

	int ch;
	char* buffer;
	int size;
	t_log* logger;

	FILE *archivoAnsisop;

	t_archivoThread* parameters = (t_archivoThread*) _parameters;

	parameters->path[strlen(parameters->path)-1] = '\0';

	archivoAnsisop = fopen(parameters->path,"r");//../ansisop/archivoPrueba.txt


	if (archivoAnsisop == NULL) {
		perror("Error al tratar de leer archivo");
		exit(EXIT_FAILURE);
	}

	fseek (archivoAnsisop , 0 , SEEK_END);
	size = ftell (archivoAnsisop);
	rewind (archivoAnsisop);

	buffer = malloc(size);

	ch = fgetc(archivoAnsisop);
	while ((ch) != EOF) {
		//Le paso la direccion de ch porque strcat recibe dos punteros char
		strcat(buffer, (char*) &ch);
		ch = fgetc(archivoAnsisop);
	}

	buffer = strcat(buffer,"\0");
	int tamanioBuffer = strlen(buffer)+1;
	int consola = CONSOLA;

	if(enviar(parameters->serverSocket, consola, buffer, tamanioBuffer, logger)){
		printf("No se pudo enviar correctamente el stream \n");
		return EXIT_FAILURE;
	}

	printf("Envio completado \n");

//	int* pid = recibirStream(serverSocket, sizeof(int));
//	printf("PID DEL PROCESO: %d\n",*pid);

	return NULL;
}

void iniciarPrograma(char* path, int serverSocket) {

	pthread_t arch_id;

	arch.serverSocket = serverSocket;
	arch.path = path;
	printf("El server es: %d  con el path %s\n", arch.serverSocket,arch.path);
	//esto es solo para ver que la info se pasa bien, despues se borra

	pthread_create(&arch_id, NULL, iniciarProgramaPorThread, &arch);

}

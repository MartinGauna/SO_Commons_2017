/*
 * funcionesConsola.c
 *
 *  Created on: 16/5/2017
 *      Author: utnso
 */

#include "funcionesConsola.h"


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
	printf("1) Iniciar programa (INICIAR) \n");
	printf("2) Finalizar programa (FIN) \n");
	printf("3) Desconectar consola (DESCONECTAR) \n");
	printf("4) Limpiar mensajes (LIMPIAR) \n");
	printf("5) Enviar mensajes (ENVIAR) \n");
}

void* iniciarProgramaPorThread(void* _parameters) {

	int ch;
	char* buffer;
	int size;

	t_archivoThread* parameters = _parameters;

	fseek (parameters->file , 0 , SEEK_END);
	size = ftell (parameters->file);
	rewind (parameters->file);

	buffer = malloc(size);

	ch = fgetc(parameters->file);
	while ((ch) != EOF) {
		//Le paso la direccion de ch porque strcat recibe dos punteros char
		strcat(buffer, (char*) &ch);
		ch = fgetc(parameters->file);
	}

	buffer = strcat(buffer,"\0");
	int tamanioBuffer = strlen(buffer)+1;

//	puts(buffer);

//	if(enviar(parameters->serverSocket, INICIAR_PROG, buffer, tamanioBuffer, logger)){
//		printf("No se pudo enviar correctamente el stream \n");
//	}

	printf("Envio completado \n");

//	int* pid = recibirStream(serverSocket, sizeof(int));
//	printf("PID DEL PROCESO: %d\n",*pid);

	return NULL;
}

void iniciarPrograma(FILE* archivo, int serverSocket) {

	pthread_t hilo;

	arch.serverSocket = serverSocket;
	arch.file = archivo;

	pthread_create(&hilo, NULL, iniciarProgramaPorThread, &arch);

}

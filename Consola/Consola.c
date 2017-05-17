
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <commons/log.h>
#include <commons/string.h>
#include <features.h>
#include <netinet/in.h>
#include <signal.h>
#include "../Commons/socket.h"
#include "../Commons/configuracion.h"
#include "funcionesConsola.h"

#define PACKAGESIZE 1024
#define PATH 300
#define INICIAR_PROGRAMA "iniciar programa\n"
#define FINALIZAR_PROGRAMA "finalizar programa\n"
#define DESCONECTAR_CONSOLA "desconectar consola\n"
#define LIMPIAR_MENSAJE "limpiar mensajes\n"
#define ENVIAR_MENSAJE "enviar mensajes\n"
#define HELP "help\n"

t_log *logger;

int main (int argc, char *argv[]) {
	t_log* logger = log_create("log_consola", "CONSOLA", 1, LOG_LEVEL_TRACE);
	configConsole* conf = (configConsole*) cargarConfiguracion("./config", 2, CONSOLA, logger);
	int socketKernel;

	char message[PACKAGESIZE];
	char path[PATH];
	int flag = 1;

	consola_imprimir_encabezado();
	printf("IP_KERNEL: %s\n",conf->ip);
	printf("PUERTO KERNEL: %d\n",conf->puerto);

	//Me conecto Al Kernel
	if(cargarSoket(conf->puerto, conf->ip, &socketKernel, logger)){
		//ERROR
	}
	//Hago el handshake con el Kernel.
	if(enviarHandshake(socketKernel, CONSOLA_HSK, KERNEL_HSK,logger)){
		//ERROR
	}

	while(flag){

	    	fgets(message, PACKAGESIZE, stdin);

	    	if(strcmp(message, DESCONECTAR_CONSOLA) == 0) {
	    		printf("Consola desconectandose... \n");
	    		flag = 0;
	    		close(socketKernel);
	    		exit(EXIT_FAILURE);
	    	}

	    	if(strcmp(message, INICIAR_PROGRAMA) == 0){
	    		printf("Inicio un programa AnSISOP \n");
	    		printf("Indique el path del programa que desea iniciar:  ");
	    		fgets(path, PATH, stdin);

	    		iniciarPrograma(path, socketKernel);
	    	}

	    	if(strcmp(message, FINALIZAR_PROGRAMA) == 0) {
	    		printf("Use comando finalizar programa \n");
	        }

	    	if(strcmp(message, LIMPIAR_MENSAJE) == 0) {
	    		printf("Limpio la consola de mensajes \n");
	    		//deberia pasar al sistema el comando clear, como?
	    	}

	    	if(strcmp(message, ENVIAR_MENSAJE) ==0) {
	    		printf("Escriba el mensaje:  ");
	    		fgets(message, 50, stdin);
	    		if(enviar(socketKernel,17,message,strlen(message)+1,logger)) { //el 17 significa una consola q solo envia mensajes
	    			printf("No se pudo enviar correctamente el stream \n");
	    			close(socketKernel);
	    		    return EXIT_FAILURE;
	    		}
	    	}

	    	if(strcmp(message, HELP) == 0) {
	    		consola_imprimir_menu();
	    	}

	    }

	liberar_memoria(logger, conf);
	return EXIT_SUCCESS;
}


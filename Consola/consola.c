#include "consola.h"

#include <ctype.h>

t_log* logger;
pthread_mutex_t lock;

int main(int argc, char *argv[]) {

	logger = log_create("log_consola", "CONSOLA", 1, LOG_LEVEL_TRACE);
	configConsole* conf = (configConsole*) cargarConfiguracion("./config", 2,
			CONSOLA, logger);
	int socketKernel;
	char handServer[10];

	char message[PACKAGESIZE];
	int flag = 1;

	printf("Configuracion:\n");
	printf("IP_KERNEL: %s\n", conf->ip);
	printf("PUERTO KERNEL: %d\n", conf->puerto);
	printf("\n");
	printf("\n");
	printf("\n");

	consola_imprimir_encabezado();
//	//Me conecto Al Kernel
//	if(cargarSoket(conf->puerto, conf->ip, &socketKernel, logger)){
//		//ERROR
//	}
//
//	//Hago el handshake con el Kernel.
//	handShakeCliente(socketKernel,handServer,"consola");
//	/*if(enviarHandshake(socketKernel, CONSOLA_HSK, KERNEL_HSK,logger)){
//		//ERROR
//	}*/

	while (flag) {

		fgets(message, PACKAGESIZE, stdin);

		if (strcmp(message, DESCONECTAR_CONSOLA) == 0) {
			printf("Consola desconectandose... \n");
			flag = 0;
			close(socketKernel);
		}

		if (strcmp(message, INICIAR_PROGRAMA) == 0) {
			printf("Inicio un programa AnSISOP \n");
			printf("Indique el path del programa que desea iniciar:  ");

			char* path = malloc(4096);
			scanf("%s", path);

			FILE* fp = fopen(path,"r");

			free(path);

				if (fp != NULL) {
					iniciarPrograma(fp,socketKernel);
				} else {
					puts("La ruta ingresada es invalida\n");
				}
		}

		if (strcmp(message, FINALIZAR_PROGRAMA) == 0) {
			printf("Use comando finalizar programa \n");
		}

		if (strcmp(message, LIMPIAR_MENSAJE) == 0) {
			printf("Limpio la consola de mensajes \n");
			system("clear");
		}

		if (strcmp(message, ENVIAR_MENSAJE) == 0) {
			printf("Escriba el mensaje:  ");
			fgets(message, 50, stdin);
			if (enviar(socketKernel, 17, message, sizeof(50), logger)) { //el 17 significa una consola q solo envia mensajes
				printf("No se pudo enviar correctamente el stream \n"); // no deberia preguntar si enviar == -1 ??
			}
		}

		if (strcmp(message, HELP) == 0) {
			consola_imprimir_menu();
		}
	}

	liberar_memoria(logger, conf);
	close(socketKernel);
	return EXIT_SUCCESS;
}


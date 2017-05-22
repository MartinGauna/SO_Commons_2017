#include "consola.h"

#include <ctype.h>

#define TAMANIO_ARCHIVO 4096

t_log* logger;
pthread_mutex_t lock;

int main(int argc, char *argv[]) {

	logger = log_create("log_consola", "CONSOLA", 1, LOG_LEVEL_TRACE);
	configConsole* conf = (configConsole*) cargarConfiguracion("./config", 2,
			CONSOLA, logger);
	int socketKernel;

	char message[PACKAGESIZE];
	int ejecutar = 1;

	printf("Configuracion:\n");
	printf("IP_KERNEL: %s\n", conf->ip);
	printf("PUERTO KERNEL: %d\n", conf->puerto);
	printf("\n\n\n");

	consola_imprimir_encabezado();
	//Me conecto Al Kernel
	if (cargarSoket(conf->puerto, conf->ip, &socketKernel, logger)) {
		//ERROR
	}

	//Hago el handshake con el Kernel.
	if(enviarHandshake(socketKernel, CONSOLA_HSK, KERNEL_HSK,logger)){
		//ERROR
	}

	while (ejecutar) {

		fgets(message, PACKAGESIZE, stdin);

		if (strcmp(message, DESCONECTAR_CONSOLA) == 0) {
			log_info(logger, "Consola desconectandose...");
			ejecutar = 0;
		}

		if (strcmp(message, INICIAR_PROGRAMA) == 0) {
//			log_info(logger, "Inicio un programa AnSISOP");
//			log_info(logger, "Indique el path del programa que desea iniciar:");
			printf("Inicio un programa AnSISOP \n");
			printf("Indique el path del programa que desea iniciar:  ");

			char* path = malloc(TAMANIO_ARCHIVO);
			scanf("%s", path);
			FILE* fp = fopen(path, "r");
			free(path);

			if (fp != NULL) {
				iniciarPrograma(fp, socketKernel);
			} else {
//				log_warning(logger, "La ruta ingresada es invalida:");
				puts("La ruta ingresada es invalida\n");
			}
		}

		if (strcmp(message, FINALIZAR_PROGRAMA) == 0) {
			printf("Use comando finalizar programa.\n");
		}

		if (strcmp(message, LIMPIAR_MENSAJE) == 0) {
			printf("Limpio la consola de mensajes.\n");
			system("clear");
		}

		if (strcmp(message, ENVIAR_MENSAJE) == 0) {
			printf("Escriba el mensaje:\t");
			fgets(message, 50, stdin);
			if (enviar(socketKernel, CONSOLA_HSK, message, sizeof(50), logger)) {
				log_error(logger, "No se pudo enviar correctamente el stream ");
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


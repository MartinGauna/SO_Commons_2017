
#include "funcionesFileSystem.h"

structParaKernel kernel;

int main (int argc, char *argv[]) {
	t_log* logger = log_create("log_kernel", "FILESYSTEM", 1, LOG_LEVEL_TRACE);
	configFileSystem* conf = (configFileSystem*) cargarConfiguracion("./config", 2, FILESYSTEM, logger);
	int socketListen, socketKernel, flag = 1;
	uint16_t codigoHandshake;
	t_package pkg;

	puts("FileSystem.");
	printf("PUERTO: %d\n",conf->puerto);
	printf("PUNTO MONTAJE: %s\n",conf->puntoMontaje);

	if(escuchar(conf->puerto, &socketListen, logger)){
		//ERROR
	}
	if(aceptar(socketListen, &socketKernel, logger)){
		//ERROR
	}
	if(recibirHandshake(socketKernel, FILESYSTEM_HSK, &codigoHandshake, logger)){
		//ERROR
	}
	if(codigoHandshake != KERNEL_HSK){//Checkear que sea el kernel el proceso al que me conecto
		log_error(logger, "Codigo incorrecto de Handshake.");
		return EXIT_FAILURE;
	}

	//	while(flag){
	//		if(aceptar(socketListen, &socketKernel, logger)){
	//			//ERROR
	//		}
	//		if(recibirHandshake(socketKernel, FILESYSTEM_HSK, &codigoHandshake, logger)){
	//			//ERROR
	//		}
	//		if(codigoHandshake == KERNEL_HSK){//Checkear que sea el kernel el proceso al que me conecto
	//			flag = 0;
	//		}
	//	}


	while(1){
		printf("Esperando mensaje del Kernel.\n");
		if(recibir(socketKernel, &pkg, logger)){
			//ERROR
			close(socketKernel);
			return EXIT_FAILURE;
		}
		printf("Mensaje recibido del kernels: %s\n",pkg.data);
		free(pkg.data);
	}

	pthread_t thread_cliente;
	kernel.socket = socketKernel;
	kernel.log = logger;
	int mientras = 1;

	while(mientras) {
		pthread_create(&thread_cliente, NULL, &atenderKernel, &kernel);

	}

	close(socketKernel);

	liberar_memoria(logger, conf);
	return EXIT_SUCCESS;
}


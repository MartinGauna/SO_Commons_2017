
#include "funcionesKernel.h"


int main (int argc, char *argv[]) {
	t_log* logger = log_create("log_kernel", "KERNEL", 1, LOG_LEVEL_TRACE);
	configKernel* conf = (configKernel*)cargarConfiguracion("./config", 14, KERNEL, logger);
	int socketMemoria,socketFS,socketListen,newSocket,socketCPU=0,socketConsola=0;
	int conectados, fd;
	uint16_t codigoHandshake;
	t_package pkg;
	fd_set readset;
	FD_ZERO(&readset);

	//Imprimo la configuracion
	printConfig(conf);


	//Me conecto con la Memoria
	if(cargarSoket(conf->puertoMemoria, conf->ipMemoria, &socketMemoria, logger)){
		//ERROR
		return EXIT_FAILURE;
	}
	if(enviarHandshake(socketMemoria, KERNEL_HSK, MEMORIA_HSK,logger)){
		//ERROR
		return EXIT_FAILURE;
	}
	log_debug(logger, "Conectado con la memoria.");

	//Meconecto con el FS
	if(cargarSoket(conf->puertoFS, conf->ipFS, &socketFS, logger)){
		//ERROR
		return EXIT_FAILURE;
	}
	if(enviarHandshake(socketFS, KERNEL_HSK, FILESYSTEM_HSK,logger)){
		//ERROR
		return EXIT_FAILURE;
	}
	log_debug(logger, "Conectado con el FileSystem.");

//	FD_SET(socketMemoria, &readset);
//	FD_SET(socketFS, &readset);

	//Me pongo a escuchar conecciones de consolas y CPUs
	if(escuchar(conf->puerto_prog, &socketListen, logger)){
		//ERROR
		return EXIT_FAILURE;
	}

	for(conectados=0;conectados<2;conectados++){

		if(aceptar(socketListen, &newSocket, logger)){
			//ERROR
			return EXIT_FAILURE;
		}
		if(recibirHandshake(newSocket, KERNEL_HSK, &codigoHandshake, logger)){
			//ERROR
			return EXIT_FAILURE;
		}
		if(!socketConsola && codigoHandshake == CONSOLA_HSK){
			socketConsola = newSocket;
			FD_SET(socketConsola, &readset);
		}else if(!socketCPU && codigoHandshake == CPU_HSK){
			socketCPU = newSocket;
			FD_SET(socketCPU, &readset);
		}else{
			close(newSocket); //No se reconoce el codigo del Handshake.
		}
	}
	FD_SET(socketListen, &readset);

	while(1){

		fd = select();

		if(recibir(socketConsola, &pkg, logger)){
			//ERROR
			closeConections(socketCPU, socketFS, socketMemoria, socketConsola);
			return EXIT_FAILURE;
		}
		printf("Mensaje recibido de la consola: %s\n",pkg.data);

//		log_debug(logger, "se envia el mensaje a la CPU");
//		if(enviar(socketCPU, HOLA, pkg.data, pkg.size, logger)){
//			//ERROR
//			closeConections(socketCPU, socketFS, socketMemoria, socketConsola);
//			return EXIT_FAILURE;
//		}
//		log_debug(logger, "se envia el mensaje al File System");
//		if(enviar(socketFS, HOLA, pkg.data, pkg.size, logger)){
//			//ERROR
//			closeConections(socketCPU, socketFS, socketMemoria, socketConsola);
//			return EXIT_FAILURE;
//		}
//		log_debug(logger, "se envia el mensaje a la Memoria");
//		if(enviar(socketMemoria, HOLA, pkg.data, pkg.size, logger)){
//			//ERROR
//			closeConections(socketCPU, socketFS, socketMemoria, socketConsola);
//			return EXIT_FAILURE;
//		}
//		free(pkg.data);


	}
	printf("Ingrese una tecla para finalizar.\n");
	getchar();
	return EXIT_SUCCESS;
}

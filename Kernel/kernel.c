//
// Created by Martin Gauna on 4/2/17.
//

#include "kernel.h"

#define STDIN 0

t_log* logger;
t_dictionary* procesos;

int main (int argc, char *argv[]) {
	t_log* logger = log_create("log_kernel", "KERNEL", 1, LOG_LEVEL_TRACE);
	configKernel* conf = (configKernel*)cargarConfiguracion("./config", 14, KERNEL, logger);
	int //socketMemoria,
		//socketFS,
		socketListen,
		newSocket,
		socketCPU=0,
		//socketConsola=0,
		*pSocket,
		pidCount = 1;
	int //conectados,
		fd, nfd=0, terminar=0 ;
//	uint16_t codigoHandshake;
	//t_package pkg;
	fd_set readSet, masterSet;
	struct timeval timeOut;
	timeOut.tv_sec = 10;
	timeOut.tv_usec = 500000;



	t_dictionary* consolas =  dictionary_create();
	t_dictionary* cpus =  dictionary_create();
	char* key;
	t_proceso* proceso;



	FD_ZERO(&masterSet);
	FD_SET(STDIN, &masterSet);
	//Imprimo la configuracion
	printConfig(conf);

/*
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

//	FD_SET(socketMemoria, &readSet);
	nfd = highestFD(socketMemoria,nfd);
//	FD_SET(socketFS, &readSet);
	nfd = highestFD(socketFS,nfd);
 */

	//Me pongo a escuchar conecciones de consolas y CPUs
	if(escuchar(conf->puerto_prog, &socketListen, logger)){
		//ERROR
		return EXIT_FAILURE;
	}

//	for(conectados=0;conectados<1;conectados++){
//
//		if(aceptar(socketListen, &newSocket, logger)){
//			//ERROR
//			return EXIT_FAILURE;
//		}
//		if(recibirHandshake(newSocket, KERNEL_HSK, &codigoHandshake, logger)){
//			//ERROR
//			return EXIT_FAILURE;
//		}
//		if(!socketConsola && codigoHandshake == CONSOLA_HSK){
//			socketConsola = newSocket;
//			FD_SET(socketConsola, &readSet);
//			nfd = highestFD(socketConsola,nfd);
//		}else if(!socketCPU && codigoHandshake == CPU_HSK){
//			socketCPU = newSocket;
//			FD_SET(socketCPU, &readSet);
//			nfd = highestFD(socketCPU,nfd);
//		}else{
//			close(newSocket); //No se reconoce el codigo del Handshake.
//		}
//	}
	FD_SET(socketListen, &masterSet);
	nfd = highestFD(socketCPU,nfd);
	while(!terminar){
		readSet = masterSet;
		fd = select(nfd, &readSet, NULL, NULL, &timeOut);

		if(fd < 0){
			log_error(logger, "Error en el select: %s", strerror(errno));
			terminar = 1;

		} else if (fd > 0 ){
			//Si el socket
			if(FD_ISSET(socketListen, &readSet)){
				newSocket = aceptarConexion(socketListen, &masterSet, &nfd, cpus, consolas, &pidCount, logger);
				if(newSocket < 0) {
					//TODO ERROR - chequear que tenemos que hacer en este caso. Crear un nuevo socket para listen talvez?
					//terminar = 1;
					//log_error(logger, "Error en el accept: %s", strerror(errno));
				} else {
//					key = string_itoa(newSocket);
//					proceso = dictionary_get(consolas, key);
//					if(pSocket != NULL) {
//						//es un socket de consola
//						//atenderConsola(proceso,pidCount, logger);
//					} else {
//						pSocket = dictionary_get(cpus, key);
//						if(pSocket != NULL) {
//							//Es un socket CPU
//						}
//					}
				}

			}
			else if(FD_ISSET(STDIN, &readSet)) {
				char c;
				while((c=getchar()) != EOF){
					putchar(c);
				}
			}
		}


//		if(recibir(socketConsola, &pkg, logger)){
//			//ERROR
//			closeConections(socketCPU, socketFS, socketMemoria, socketConsola);
//			return EXIT_FAILURE;
//		}
//		printf("Mensaje recibido de la consola: %s\n",pkg.data);

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
	dictionary_destroy_and_destroy_elements(cpus, free);
//	dictionary_destroy_and_destroy_elements(consolas, free);
	return EXIT_SUCCESS;
}

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

/**
 * Acepta conexiones de las consolas y las cpus
 *
 */
int aceptarConexion(int socketListen, fd_set* masterSet, int* nfd, t_dictionary* cpus, t_dictionary* consolas, int* pidCount, t_log* logger){

	int newSocket;
	char* key;
	uint16_t codigoHandshake;
//	t_proceso* proc;

	if(aceptar(socketListen, &newSocket, logger)){
		//ERROR
		return -1;
	}
	if(recibirHandshake(newSocket, KERNEL_HSK, &codigoHandshake, logger)){
		//ERROR
		return -1;
	}

	//Se conecta una consola
	if(codigoHandshake == CONSOLA_HSK){

		atenderConsola(newSocket, pidCount);
		dictionary_put(consolas, key, newSocket);
	}else if(codigoHandshake == CPU_HSK){
//		FD_SET(*newSocket, readSet);
//		highestFD(*newSocket, *nfd);
//		dictionary_put(cpus, key, newSocket);
	}else{ //No se reconoce el codigo del Handshake.
		log_warning(logger, "Se intento conectar un proceso no reconocido.");
		close(newSocket);
		return -1;
	}
	return newSocket;
}

int atenderConsola(int socket, int* pidCount){
	t_package pkg;
	t_proceso* proc;

	if(recibir(socket, &pkg, logger)){
		//ERROR
		return EXIT_FAILURE;
	}
	if(pkg.size != 0 && pkg.data != NULL){
		//Proceso Nuevo a Ejecutar
		if(pkg.code == INICIAR_PROG){
			proc = (t_proceso*)malloc(sizeof(t_proceso));
			proc->pid = crearPidSock(pidCount);
			proc->ansisop = pkg.data;
		}
		else if (pkg.code == FINALIZAR_PROG){
			pkg.data;
			log_debug(logger, "Se pide finalizar el siguiente proceso: %s", pkg.data);
			dictionary_get(procesos, pkg.data);
		}
	}
	return EXIT_SUCCESS;
}

//t_proceso* crearProceso(int socket, int* pidCount, t_log* logger){
//	t_package pkg;
//	t_proceso * proc = NULL;
//	proc->socket = socket;
//	proc->pid = crearPidSock(pidCount);
//
//	//Espero recibir el codigo del nuevo proceso.
//	if(recibir(socket, &pkg, logger)){
//		//ERROR
//		return NULL;
//	}
//	if(pkg.code == INICIAR_PROG){
//
//		if(pkg.size != 0 && pkg.data != NULL){
//			proc = (t_proceso *)malloc(sizeof(t_proceso));
//			proc->socket = socket;
//			proc->pid = crearPidSock(pidCount);
//			proc->ansisop = pkg.data;
//		}
//	}
//
//	return proc;
//}

int crearPidSock(int* pidCount){
	int res = *pidCount;
	//TODO Agregar checkeo para max int value o es inecesario?
	*pidCount = (*pidCount) + 1;
	return res;
}

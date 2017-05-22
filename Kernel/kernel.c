//
// Created by Martin Gauna on 4/2/17.
//

#include "kernel.h"

t_log* logger;
t_list* newQueue;
t_queue* readyQueue;
t_queue* finishQueue;

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


	newQueue = list_create();
	readyQueue = queue_create();
	finishQueue  = queue_create();

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
				atenderConsolaKernel();
			}
		}
	}
	dictionary_destroy(consolas);
	dictionary_destroy(cpus);
	printf("Ingrese una tecla para finalizar.\n");
	getchar();
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

	//Los mensajes de la consola se atienden inmediatamente porque lo que se tiene que hacer es algo rapido siempre.
	if(recibir(socket, &pkg, logger)){
		//ERROR
		return EXIT_FAILURE;
	}
	if(pkg.code == INICIAR_PROG){
		proc = crearProceso(socket, pidCount, pkg.data);
		list_add(newQueue, proc);
		//TODO Falta el cheque de memoria para ver si se puede agregar el proceso y ver el nivel de multiprog para ver a que lista agregarlo
		char* pid;
//		char pid[PID_STR_COUNT];
//		snprintf(pid, PID_STR_COUNT, "%d", proc->pcb.pid);
//		itoa(proc->pcb.pid, pid, 10);
		pid = string_itoa(proc->pcb.pid);
		log_debug(logger, "Nuevo procesocreado. PID:%d",proc->pcb.pid);
		enviar(socket, ACEPTAR_PROG, pid, strlen(pid), logger);
		free(pid);
	}
	else if (pkg.code == FINALIZAR_PROG){
		//TODO Buscar el proceso en todas las listas(new, ready)
	}
	else if (pkg.code == MATAR_PROCESOS){
		//TODO Matar todos los procesos de una consola
	}
	else{
		log_warning(logger,"Codigo recibido por una Consola no reconocido.Codigo: %d", pkg.code);
	}

	return EXIT_SUCCESS;
}

t_proceso* crearProceso(int socket, int* pidCount, char* ansisop) {
	t_proceso* proc = (t_proceso*)malloc(sizeof(t_proceso));

	//TODO Crear un t_pid que tenga sentido
	proc->consolaSocket = socket;
	proc->pcb.pid = crearPidSock(pidCount);
	proc->pcb.ansisop = ansisop;

	return proc;
}

int crearPidSock(int* pidCount){
	int res = *pidCount;
	//TODO Agregar checkeo para max int value o es inecesario?
	*pidCount = (*pidCount) + 1;
	return res;
}

int atenderConsolaKernel(){
	char c;
	while((c=getchar()) != EOF){
		putchar(c);
	}
	putchar('\n');
	return EXIT_SUCCESS;
}

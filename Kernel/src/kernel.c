
#include "funcionesKernel.h"

structParaCliente cliente;

int main (int argc, char *argv[]) {
	t_log* logger = log_create("log_kernel", "KERNEL", 1, LOG_LEVEL_TRACE);
	configKernel* conf = (configKernel*)cargarConfiguracion("./config", 14, KERNEL, logger);
	int socketMemoria,socketFS,socketListen,newSocket,socketCPU=0,socketConsola=0;
	int conectados, fd;
	uint16_t codigoHandshake;
	t_package pkg;
	pthread_t thread_cliente;
	FDSocket fdSocket[100];
	printf("Se crea el Kernel\n");

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

	//Inicializo Select
	log_info(logger, "Inicializo el SELECT");
	fd_set readset;
	fd_set read_fds;
	int fdmax;
	int newfd;
	int i;
	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);

	FD_ZERO(&readset);
	FD_ZERO(&read_fds);
	FD_SET(socketListen, &readset);
	FD_SET(socketMemoria, &readset);
	FD_SET(socketFS, &readset);
	fdmax = socketListen;

	//Me pongo a escuchar conecciones de consolas y CPUs
	if(escuchar(conf->puerto_prog, &socketListen, logger)){
		//ERROR
		return EXIT_FAILURE;
	}

	//Me mantengo en el bucle para asi poder procesar cambios en los sockets
	while(1) {
		//Copio los sockets y me fijo si alguno tiene cambios, si no hay itero de vuelta
		read_fds = readset; // copialo
		if (select(fdmax+1, &read_fds, NULL, NULL, NULL)<0) {
			if (errno != EINTR){
				perror("select error");
				return 1;
			}
		}else{
			//Recorro los sockets con cambios
			for(i = 0; i <= fdmax; i++) {
				//Detecto si hay datos en un socket especifico
				if (FD_ISSET(i, &read_fds)) {
					//Si es el socket de escucha proceso el nuevo socket
					if (i == socketListen) {
						addrlen = sizeof(addr);
						if ((newfd = accept(socketListen, (struct sockaddr*)&addr, &addrlen)) == -1){
							log_info(logger,"Ocurrio error al aceptar una conexion");
						} else {
							FD_SET(newfd, &readset); // Agrego el nuevo socket al  select
							log_info(logger,"Agrego un socket");
							//Actualizo la cantidad
							if (newfd > fdmax) {
								fdmax = newfd;
							}

							log_trace(logger, "Nueva conexion de %s en  el socket %d", inet_ntoa(addr.sin_addr),newfd);

						}
					} else {

						//Recibe consolas y cpus
						if(fdSocket[i].activo == 0){

							//for(conectados=0;conectados<2;conectados++)
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

//******* Aca hay un tema importante para resolver, si tenemos socketCPU y con el for me restringe a que haya solo una CPU conectada y no varias
//******* Pero usar el vector es de negros, asi que hay que arreglar esto, mas que nada para despues armar bien la estructura que se le pasa al thread
//******* Y poder solucionar el tema de el envio d msj a todas las CPU que esta comentado dentro del thread

							fdSocket[i].activo = TRUE;
							fdSocket[i].fd = i;
							strcpy(fdSocket[i].nombre,codigoHandshake);
							printf("%s conectado. Esperando mensajes: \n", codigoHandshake);

						}else{
						//Si es un socket existente

							cliente.socketCliente = i;
							cliente.master = readset;
							cliente.socketFS = socketFS;
							cliente.socketM = socketMemoria;
							//cliente.fdSocket[100]= fdSocket[100];
							cliente.fdmax = fdmax;

							pthread_create(&thread_cliente, NULL, (void*)atenderCliente, &cliente);

							}

						}
					}
				  }
				}
			}

	printf("Ingrese una tecla para finalizar.\n");
	getchar();
	return EXIT_SUCCESS;
}

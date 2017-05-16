
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
#include "sockets.h"
#include "configKernel.h"
#include "funciones.h"


#define PACKAGESIZE 1024

structParaCliente cliente;

int main(){

	remove("log");

	t_log* testlog = log_create("log", "Kernel", 0, LOG_LEVEL_TRACE);


	if (testlog == NULL) {
		puts("No se pudo generar el archivo de logueo.\n");
		return (-1);
	};

	int listenningSocket;
	int socketMemoria;
	int socketFileSystem;
	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);
	FDSocket fdSocket[100];
	char handServer[10];
	char handCliente[10];
//	create_log(testlog);
	printf("Se crea el Kernel\n");

	t_configKernel* kernel = leerConfigKernel();
	char *IP_MEMORIA = kernel->IP_MEMORIA;
	char* PUERTO_MEMORIA = string_itoa(kernel->PUERTO_MEMORIA);
	char* PUERTO_PROG = string_itoa(kernel->PUERTO_PROG);
	char* PUERTO_FILESYSTEM = string_itoa(kernel->PUERTO_FS);
	char *IP_FILESYSTEM = kernel->IP_FS;

	create_server(&listenningSocket, PUERTO_PROG);

	create_client(&socketMemoria , IP_MEMORIA, PUERTO_MEMORIA);
	handShakeCliente(socketMemoria,handServer,"kernel");

	create_client(&socketFileSystem, IP_FILESYSTEM, PUERTO_FILESYSTEM);
	handShakeCliente(socketFileSystem,handServer,"kernel");

	pthread_t thread_cliente;

	//pthread_t consolaKernel;
	//pthread_create(&consolaKernel, NULL, &consolaKernel, NULL); SEGMENTATION FAULT O.o

	//Inicializo el select
			log_info(testlog,"Inicializo el SELECT");
			fd_set master;		// conjunto maestro de descriptores de fichero
			fd_set read_fds;	// conjunto temporal de descriptores de fichero para select()
			int fdmax;			// numero maximo de descriptores de fichero
			int newfd;			// descriptor de socket de nueva conexion aceptada
			int i;

			FD_ZERO(&master);					// borra los conjuntos maestro y temporal
			FD_ZERO(&read_fds);
			FD_SET(listenningSocket, &master);	// agregar listener al conjunto maestro
			fdmax = listenningSocket; 			// seguir la pista del descriptor de fichero mayor, por ahora es este

			//Me mantengo en el bucle para asi poder procesar cambios en los sockets
			while(1) {
				//Copio los sockets y me fijo si alguno tiene cambios, si no hay itero de vuelta
				read_fds = master; // copialo
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
							if (i == listenningSocket) {
								addrlen = sizeof(addr);
								if ((newfd = accept(listenningSocket, (struct sockaddr*)&addr, &addrlen)) == -1){
									log_info(testlog,"Ocurrio error al aceptar una conexion");
								} else {
									FD_SET(newfd, &master); // Agrego el nuevo socket al  select
									log_info(testlog,"Agrego un socket");
									//Actualizo la cantidad
									if (newfd > fdmax) {
										fdmax = newfd;
									}

									log_trace(testlog, "Nueva conexion de %s en  el socket %d", inet_ntoa(addr.sin_addr),newfd);

								}
							} else {
								//Recibe consolas y cpus
								if(fdSocket[i].activo == 0){

									handShakeServidor(i,"kernel",handCliente);
									fdSocket[i].activo = TRUE;
									fdSocket[i].fd = i;
									strcpy(fdSocket[i].nombre,handCliente);
									printf("%s conectado. Esperando mensajes: \n",handCliente);

								}else{
								//Si es un socket existente

									cliente.socketCliente = i;
									cliente.master = master;
									cliente.socketFS = socketFileSystem;
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
				//
			//
	close(socketMemoria);
	log_info(testlog, "cierro socketCliente");
	close(socketFileSystem);
	log_info(testlog, "cierro socketFileSystem");
	close(listenningSocket);
	log_info(testlog, "cierro socketServer");

	return 0;
}

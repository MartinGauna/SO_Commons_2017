/*
 ============================================================================
 Name        : Kernel.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <commons/log.h>
#include <features.h>
#include <netinet/in.h>
#include "sockets.h"
#include "configKernel.h"

#define PUERTO "5010"
#define PUERTO_MEMORIA "5100"
#define IP_MEMORIA "127.0.0.1"

#define PUERTO_FILESYSTEM "5003"
#define IP_FILESYSTEM "127.0.0.1"

#define PACKAGESIZE 1024


int main(){


	remove("log");

	t_log* testlog = log_create("log", "Kernel", 0, LOG_LEVEL_TRACE);;


	if (testlog == NULL) {
		puts("No se pudo generar el archivo de logueo.\n");
		return (-1);
	};

	int listenningSocket;
	int socketMemoria;
	int socketFileSystem;
	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);

//	create_log(testlog);
	printf("Se crea el Kernel\n");

	t_configKernel* kernel = leerConfigKernel(kernel);
	printf("Anda? puerto cpu es 5001? Devuelve : %d \n \n", kernel->PUERTO_CPU);

	create_server(&listenningSocket, PUERTO);

	create_client(&socketMemoria , IP_MEMORIA, PUERTO_MEMORIA);
	handShakeCliente(socketMemoria,"memoria","kernel");
	create_client(&socketFileSystem, IP_FILESYSTEM, PUERTO_FILESYSTEM);
	handShakeCliente(socketFileSystem,"filesystem","kernel");

	//Inicializo el select
			log_info(testlog,"Inicializo el SELECT");
			fd_set master;		// conjunto maestro de descriptores de fichero
			fd_set read_fds;	// conjunto temporal de descriptores de fichero para select()
			int fdmax;			// numero maximo de descriptores de fichero
			int newfd;			// descriptor de socket de nueva conexion aceptada
			int i;
			int nbytes;
			char package[PACKAGESIZE];

			FD_ZERO(&master);					// borra los conjuntos maestro y temporal
			FD_ZERO(&read_fds);
			FD_SET(listenningSocket, &master);	// agregar listener al conjunto maestro
			fdmax = listenningSocket; 			// seguir la pista del descriptor de fichero mayor, por ahora es este

			//Me mantengo en el bucle para asi poder procesar cambios en los sockets
			while(1) {
				//Copio los sockets y me fijo si alguno tiene cambios, si no hay itinero de vuelta
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
								//Si es un socket existente
								if ((nbytes = recv(i, &package, PACKAGESIZE, 0)) <= 0) {
									//Si la conexion se cerro
									if (nbytes == 0) {
										log_trace(testlog, "El socket %d se desconecto", i);
									} else {
										log_trace(testlog, "Error al recibir informacion del socket");
									}
									close(i);
									FD_CLR(i, &master); // eliminar del conjunto maestro
								} else {
									// tenemos datos de algÃºn cliente
									if (nbytes != 0){
										log_info(testlog,"Imprimo msj: %s", package);
										printf("Nuevo msj:\n");
										printf("%s", package);
										send(socketMemoria, package, strlen(package) + 1, 0);
										send(socketFileSystem, package, strlen(package) + 1, 0);
									}

								}
							}
						}
					}
				}
			}

	close(socketMemoria);
	log_info(testlog, "cierro socketCliente");
	close(socketFileSystem);
	log_info(testlog, "cierro socketFileSystem");
	close(listenningSocket);
	log_info(testlog, "cierro socketServer");

	return 0;
}

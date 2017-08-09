#include "networkHandler.h"

/*
 * Esta funcion se encarga de que se envien TODOS los bytes requeridos
 * Return -1 si no se pudo enviar
 * Return -2 si hubo algun problema en la recursion
 * Return >0  si se enviaron todos los bytes (devuelve la cantidad)
 * Return 0 conexion cerrada
 * En caso de error guardar en el archivo de logs de la biblioteca
 */
int librocket_enviarMensaje(int sockfd, const void * datos, size_t bytesAenviar,
		t_log* t_log) {

	int bytes_enviados = send(sockfd, datos, bytesAenviar, 0);

	if (bytes_enviados == -1) { //ERROR
		perror("send:");
		log_info(t_log, "No se pudieron enviar datos");
		return -1;
	} else if (bytes_enviados == 0) { //conexion cerrada
		char* mensaje = string_new();
		string_append(&mensaje, "Se desconecto el cliente con el socket  ");
		string_append(&mensaje, sockfd);
		log_info(t_log, mensaje);
		free(mensaje);
		return 0;
	} else if (bytes_enviados < bytesAenviar) {
		char* mensaje = malloc(200);
		sprintf(mensaje, "Se enviaron %d bytes de %d esperados", bytes_enviados,
				bytesAenviar);
		log_error(t_log, mensaje);
		log_error(t_log,
				"Entrando en recursion para enviar mensaje completo \n");
		free(mensaje);
		int nuevosBytes = bytesAenviar - bytes_enviados;
		librocket_enviarMensaje(sockfd, datos + bytes_enviados, nuevosBytes,
				t_log);
	} else if (bytes_enviados == bytesAenviar) {
		//mandado piola
		return bytes_enviados;
	}

	return bytes_enviados;

}

/*
 * Funcion que se asegura de recibir todos los bytes pedidos
 * Return -1 si no se pudo recibir
 * Return 0 si la conexion esta cerrada
 * Return 1  si se recibieron todos los datos *
 * En caso de errores loguea en un archivo
 */
int librocket_recibirMensaje(int sockfd, void* buffer, size_t bytesAleer,
		t_log* log) {

	int bytesHeader = recv(sockfd, buffer, bytesAleer, 0);

	//VALIDACIONES
	if (bytesHeader == -1) { //ERROR
		perror("recv:");
		log_error(log, "Error al recibir datos");
		return -1;
	} else if (bytesHeader == 0) { //SE CERRO LA CONEXION
		return 0;
	} else if (bytesHeader < bytesAleer) {
		char* mensaje = malloc(200);
		sprintf(mensaje,
				"Se recibieron %i bytes de %i esperados en la linea %d del archivo %s",
				bytesHeader, bytesAleer, __LINE__, __FILE__);
		log_error(log, mensaje);
		log_error(log, "Entrando en recursion para recibir mensaje completo\n");
		free(mensaje);
		int nuevosBytes = bytesAleer - bytesHeader;
		librocket_recibirMensaje(sockfd, buffer + bytesHeader, nuevosBytes,
				log);
	} else if (bytesHeader == bytesAleer) {
		//se envio ok!!
		return 1;
	}
	//FIN VALIDACIONES (loguea y retorna)

	return 1;
}

//Crea un servidor de una sola conexion
//retorna 1 en caso de error
//Como parametro reciba una funcion/listener que recibe un socket y realiza operaciones sobre ese socket
//esta funcion es la encargada
int librocket_levantarServidorSimple(char * PUERTO,
		int maximo_conexiones_pendientes, void (*function)(int mSocket)) {
	t_log * info_logger = log_create(LOG_INFO_PATH, LIBRARY_NAME, false,
	LOG_LEVEL);
	t_log* error_logger = log_create(LOG_ERROR_PATH, LIBRARY_NAME, false,
	LOG_LEVEL);

	int sockfd, new_fd;
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	int yes = 1;
	int rv;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // uso mi IP

	//lleno la estructura que contiene toda la informacion del servidor
	//null en el primer parametro porque la ip ya esta contenida en hints
	if ((rv = getaddrinfo(NULL, PUERTO, &hints, &servinfo)) != 0) {

		char* mensaje = malloc(200);
		sprintf(mensaje, "getaddrinfo: %s\n", gai_strerror(rv));
		log_error(error_logger, mensaje);
		free(mensaje);
		log_destroy(error_logger);
		log_destroy(info_logger);
		return 1;
	}

	//getaddrinfo devuelve una lista de addrinfo, veo si alguna es correcta para usar
	for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol))
				== -1) {
			perror("server: socket");
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))
				== -1) {
			perror("setsockopt");

		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("server: bind");
			continue;
		}

		break;
	}

	freeaddrinfo(servinfo);

	if (p == NULL) {
		fprintf(stderr, "server: failed to bind\n");
		char* mensaje = malloc(200);
		sprintf(mensaje, "server failed to bind\n");
		log_error(error_logger, mensaje);
		free(mensaje);
		log_destroy(error_logger);
		log_destroy(info_logger);
		return 1;
	}

	if (listen(sockfd, maximo_conexiones_pendientes) == -1) {
		char* mensaje = malloc(200);
		sprintf(mensaje, "Error al escuchar sobre el socket %d", sockfd);
		log_error(error_logger, mensaje);
		free(mensaje);
		log_destroy(error_logger);
		log_destroy(info_logger);
		return 1;
	}

	log_info(info_logger, "Servidor esperando por conexiones...\n");

	while (1) {
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *) &their_addr, &sin_size);
		if (new_fd == -1) {
			char* mensaje = malloc(200);
			sprintf(mensaje, "Error al intentar aceptar una conexion");
			log_error(error_logger, mensaje);
			free(mensaje);

			continue;
		}

		//la funcion que recibo por parametro se encargara de manejar
		//la conexion con el cliente
		//char* logMsg = string_new();
		//string_append(&logMsg,"Conexion aceptada,socket ");
		//string_append(&logMsg,new_fd);
		//log_info(info_logger,logMsg);
		//free(logMsg);
		function(new_fd);

	}
	log_destroy(error_logger);
	log_destroy(info_logger);
	return 0;

}

void loguearUltimoError(t_log* errorLogger, int lineNumber, char* fileName) {
	char* myErrorMessage = string_new();
	string_append(&myErrorMessage, strerror(errno));
	string_append(&myErrorMessage, " En la linea:");
	string_append(&myErrorMessage, string_itoa(lineNumber));
	string_append(&myErrorMessage, " Y archivo:");
	string_append(&myErrorMessage, fileName);
	log_error(errorLogger, myErrorMessage);
	log_destroy(errorLogger);
	free(myErrorMessage);

}

int librocket_levantarServidorMultiplexCon3Puertos(char* PUERTO1, char* PUERTO2,
		const char* PUERTO3,
		void (*funcionParaConexionEntrante)(int mSocket, char* puerto),
		fd_set* master, void (*funcionParaMensajeRecibido)(int mSocket)) {

	fd_set read_fds; // conjunto temporal de descriptores de fichero para select()

	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_in remoteaddr; // dirección del cliente
	int fdmax = 0;
	int listenerPuerto1, listenerPuerto2, listenerPuerto3;
	int newfd;
	char buf[256]; //TODO PAQUETIZAR segun protocolo
	int nbytes;
	int yes = 1;
	socklen_t addrlen;
	int i;
	FD_ZERO(&read_fds);
	FD_ZERO(master);

	t_log* errorLogger = log_create(LOG_ERROR_PATH, LIBRARY_NAME, false,
			LOG_LEVEL_ERROR);
	t_log* infoLogger = log_create(LOG_INFO_PATH, LIBRARY_NAME, false,
			LOG_LEVEL_INFO);

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	int getaddrinfoResult;

	//PUERTO 1
	if ((getaddrinfoResult = getaddrinfo(NULL, PUERTO1, &hints, &servinfo))
			!= 0) {
		loguearUltimoError(errorLogger, __LINE__, __FILE__);

	}

	for (p = servinfo; p != NULL; p = p->ai_next) {
		listenerPuerto1 = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (listenerPuerto1 < 0) {
			continue;
		}

		setsockopt(listenerPuerto1, SOL_SOCKET, SO_REUSEADDR, &yes,
				sizeof(int));

		if (bind(listenerPuerto1, p->ai_addr, p->ai_addrlen) < 0) {
			close(listenerPuerto1);
			continue;
		}

		break;
	}

	if (p == NULL) {
		loguearUltimoError(errorLogger, __LINE__, __FILE__);
		log_destroy(errorLogger);
		//log_destroy(infoLogger);
		return 3;
	}

	freeaddrinfo(servinfo);

	//10 es la cantidad de conexiones que se admiten en cola
	if (listen(listenerPuerto1, 10) == -1) {
		loguearUltimoError(errorLogger, __LINE__, __FILE__);
		return 3;
	}

	//PUERTO 2
	if ((getaddrinfoResult = getaddrinfo(NULL, PUERTO2, &hints, &servinfo))
			!= 0) {
		loguearUltimoError(errorLogger, __LINE__, __FILE__);

	}

	for (p = servinfo; p != NULL; p = p->ai_next) {
		listenerPuerto2 = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (listenerPuerto2 < 0) {
			continue;
		}

		setsockopt(listenerPuerto2, SOL_SOCKET, SO_REUSEADDR, &yes,
				sizeof(int));

		if (bind(listenerPuerto2, p->ai_addr, p->ai_addrlen) < 0) {
			close(listenerPuerto2);
			continue;
		}

		break;
	}

	if (p == NULL) {
		loguearUltimoError(errorLogger, __LINE__, __FILE__);
		log_destroy(errorLogger);
		//log_destroy(infoLogger);
		return 3;
	}

	freeaddrinfo(servinfo);

	//10 es la cantidad de conexiones que se admiten en cola
	if (listen(listenerPuerto2, 10) == -1) {
		loguearUltimoError(errorLogger, __LINE__, __FILE__);
		return 3;
	}

	int puerto3int = atoi(PUERTO3);
	FD_SET(listenerPuerto1, master);
	FD_SET(listenerPuerto2, master);
	FD_SET(puerto3int, master);

	if (listenerPuerto1 >= listenerPuerto2)
		fdmax = listenerPuerto1;
	else if (listenerPuerto2 >= puerto3int)
		fdmax = listenerPuerto2;
	else if (listenerPuerto1 >= puerto3int)
		fdmax = listenerPuerto1;
	else
		fdmax = puerto3int;

	while (1) {

		read_fds = *master;
		if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1) {
			loguearUltimoError(errorLogger, __LINE__, __FILE__);
			log_destroy(errorLogger);
			log_destroy(infoLogger);
			return 4;
		}

		for (i = 0; i <= fdmax; i++) {

			if (FD_ISSET(i, &read_fds)) {
				if (i == listenerPuerto1 || i == listenerPuerto2) {
					// hay nuevas conexiones
					addrlen = sizeof remoteaddr;

					if (i == listenerPuerto1)
						newfd = accept(listenerPuerto1,
								(struct sockaddr *) &remoteaddr, &addrlen);
					else
						newfd = accept(listenerPuerto2,
								(struct sockaddr *) &remoteaddr, &addrlen);

					if (newfd == -1) {
						loguearUltimoError(errorLogger, __LINE__, __FILE__);
						log_destroy(errorLogger);
						log_destroy(infoLogger);
					} else {
						FD_SET(newfd, master); // lo agrego a la lista de conexiones
						if (newfd > fdmax) { // actualizo max file descriptor/socket
							fdmax = newfd;
						}
						char * infoMsg = malloc(200);
						sprintf(infoMsg,
								"selectserver: new connection from  socket %d",
								newfd);
						log_info(infoLogger, infoMsg);
						free(infoMsg);
						//llamo a la funcion pasada por parametro
						if (i == listenerPuerto1)
							funcionParaConexionEntrante(newfd, PUERTO1);
						else
							funcionParaConexionEntrante(newfd, PUERTO2);

					}
				} else {
					// un cliente mando un mensaje
					funcionParaMensajeRecibido(i);

				}

				///////////////////////////////////////////////////

			}
		}

	}
	return 0;
}

int librocket_levantarServidorDelKernel(char* PUERTO1, char* PUERTO2,
		const char* PUERTO3, int socketInotify,
		void (*funcionParaConexionEntrante)(int mSocket, char* puerto),
		fd_set* master, void (*funcionParaMensajeRecibido)(int mSocket)) {

	fd_set read_fds; // conjunto temporal de descriptores de fichero para select()

	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_in remoteaddr; // dirección del cliente
	int fdmax = 0;
	int listenerPuerto1, listenerPuerto2, listenerPuerto3;
	int newfd;
	char buf[256]; //TODO PAQUETIZAR segun protocolo
	int nbytes;
	int yes = 1;
	socklen_t addrlen;
	int i;
	FD_ZERO(&read_fds);
	FD_ZERO(master);

	t_log* errorLogger = log_create(LOG_ERROR_PATH, LIBRARY_NAME, false,
			LOG_LEVEL_ERROR);
	t_log* infoLogger = log_create(LOG_INFO_PATH, LIBRARY_NAME, false,
			LOG_LEVEL_INFO);

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	int getaddrinfoResult;

	//PUERTO 1
	if ((getaddrinfoResult = getaddrinfo(NULL, PUERTO1, &hints, &servinfo))
			!= 0) {
		loguearUltimoError(errorLogger, __LINE__, __FILE__);

	}

	for (p = servinfo; p != NULL; p = p->ai_next) {
		listenerPuerto1 = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (listenerPuerto1 < 0) {
			continue;
		}

		setsockopt(listenerPuerto1, SOL_SOCKET, SO_REUSEADDR, &yes,
				sizeof(int));

		if (bind(listenerPuerto1, p->ai_addr, p->ai_addrlen) < 0) {
			close(listenerPuerto1);
			continue;
		}

		break;
	}

	if (p == NULL) {
		loguearUltimoError(errorLogger, __LINE__, __FILE__);
		log_destroy(errorLogger);
		//log_destroy(infoLogger);
		return 3;
	}

	freeaddrinfo(servinfo);

	//10 es la cantidad de conexiones que se admiten en cola
	if (listen(listenerPuerto1, 10) == -1) {
		loguearUltimoError(errorLogger, __LINE__, __FILE__);
		return 3;
	}

	//PUERTO 2
	if ((getaddrinfoResult = getaddrinfo(NULL, PUERTO2, &hints, &servinfo))
			!= 0) {
		loguearUltimoError(errorLogger, __LINE__, __FILE__);

	}

	for (p = servinfo; p != NULL; p = p->ai_next) {
		listenerPuerto2 = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (listenerPuerto2 < 0) {
			continue;
		}

		setsockopt(listenerPuerto2, SOL_SOCKET, SO_REUSEADDR, &yes,
				sizeof(int));

		if (bind(listenerPuerto2, p->ai_addr, p->ai_addrlen) < 0) {
			close(listenerPuerto2);
			continue;
		}

		break;
	}

	if (p == NULL) {
		loguearUltimoError(errorLogger, __LINE__, __FILE__);
		log_destroy(errorLogger);
		//log_destroy(infoLogger);
		return 3;
	}

	freeaddrinfo(servinfo);

	//10 es la cantidad de conexiones que se admiten en cola
	if (listen(listenerPuerto2, 10) == -1) {
		loguearUltimoError(errorLogger, __LINE__, __FILE__);
		return 3;
	}

	int puerto3int = atoi(PUERTO3);
	FD_SET(listenerPuerto1, master);
	FD_SET(listenerPuerto2, master);
	FD_SET(puerto3int, master);
	FD_SET(socketInotify,master);

	fdmax = 0;
	//TODO Check
	if (listenerPuerto1 >= listenerPuerto2  && listenerPuerto1 >= fdmax)
		fdmax = listenerPuerto1;
	if (listenerPuerto2 >= puerto3int && listenerPuerto2 >= fdmax)
		fdmax = listenerPuerto2;
	if (listenerPuerto1 >= puerto3int)
		fdmax = listenerPuerto1;
	else
		fdmax = puerto3int;

	while (1) {

		read_fds = *master;
		if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1) {
			loguearUltimoError(errorLogger, __LINE__, __FILE__);
			log_destroy(errorLogger);
			log_destroy(infoLogger);
			return 4;
		}

		for (i = 0; i <= fdmax; i++) {

			if (FD_ISSET(i, &read_fds)) {
				if (i == listenerPuerto1 || i == listenerPuerto2) {
					// hay nuevas conexiones
					addrlen = sizeof remoteaddr;

					if (i == listenerPuerto1)
						newfd = accept(listenerPuerto1,
								(struct sockaddr *) &remoteaddr, &addrlen);
					else
						newfd = accept(listenerPuerto2,
								(struct sockaddr *) &remoteaddr, &addrlen);

					if (newfd == -1) {
						loguearUltimoError(errorLogger, __LINE__, __FILE__);
						log_destroy(errorLogger);
						log_destroy(infoLogger);
					} else {
						FD_SET(newfd, master); // lo agrego a la lista de conexiones
						if (newfd > fdmax) { // actualizo max file descriptor/socket
							fdmax = newfd;
						}
						char * infoMsg = malloc(200);
						sprintf(infoMsg,
								"selectserver: new connection from  socket %d",
								newfd);
						log_info(infoLogger, infoMsg);
						free(infoMsg);
						//llamo a la funcion pasada por parametro
						if (i == listenerPuerto1)
							funcionParaConexionEntrante(newfd, PUERTO1);
						else
							funcionParaConexionEntrante(newfd, PUERTO2);

					}
				} else {
					// un cliente mando un mensaje
					funcionParaMensajeRecibido(i);

				}

				///////////////////////////////////////////////////

			}
		}

	}
	return 0;
}


int librocket_levantarServidorMultiplexado(char* PUERTO,
		void (*funcionParaConexionEntrante)(int mSocket,
				struct sockaddr_in remoteaddr), fd_set* master,
		void (*funcionParaMensajeRecibido)(int mSocket)) {

	fd_set read_fds; // conjunto temporal de descriptores de fichero para select()

	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_in remoteaddr; // dirección del cliente
	int fdmax = 0;
	int listener;
	int newfd;
	char buf[256]; //TODO PAQUETIZAR segun protocolo
	int nbytes;
	int yes = 1;
	socklen_t addrlen;
	int i;
	FD_ZERO(&read_fds);
	FD_ZERO(master);

	t_log* errorLogger = log_create(LOG_ERROR_PATH, LIBRARY_NAME, false,
	LOG_LEVEL);
	t_log* infoLogger = log_create(LOG_INFO_PATH, LIBRARY_NAME, false,
	LOG_LEVEL);

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	int getaddrinfoResult;

	if ((getaddrinfoResult = getaddrinfo(NULL, PUERTO, &hints, &servinfo))
			!= 0) {
		loguearUltimoError(errorLogger, __LINE__, __FILE__);

	}

	for (p = servinfo; p != NULL; p = p->ai_next) {
		listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (listener < 0) {
			continue;
		}

		setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

		if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
			close(listener);
			continue;
		}

		break;
	}

	if (p == NULL) {
		loguearUltimoError(errorLogger, __LINE__, __FILE__);
		log_destroy(errorLogger);
		//log_destroy(infoLogger);
		return 3;
	}

	freeaddrinfo(servinfo);

	//10 es la cantidad de conexiones que se admiten en cola
	if (listen(listener, 10) == -1) {
		loguearUltimoError(errorLogger, __LINE__, __FILE__);
		return 3;
	}
	FD_SET(listener, master);
	fdmax = listener;
	while (1) {

		read_fds = *master;
		if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1) {
			loguearUltimoError(errorLogger, __LINE__, __FILE__);
			log_destroy(errorLogger);
			log_destroy(infoLogger);
			return 4;
		}

		for (i = 0; i <= fdmax; i++) {

			if (FD_ISSET(i, &read_fds)) {
				if (i == listener) {
					// hay nuevas conexiones
					addrlen = sizeof remoteaddr;
					newfd = accept(listener, (struct sockaddr *) &remoteaddr,
							&addrlen);

					if (newfd == -1) {
						loguearUltimoError(errorLogger, __LINE__, __FILE__);
						log_destroy(errorLogger);
						log_destroy(infoLogger);
					} else {
						FD_SET(newfd, master); // lo agrego a la lista de conexiones
						if (newfd > fdmax) { // actualizo max file descriptor/socket
							fdmax = newfd;
						}
						char * infoMsg = malloc(200);
						sprintf(infoMsg,
								"Nueva conexion del socket %d",
								newfd);
						log_info(infoLogger, infoMsg);
						free(infoMsg);
						//llamo a la funcion pasada por parametro
						funcionParaConexionEntrante(newfd, remoteaddr);

					}
				} else {
					// un cliente mando un mensaje
					funcionParaMensajeRecibido(i);

				}

				///////////////////////////////////////////////////

			}
		}

	}
	return 0;
}

/*
 * se debe recibir por parametro la funcion que se debe encargar de procesar y mandar todos los mensajes
 * ademas debe encargarse de la concurrencia y sincronizacion (Ver abajo una funcion de ejemplo)
 * Retorna 1 en caso de error, 0 de lo contrario
 */
int librocket_levantarServidorConMultiplesHilos(char* PUERTO,
		void* (*funcionParaElHilo)(void sockfd)) {
	t_log * info_logger = log_create(LOG_INFO_PATH, LIBRARY_NAME, false,
	LOG_LEVEL);
	t_log* error_logger = log_create(LOG_ERROR_PATH, LIBRARY_NAME, false,
	LOG_LEVEL);
	int maximo_conexiones_pendientes = 20;

	int sockfd, new_fd;
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	int yes = 1;
	int rv;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // uso mi IP

	//lleno la estructura que contiene toda la informacion del servidor
	//null en el primer parametro porque la ip ya esta contenida en hints

	if ((rv = getaddrinfo(NULL, PUERTO, &hints, &servinfo)) != 0) {

		char* mensaje = malloc(200);
		sprintf(mensaje, "getaddrinfo: %s\n", gai_strerror(rv));
		log_error(error_logger, mensaje);
		free(mensaje);
		log_destroy(error_logger);
		log_destroy(info_logger);
		return 1;
	}

	//getaddrinfo devuelve una lista de addrinfo, veo si alguna es correcta para usar
	for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol))
				== -1) {
			perror("server: socket");
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))
				== -1) {
			perror("setsockopt");

		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("server: bind");
			continue;
		}

		break;
	}

	freeaddrinfo(servinfo);

	if (p == NULL) {
		fprintf(stderr, "Servidor:Fallo en el bind\n");
		char* mensaje = malloc(200);
		sprintf(mensaje, "Servidor: Fallo en el bind\n");
		log_error(error_logger, mensaje);
		free(mensaje);
		log_destroy(error_logger);
		log_destroy(info_logger);
		return 1;
	}

	if (listen(sockfd, maximo_conexiones_pendientes) == -1) {
		char* mensaje = malloc(200);
		sprintf(mensaje, "Error al escuchar sobre el socket %d", sockfd);
		log_error(error_logger, mensaje);
		free(mensaje);
		log_destroy(error_logger);
		log_destroy(info_logger);
		return 1;
	}

	log_info(info_logger, "Servidor esperando por conexiones...\n");

	while (1) {
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *) &their_addr, &sin_size);
		if (new_fd == -1) {
			char* mensaje = malloc(200);
			sprintf(mensaje, "Error al intentar aceptar una conexion");
			log_error(error_logger, mensaje);
			free(mensaje);

			continue;
		}

		//la funcion que recibo por parametro se encargara de manejar
		//la conexion con el cliente
		//char* logMsg = string_new();
		//string_append(&logMsg,"Conexion aceptada,socket ");
		//string_append(&logMsg,new_fd);
		//log_info(info_logger,logMsg);
		//free(logMsg);
		funcionCreadoraDeHilos(new_fd, funcionParaElHilo);

	}
	log_destroy(error_logger);
	log_destroy(info_logger);
	return 0;

	//termina la funcion multiples hilos
}

/*
 *cada vez que se conecta un socket se llama a esta funcion pasandole el correspondiente socket
 *y crea un hilo que se encargara de la conexiones
 *
 */
void funcionCreadoraDeHilos(int sockfd, void* (*funcionParaElHilo)(void sockfd)) {
	pthread_t hilo;

	printf("Conexion recibida\n");
	crearHilo(&hilo, funcionParaElHilo, sockfd);

}

void * hiloManejador(void* sockfd) {

	printf("Hilo creado\n");
	int clienteSocket = (int) sockfd;
	t_log* logger = log_create(LOG_INFO_PATH, LIBRARY_NAME, false,
			LOG_LEVEL_INFO);

	char* buffer;

	while (1) {
		buffer = malloc(5);
		//recibo el header
		int result = librocket_recibirMensaje(clienteSocket, buffer, 4, logger);

		switch (result) {
		case 0: //cliente se desconecto
			printf("Socket %d cerrado, cerramos el hilo\n", clienteSocket);
			fflush(stdout);
			close(clienteSocket);
			return NULL; //CLIENTE DESCONECTADO, SE CIERRA EL HILO
		case -1: //error
			printf("error!\n");
			break;

		case 1: //datos recibidos bien
			buffer[4] = '\0';
			printf("Header: %s \n", buffer);
			free(buffer);
			break;

		}

	}

	return NULL;

}


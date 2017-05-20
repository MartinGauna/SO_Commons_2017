/*
 * socket.c
 *
 *  Created on: 7/4/2017
 *      Author: utnso
 */

#include "socket.h"

/**
 * @NAME: escuchar
 * @DESC: crea un socket y se encarga de hacer un liste sobre el mismo.
 * @PARAMS: {int} 	puerto Numero de puerto
 * 			{int *}	socket Puntero en el que se almacenara el socket, necesita tener la memoria asignada.
 */
int escuchar(int puerto, int* socket, t_log* logger){
	int listenBacklog = BACKLOG;
	if(cargarSoket(puerto, "",  socket, logger)){
		return EXIT_FAILURE;
	}
	if(listen(*socket, listenBacklog) < 0){
		log_error(logger, "Error en el listen: %s",strerror(errno));
		close(*socket);
		return EXIT_FAILURE;
	}
	log_info(logger, "Escuchando en el puerto: %d", puerto);
	return EXIT_SUCCESS;
}

/**
 * @NAME: escuchar
 * @DESC: Acepta una conexion sobre un socket que esta haciendo un listen. por lo que hay que llamar a escuchar antes que a esta funcion.
 * @PARAMS: {int} 	socket Socket que esta escuchando
 * 			{int *}	newSocket Nuevo sokcet generado al hacer el accept.
 */
int aceptar(int socket,int* newSocket, t_log* logger){
	struct sockaddr_storage their_addr;
	socklen_t addrSize;
	addrSize = sizeof their_addr;
	*newSocket = accept(socket, (struct sockaddr *)&their_addr, &addrSize);
	if(*newSocket < 0){
		log_error(logger, "Error en el accept: %s",strerror(errno));
		return EXIT_FAILURE;
	}
	log_info(logger, "Conexion aceptada.");
	return EXIT_SUCCESS;
}

/**
 * @NAME: cargarSoket
 * @DESC: Crea un socket que se puede usar tanto para listen, si no se le pasa la ip, como para connect,
 * 		  si le paso la ip a la que me quiero conectar. La memoria de la variables puntero debe estar previamente asignada.
 * @PARAMS: {int} 	iPuerto puesto que se le asigna al socket.
 * 			{int*}	ip ip a la que me quiero conectar, o si es null el socket se puede usar para hacer listen.
 * 			{int*}	pSocket Socket creado dentro de la funcion.
 */
int cargarSoket(int iPuerto,const char* ip, int* pSocket, t_log* logger){
	int socketFD;
	struct addrinfo hints, *servInfo, *p;
	int rv;
	char* puerto = string_itoa(iPuerto);

	if(!strcmp(puerto,"")){
		log_error(logger,"Error al convertir el puerto a string.");
		return EXIT_FAILURE;
	}
	memset(&hints,0,sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	if(!strcmp(ip,"")){		// use my IP
		hints.ai_flags = AI_PASSIVE;
		ip = NULL;
	}

	if ((rv = getaddrinfo(ip, puerto, &hints, &servInfo)) != 0) {
//		char* addrError = string_from_format("getaddrinfo: %s\n", gai_strerror(rv));
		log_error(logger,"getaddrinfo:%s", gai_strerror(rv) );
//		free(addrError);
		return EXIT_FAILURE;
	}
	for(p = servInfo; p != NULL; p = p->ai_next) {
		if ((socketFD = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			log_warning(logger, "Socket error: %s",strerror(errno));
			continue;
		}
		if(ip == NULL){
			if(bind(socketFD, p->ai_addr, p->ai_addrlen) == -1) {
				close(socketFD);
				log_warning(logger, "Bind error: %s",strerror(errno));
				continue;
			}
		}
		else{
			if (connect(socketFD, p->ai_addr, p->ai_addrlen) == -1) {
				close(socketFD);
				log_warning(logger, "Connect error: %s",strerror(errno));
				continue;
			}
		}
		break;
	}
	freeaddrinfo(servInfo);
	if(p==NULL){
		log_error(logger, "No se pudo crear el socket.");
		exit(EXIT_FAILURE);
	}
	*pSocket = socketFD;
	log_trace(logger,"socket - SocketFD: %d",socketFD);
	return EXIT_SUCCESS;
}

/**
 * @NAME: enviarHandshake
 * @DESC: Envia un codigo de handshake,luego recibe la respuesta y chequea que sea la esperada.
 * 		  NOTA IMPORTANTE: no tengo que llamar a recibirHandshake,
 * @PARAMS: {int} 	socket por el que realizo la comunicación.
 * 			{uint16_t}	codigoMio Codigo del programa que llama a la funcion.
 * 			{uint16_t}	codigoOtro Codigo del programa  al que me quiero conectar.
 */
int enviarHandshake (int socket, uint16_t codigoMio, uint16_t codigoOtro, t_log* logger){
	t_package handshakeRcv;

	if(enviar(socket,codigoMio,NULL,0,logger)){
		log_error(logger, "Error al enviar el handshake");
		return EXIT_FAILURE;
	}
	if(recibir(socket, &handshakeRcv, logger)){
		log_error(logger,"Error al recibir el handshake");
		return EXIT_FAILURE;
	}
	if(handshakeRcv.code != codigoOtro){
		log_warning(logger, "Codigo de handshake incorrecto");
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

int recibirHandshake (int socket, uint16_t codigoMio, uint16_t* codigoOtro, t_log* logger){
	t_package handshakeRcv;
	if(recibir(socket, &handshakeRcv, logger)){
		log_error(logger,"Error al recibir el handshake.");
		return EXIT_FAILURE;
	}
	*codigoOtro = handshakeRcv.code;
	if(enviar(socket,codigoMio,NULL,0,logger)){
		log_error(logger, "Error al enviar el handshake");
		return EXIT_FAILURE;
	}
	log_debug(logger, "Codigo de Handshake recibido: %d.",handshakeRcv.code);
	return EXIT_SUCCESS;
}

uint32_t packageSize(uint32_t size){
	return size + packageHeaderSize;
}

char* compress(int code, char* data, uint32_t size, t_log* logger){
	char* compressPack = (char*) malloc(packageSize(size));
	if (compressPack != NULL){
		memcpy(compressPack, &code, sizeof(uint16_t));
		memcpy(compressPack + sizeof(uint16_t), &size, sizeof(uint32_t));
		memcpy(compressPack + packageSize(0), data, size);
		return compressPack;
	}
	free(compress);
	log_error(logger,"Error al asignar espacio para el packete de salida");
	return NULL;
}

int enviar(int socket, uint16_t code, char* data, uint32_t size, t_log* logger){
	log_trace(logger,"enviar()");
	char* package = compress(code, data, size, logger);
	int sizeOfData = packageSize(size);
	int totalDataSent = 0;
	int sent;

	do{
		sent = send(socket, package, packageSize(size) - totalDataSent, 0);
		if (sent < 0){
			log_error(logger, "Error al enviar: %s",strerror(errno));
			return EXIT_FAILURE;
		}
		totalDataSent += sent;
	}while(totalDataSent < sizeOfData);
	log_debug(logger, "send: %s",strerror(errno));
	log_trace(logger, "Codigo:%d\tSize:%d",code, size);
	return EXIT_SUCCESS;
}

int recibir(int socket,t_package* mensaje, t_log* logger){
	int headerSize = packageHeaderSize;
	char* buffer;
	//Recibo el header primero
	if(recvPkg(socket, &buffer, headerSize, logger)){
		return EXIT_FAILURE;
	}
	//descomprimo el header.
	memcpy(&(mensaje->code), buffer, sizeof(uint16_t));
	memcpy(&(mensaje->size), buffer+sizeof(uint16_t), sizeof(uint32_t));
	log_debug(logger, "Header Recibido - Code:%d\tSize:%d", mensaje->code,mensaje->size);
	if(buffer){
		free(buffer);
	}
	if(!mensaje->size){
		return EXIT_SUCCESS;
	}
	//Ahora recibo los datos de ser necesario.
	if(recvPkg(socket, &buffer,mensaje->size, logger)){
		return EXIT_FAILURE;
	}
	mensaje->data = buffer;
	log_trace(logger, "Data length: %d\nData: %s",mensaje->size+1, mensaje->data);
	return EXIT_SUCCESS;
}

int recvPkg(int socket, char** buffer, uint32_t size, t_log* logger){
	int recibido, recibidoTotal=0;
	char* buff, *buffAux;
	*buffer = NULL;
	if(size < 1){
		return EXIT_SUCCESS;
	}
	buff = (char*)malloc(sizeof(size)+1);
	buffAux = (char*)malloc(sizeof(size)+1);
	memset(buffAux,'\0',size + 1);
	do{
		memset(buff,'\0',size + 1);
		recibido = recv(socket, buff, size,0);
		if(recibido < 0){
			free(buff);
			free(buffAux);
			log_error(logger, "Error al recibir: %s",strerror(errno));
			return EXIT_FAILURE;
		}if(recibido > 0){
			memcpy((buffAux+recibidoTotal), buff, recibido);
		}
		recibidoTotal += recibido;
	} while(recibidoTotal < size);
	free(buff);
	(*buffer) = buffAux;
	return EXIT_SUCCESS;
}

int highestFD(int fd, int nfd){
	if(fd >= nfd){
		return fd + 1;
	}
	return nfd;
}

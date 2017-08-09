#include "cliente.h"

//Devuelve 0 en caso de que se haya conectado al ip y puerto indicados
//setea en el parametro pasado por refencia 'sock_int' el valor del socket de conexion
int libconnections_conectar_a_servidor(char * IP, char * PUERTO, int * sock_int) {

	int sockfd;

	struct addrinfo hints, *servinfo, *p;
	int rv;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(IP, PUERTO, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return -1;
	}

	for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol))
				== -1) {
			perror("client: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);

			continue;
		}

		break;
	}

	if (p == NULL) {
		char* string = string_new();
		string_append(&string, "No se pudo conectar a ");
		string_append(&string, IP);
		string_append(&string, " al puerto ");
		string_append(&string, PUERTO);
		string_append(&string, "\n");
		fprintf(stderr, string);
		return 2;
	}

	(*sock_int) = sockfd;
	freeaddrinfo(servinfo); // all done with this structure
	//el socket no lo quiero cerrar, debe cerrarlo quien llame a la funcion
	return EXIT_SUCCESS;

}

void *get_in_addr(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*) sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*) sa)->sin6_addr);
}

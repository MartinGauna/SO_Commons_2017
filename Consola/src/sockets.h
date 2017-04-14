#ifndef SOCKETS_H_
#define SOCKETS_H_

void create_server(int* listenningSocket, char* port);

int accept_connection(int listenningSocket, int* socketCliente);

void create_client(int* serverSocket, char* ip, char* port);
void handShakeServidor(int serverSocket,const char * servidor,const char * cliente);
void handShakeCliente(int clienteSocket,const char * servidor,const char * cliente);
#endif /* SOCKETS_H_ */

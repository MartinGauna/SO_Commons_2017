/*
 * funciones.h
 *
 *  Created on: 30/4/2017
 *      Author: utnso
 */

#ifndef FUNCIONES_H_
#define FUNCIONES_H_

#define CONSOLA 1
#define CPU 2
#define MEMORIA 3
#define FILESYSTEM 4
#define KERNEL 5

#define PACKAGESIZE 1024
#define PATH 300
#define INICIAR_PROGRAMA "Iniciar Programa\n"
#define FINALIZAR_PROGRAMA "Finalizar Programa\n"
#define DESCONECTAR_CONSOLA "Desconectar Consola\n"
#define LIMPIAR_MENSAJE "Limpiar Mensajes\n"

#define TRUE  1;
#define FALSE  0;

typedef struct {
	long id;
	long tamanio; 		// Estructura del Header de un mensaje
} t_headMensaje;

typedef struct{
	int* PID;
	int* contadorDePaginas;
}pcb_t;

typedef struct FDSockets {
	   int fd;
	   char nombre[20];
	   int activo;
} FDSocket;

typedef struct{
	int socketCliente;
	fd_set master;
	int socketFS;
	int socketM;
	//FDSocket fdSocket[100];
	int fdmax;
}structParaCliente;

void* consolaKernel(void* unused);
void* atenderCliente(void* estructura);
int* leerHeader(int socketARecibir);
void* recibirStream(int socketDondeRecibe, int tamanioEstructuraARecibir);
int enviarStream(int socketDestino,int header, int tamanioMensaje, void* mensaje);

#endif /* FUNCIONES_H_ */

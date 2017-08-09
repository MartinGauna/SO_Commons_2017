#ifndef HEADERS_CONEXIONESKERNELCONSOLA_H_
#define HEADERS_CONEXIONESKERNELCONSOLA_H_

// INCLUSIONES
#include <stdio.h>
#include <stdlib.h>
#include <connections/networkHandler.h>
#include <protocolos/protocols.h>
#include "pcbOperaciones.h"
#include "kernel.h"
#include <parser/metadata_program.h>
#include "planificacion.h"

// ESTRUCTURAS
struct arg_struct {
	char* PUERTO;
};

// VARIABLES
pthread_t hiloConsola;
pthread_t hiloCpu;
pthread_t* hiloServidorSimpleMemoria;

// FUNCIONES
void enviarMensajeATodosLosProcesos(char*);
void* nuevaConsolaConectada(int, struct sockaddr_in);
void procesarMensajeDeLaConsola(int socket, int32_t tipoMensaje);
void mensajeRecibidoDeConsola(int socket);
void iniciarProceso(int consolaSocket);
void finalizarProceso(int programaSocket);
void cargarMetadata(char* codigoPrograma, t_pcb* pcb);

#endif

/*
 * consola.h
 *
 *  Created on: 19/5/2017
 *      Author: utnso
 */

#ifndef CONSOLA_H_
#define CONSOLA_H_

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
#include <signal.h>
#include "funcionesConsola.h"
#include "../Commons/configuracion.h"
#include "../Commons/socket.h"

#define PACKAGESIZE 1024
#define PATH 300
#define INICIAR_PROGRAMA "iniciar programa\n"
#define FINALIZAR_PROGRAMA "finalizar programa\n"
#define DESCONECTAR_CONSOLA "desconectar consola\n"
#define LIMPIAR_MENSAJE "limpiar mensajes\n"
#define ENVIAR_MENSAJE "enviar mensajes\n"
#define HELP "help\n"


#endif /* CONSOLA_H_ */

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
#define INICIAR_PROGRAMA "iniciar\n"
#define FINALIZAR_PROGRAMA "fin\n"
#define DESCONECTAR_CONSOLA "desconectar\n"
#define LIMPIAR_MENSAJE "limpiar\n"
#define ENVIAR_MENSAJE "enviar\n"
#define HELP "help\n"

extern t_log* logger;
extern pthread_mutex_t lock;



#endif /* CONSOLA_H_ */

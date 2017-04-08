/*
 ============================================================================
 Name        : Consola.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <sys/socket.h>
#include "configConsola.h"
#include "socketsConsola.h"

int main(void) {

	t_regConfig archConfig = leeArchConfigConsola("../archivoConfiguracionConsola.cfg");

	struct cliente consola = newClient(archConfig.IP_KERNEL, archConfig.PUERTO_KERNELL);

	while((connectServer(consola)) == -1) {
		printf("CONSOLA: Error al intentar conectar con el Kernel, se realizara un "
				"nuevo intento en 3 segundos.");
		sleep(3);
	}

	handShakeClient(consola.socketCliente, "Hola soy una consola y me quiero conectar a ti.");

	sendMessageToServer(consola.socketCliente, "Estoy funcionando!");
	//falta definir el tamaño del mensaje

	return 0;
}

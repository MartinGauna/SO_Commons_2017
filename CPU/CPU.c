//
// Created by Martin Gauna on 4/2/17.
//
/*
 * #!/usr/bin/ansisop
 begin
 variables a, b
 a = 3
 b = 5
 a = b + 12
 end
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
#include <commons/string.h>
#include <features.h>
#include <netinet/in.h>
#include "funcionesCPU.h"

t_log *logger;
t_log* testlog;

//AnSISOP_funciones primitivas = {
//		.AnSISOP_definirVariable		= definirVariable,
//		.AnSISOP_obtenerPosicionVariable= obtenerPosicionVariable,
//		.AnSISOP_dereferenciar			= dereferenciar,
//		.AnSISOP_asignar				= asignar,
//		.AnSISOP_obtenerValorCompartida = obtenerValorCompartida,
//		.AnSISOP_asignarValorCompartida = asignarValorCompartida,
//		.AnSISOP_irAlLabel				= irAlLabel,
//		.AnSISOP_llamarConRetorno		= llamarConRetorno,
//		.AnSISOP_retornar				= retornar,
//		//.AnSISOP_imprimir				= imprimir,
//		//.AnSISOP_imprimirTexto			= imprimirTexto,
//		//.AnSISOP_entradaSalida			= entradaSalida,
//		.AnSISOP_finalizar				= finalizar
//
//};
//AnSISOP_kernel primitivas_kernel = {
//		.AnSISOP_wait					=wait_kernel,
//		.AnSISOP_signal					=signal_kernel,
//		.AnSISOP_reservar				=reservar_kernel,
//		.AnSISOP_liberar				=liberar_kernel,
////		.AnSISOP_abrir					=abrir_kernel,
//		.AnSISOP_borrar					=borrar_kernel,
//		.AnSISOP_cerrar					=cerrar_kernel,
//		.AnSISOP_moverCursor			=moverCursor_kernel,
//		.AnSISOP_escribir				=escribir_kernel,
//		.AnSISOP_leer					=leer_kernel
//};

int main (int argc, char *argv[]) {
	t_log* logger = log_create("log_kernel", "CPU", 1, LOG_LEVEL_TRACE);
	configCPU* conf = (configCPU*) cargarConfiguracion("./config", 4, CPU, logger);
	int socketKernel, socketMemoria;
	t_package pkg;

	puts("CPU.");
	printf("IP_KERNEL: %s\n",conf->ipKernel);
	printf("IP_MEMORIA: %s\n",conf->ipMemoria);
	printf("PUERTO_KERNEL: %d\n",conf->puertoKernel);
	printf("PUERTO_MEMORIA %d\n",conf->puertoMemoria);
	/*Para cuando ande el parser:
	char* sentencia = 'variables a, b';
	analizadorLinea(sentencia, &primitivas,&primitivas_kernel);
	printf("chau capo");
	*/

	if(cargarSoket(conf->puertoKernel, conf->ipKernel, &socketKernel, logger)){
		//ERROR
	}
	if(enviarHandshake(socketKernel, CPU_HSK, KERNEL_HSK,logger)){
		//ERROR
	}
	log_debug(logger, "Conectado al Kernel");
	if(cargarSoket(conf->puertoMemoria, conf->ipMemoria, &socketMemoria, logger)){
		//ERROR
	}
	if(enviarHandshake(socketMemoria, CPU_HSK, MEMORIA_HSK,logger)){
		//ERROR
	}
	log_debug(logger, "Conectado a la Memoria");

	while(1){
		printf("Esperando mensaje del Kernel.\n");
		if(recibir(socketKernel, &pkg, logger)){
			//ERROR
			close(socketKernel);
			return EXIT_FAILURE;
		}
		printf("Mensaje recibido del kernels: %s\n",pkg.data);
		free(pkg.data);
	}
	liberar_memoria(logger, conf);
	return EXIT_SUCCESS;
}


#include "headers/conexionCpuKernel.h"

void conectarseAlKernel() {

	int result = libconnections_conectar_a_servidor(cpu_config->ipKernel,
			cpu_config->puertoKernel, &kernelSocket);

	if (result != 0) {
		log_info(infoLogger, "No se pudo conectar al Kernel.");
		perror("No se pudo conectar al Kernel.");
		exit(EXIT_FAILURE);
	}

	PROTOCOLO_CPU_A_KERNEL handshake = HANDSHAKE_CONECTAR_CPU_A_KERNEL;
	librocket_enviarMensaje(kernelSocket, &handshake,
			sizeof(PROTOCOLO_CPU_A_KERNEL), infoLogger);

	PROTOCOLO_KERNEL_A_CPU handshakeRespuesta;
	result = librocket_recibirMensaje(kernelSocket, &handshakeRespuesta,
			sizeof(PROTOCOLO_KERNEL_A_CPU), infoLogger);

	switch (result) {
	case CONEXION_CERRADA:
		log_info(infoLogger, "Se desconecto el Kernel.");
		printf("Se desconecto el Kernel.\n");
		perror("Se desconecto el Kernel.\n");
		break;

	case RECIBIDO_CORRECTAMENTE:
		if (handshakeRespuesta == HANDSHAKE_CONEXION_DE_CPU_A_KERNEL_ACEPTADA) {
			escucharMensajesDelKernel();
		}
		break;

	case NO_SE_PUDO_RECIBIR:
		log_info(infoLogger, "Error recibiendo data del Kernel.");
		perror("Error recibiendo data del Kernel.\n");
		break;
	}

}

void escucharMensajesDelKernel() {

	while (1) {

		PROTOCOLO_KERNEL_A_CPU mensaje;

		int result = librocket_recibirMensaje(kernelSocket, &mensaje,
				sizeof(PROTOCOLO_KERNEL_A_CPU), infoLogger);

		switch (result) {
		case CONEXION_CERRADA:
			log_info(infoLogger, "Se desconecto el Kernel.");
			printf("Se desconecto el Kernel.\n");
			perror("Se desconecto el Kernel.\n");
			return;

		case RECIBIDO_CORRECTAMENTE:
			procesarMensajeDelKernel(mensaje);
			break;

		case NO_SE_PUDO_RECIBIR:
			log_info(infoLogger, "Error recibiendo data del Kernel.");
			perror("Error recibiendo data del Kernel.\n");
			break;
		}

	}
}

void procesarMensajeDelKernel(PROTOCOLO_KERNEL_A_CPU mensaje) {

	if (mensaje == ENVIO_TEXTO_A_CPU) {

		int32_t longitud = 0;

		librocket_recibirMensaje(kernelSocket, &longitud, sizeof(int32_t),
				infoLogger);

		char* texto = malloc(longitud);

		librocket_recibirMensaje(kernelSocket, texto, longitud, infoLogger);

		printf("El kernel envio un texto: %s.\n", texto);

		free(texto);

	} else if (mensaje == EJECUTAR_PROGRAMA) {

		pcbActual = recibirPcb(kernelSocket, infoLogger);

		printf("Se recibio un programa para ejecutar con PID: %i.\n",
				pcbActual->PID);

		log_info(infoEjecucion, "===============NUEVO PROCESO===============");
		log_info(infoEjecucion, "PID: %i.", pcbActual->PID);
		log_info(infoEjecucion,
				"==============TRAZA DE EJECUCION=============");

		pcb_ejecutando = true;
		pcbActual->banderas.ejecutando = true;

		ejecutarPrograma();

	} else if (mensaje == ENVIO_TAMANIO_PAGINA) {

		librocket_recibirMensaje(kernelSocket, &tamanioPagina, sizeof(int32_t),
				infoLogger);
		log_info(infoLogger, "Se recibio el tamanio de pagina: %i.",
				tamanioPagina);

	} else {

		log_info(infoLogger, "No se pudo entender el mensaje del Kernel.");

	}

}


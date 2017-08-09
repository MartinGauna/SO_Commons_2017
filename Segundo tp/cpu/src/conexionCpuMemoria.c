#include "headers/conexionCpuMemoria.h"

void conectarseAMemoria() {

	int result = libconnections_conectar_a_servidor(cpu_config->ipMemoria,
			cpu_config->puertoMemoria, &memoriaSocket);

	if (result != 0) {
		perror("No se pudo conectar a Memoria.\n");
		exit(1);
	}

	PROTOCOLO_CONECTAR_A_MEMORIA handshake = HANDSHAKE_CONECTAR_CPU_A_MEMORIA;
	librocket_enviarMensaje(memoriaSocket, &handshake, sizeof(int32_t),
			infoLogger);

	PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU handshakeRespuesta;
	result = librocket_recibirMensaje(memoriaSocket, &handshakeRespuesta,
			sizeof(PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU), infoLogger);

	switch (result) {
	case CONEXION_CERRADA:
		printf("Se desconecto la Memoria.\n");
		perror("Se desconecto la Memoria.\n");
		break;

	case RECIBIDO_CORRECTAMENTE:
		if (handshakeRespuesta
				== HANDSHAKE_CPU_CONECTADO) {
			escucharMensajesDeMemoria();
		}
		break;

	case NO_SE_PUDO_RECIBIR:
		perror("Error recibiendo data de la Memoria.\n");
		break;
	}

}

void escucharMensajesDeMemoria() {

	while (1) {

		PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU mensaje;

		int result = librocket_recibirMensaje(memoriaSocket, &mensaje,
				sizeof(PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU), infoLogger);

		switch (result) {
		case CONEXION_CERRADA:
			printf("Se desconecto la Memoria.\n");
			perror("Se desconecto la Memoria.\n");
			return;

		case RECIBIDO_CORRECTAMENTE:
			procesarMensajeDeMemoria(mensaje);
			break;

		case NO_SE_PUDO_RECIBIR:
			perror("Error recibiendo data de la Memoria.\n");
			break;
		}

	}
}

void procesarMensajeDeMemoria(PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU mensaje) {

}

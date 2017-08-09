#include "headers/conexionesKernelConsola.h"

void* nuevaConsolaConectada(int socket, struct sockaddr_in remote) {

	log_info(infoLogger, "Se conecto una consola.\n");
	fflush(stdout);
	return NULL;

}

void mensajeRecibidoDeConsola(int socket) {
	PROTOCOLO_CONSOLA_A_KERNEL mensaje;
	PROTOCOLO_KERNEL_A_CONSOLA respuesta;

	int result = librocket_recibirMensaje(socket, &mensaje,
			sizeof(PROTOCOLO_CONSOLA_A_KERNEL), infoLogger);

	if (result == 0) {
		//se cerro la conexion
		log_info(infoLogger, "Se desconecto la consola socket %d",socket);
		pedidoAbortarProgramaDeConsola(socket, true);
		FD_CLR(socket, &socketsConsolaSet);
		return;
	}

	if (mensaje == HANDSHAKE_CONECTAR_CONSOLA_A_KERNEL) {
		//una consola se acaba de conectar
		respuesta = HANDSHAKE_CONEXION_DE_CONSOLA_A_KERNEL_ACEPTADA;
		librocket_enviarMensaje(socket, &respuesta,
				sizeof(PROTOCOLO_KERNEL_A_CONSOLA), infoLogger);
	} else if (mensaje == ENVIO_TEXTO_CONSOLA) {
		//consola envio un mensaje de texto
		int32_t tamanioTexto;
		librocket_recibirMensaje(socket, &tamanioTexto, sizeof(int32_t),
				infoLogger);
		char * texto = malloc(tamanioTexto);
		librocket_recibirMensaje(socket, texto, tamanioTexto, infoLogger);
		log_info(infoLogger, "Una consola envio un texto:");
		log_info(infoLogger, texto);
		enviarMensajeATodosLosProcesos(texto);
	} else if (mensaje == CONSOLA_PIDE_INICIAR_PROCESO) {
		iniciarProceso(socket);
	} else if (mensaje == CONSOLA_PIDE_FINALIZAR_PROCESO) {
		pedidoAbortarProgramaDeConsola(socket, false);
	} else {
		log_info(infoLogger,
				"La consola envio un mensaje pero el kernel no puede entenderlo.\n");
	}
}

void iniciarProceso(int programaSocket) {

	PROTOCOLO_KERNEL_A_CONSOLA respuesta;
	int32_t tamanioCodigoPrograma;

	librocket_recibirMensaje(programaSocket, &tamanioCodigoPrograma,
			sizeof(int32_t), infoLogger);

	char* codigoPrograma = malloc(tamanioCodigoPrograma);

	librocket_recibirMensaje(programaSocket, codigoPrograma,
			tamanioCodigoPrograma, infoLogger);

	log_info(infoLogger,
			"La consola con socket %d pidio abrir un proceso cuyo codigo es %s",
			programaSocket, codigoPrograma);

	t_pcb* pcb = NULL;
	if (checkNivelMultiprogramacion()) {
		crearNuevoProcesoListo(&pcb);

		//agrego una entrada para saber mas tarde a que pid le corresponde el hilo programa
		entrada_programaSocket_pid* entrada = malloc(
				sizeof(entrada_programaSocket_pid));
		entrada->programaSocket = programaSocket;
		entrada->pid = pcb->PID;

		list_add(listaProgramaSocketPid, entrada);

		//Envio a la consola que se admitio el proceso y su PID
		respuesta = PROCESO_INICIADO;
		librocket_enviarMensaje(programaSocket, &respuesta,
				sizeof(PROTOCOLO_KERNEL_A_CONSOLA), infoLogger);
		librocket_enviarMensaje(programaSocket, &(pcb->PID), sizeof(int32_t),
				infoLogger);

		//ahora tengo que asignarle las paginas correspondientes pidiendoselas a memoria
		int32_t resultadoAsignacionPaginasIniciales =
				pedirAMemoriaInicializacionDeProceso(&pcb, codigoPrograma);
		if (resultadoAsignacionPaginasIniciales
				== -2|| resultadoAsignacionPaginasIniciales == FAILURE) {
			//no hay memoria o no se creo por algun error
			pcb->EC = NO_SE_PUDO_ASIGNAR_RECURSO;
			pasarPcbDeListoAFinalizados(&pcb);
			eliminarPidProgramaSocket(programaSocket);

			respuesta = PROCESO_RECHAZADO;
			librocket_enviarMensaje(programaSocket, &respuesta,
					sizeof(PROTOCOLO_KERNEL_A_CONSOLA), infoLogger);
			librocket_enviarMensaje(programaSocket, &(pcb->EC), sizeof(int32_t),
					infoLogger);
			free(codigoPrograma);
		} else {
			//el proceso ya esta con sus pagInas en memoria
			//INFORMO A CONSOLA QUE RECIBIO PAGINAS
			respuesta = PROCESO_RECIBIO_PAGINAS_INICIALES;
			librocket_enviarMensaje(programaSocket, &respuesta,
					sizeof(PROTOCOLO_KERNEL_A_CONSOLA), infoLogger);
			//MANDO EL CODIGO A LA MEMORIA
			enviarCodigoDePrograma(pcb->PID, codigoPrograma);

			t_metadata_program* metadata = metadata_desde_literal(
					codigoPrograma);

			pcb->instruccion_inicio = metadata->instruccion_inicio;
			pcb->instrucciones_size = metadata->instrucciones_size;
			pcb->PC = pcb->instruccion_inicio;

			int32_t contador_instrucciones;
			for (contador_instrucciones = 0;
					contador_instrucciones < pcb->instrucciones_size;
					contador_instrucciones++) {

				t_intructions instruccion =
						metadata->instrucciones_serializado[contador_instrucciones];

				t_intructions* instruccionParaAgregar = malloc(
						sizeof(t_intructions));

				instruccionParaAgregar->offset = instruccion.offset;
				instruccionParaAgregar->start = instruccion.start;

				list_add(pcb->indice_codigo, instruccionParaAgregar);

			}

			pcb->etiquetas_size = metadata->etiquetas_size;

			char* bufferEtiquetas = malloc(metadata->etiquetas_size);
			memcpy(bufferEtiquetas, metadata->etiquetas,
					metadata->etiquetas_size);
			pcb->indice_etiquetas = bufferEtiquetas;
			pcb->cantidad_de_funciones = metadata->cantidad_de_funciones;
			pcb->cantidad_de_etiquetas = metadata->cantidad_de_etiquetas;

			metadata_destruir(metadata);

			free(codigoPrograma);

		}

	} else {
		/*
		 * PASA A COLA DE NEW POR NIVEL DE MULTIPROG.
		 * El proceso se mantiene en la cola de Nuevos hasta que pueda entrar a listos
		 * No almaceno el codigo del programa en el PCB, mas tarde deberia pedirlo a la consola
		 */
		t_pcb* pcb = crearPcb();
		list_add(colaNuevos, pcb);
		entrada_programaSocket_pid* entrada = malloc(
				sizeof(entrada_programaSocket_pid));
		entrada->programaSocket = programaSocket;
		entrada->pid = pcb->PID;
		list_add(listaProgramaSocketPid, entrada);
		PROTOCOLO_KERNEL_A_CONSOLA msj = PROCESO_EN_ESPERA;
		librocket_enviarMensaje(programaSocket, &msj,
				sizeof(PROTOCOLO_KERNEL_A_CONSOLA), infoLogger);
		librocket_enviarMensaje(programaSocket, &(pcb->PID), sizeof(int32_t),
				infoLogger);

	}
	if (list_size(colaListos) > 0 && list_size(cpusLibres) > 0) {
		planificar(NULL);
	}
}

void finalizarProceso(int programaSocket) {

}

void enviarMensajeATodosLosProcesos(char* mensaje) {
//envia un texto a cpu memoria y fileSystem
	int longitudMensaje = string_length(mensaje) + 1;

	//FS
	PROTOCOLO_KERNEL_A_FS envioTextoAFS = ENVIO_TEXTO_A_FS;
	librocket_enviarMensaje(fsSocket, &envioTextoAFS, sizeof(int32_t),
			infoLogger);
	librocket_enviarMensaje(fsSocket, &longitudMensaje, sizeof(int32_t),
			infoLogger);
	librocket_enviarMensaje(fsSocket, mensaje, longitudMensaje, infoLogger);

	//memoria
	PROTOCOLO_KERNEL_Y_CPU_A_MEMORIA envioTextoAMemoria = ENVIO_TEXTO_A_MEMORIA;
	librocket_enviarMensaje(memoriaSocket, &envioTextoAMemoria, sizeof(int32_t),
			infoLogger);
	librocket_enviarMensaje(memoriaSocket, &longitudMensaje, sizeof(int32_t),
			infoLogger);
	librocket_enviarMensaje(memoriaSocket, mensaje, longitudMensaje,
			infoLogger);

	//CPUS
	int cantidadCpus = list_size(cpusLibres);
	int i = 0;
	for (; i < cantidadCpus; i++) {
		CPU* cpu = list_get(cpusLibres, i);
		int cpuSocket = cpu->socket;
		PROTOCOLO_KERNEL_A_CPU tipoMensaje = ENVIO_TEXTO_A_CPU;
		librocket_enviarMensaje(cpuSocket, &tipoMensaje,
				sizeof(PROTOCOLO_KERNEL_A_CPU), infoLogger);
		librocket_enviarMensaje(cpuSocket, &longitudMensaje, sizeof(int32_t),
				infoLogger);
		librocket_enviarMensaje(cpuSocket, mensaje, longitudMensaje,
				infoLogger);
	}

	//CPUS ocupadas
	cantidadCpus = list_size(cpusOcupadas);
	i = 0;
	for (; i < cantidadCpus; i++) {
		CPU* cpu = list_get(cpusLibres, i);
		int cpuSocket = cpu->socket;
		PROTOCOLO_KERNEL_A_CPU tipoMensaje = ENVIO_TEXTO_A_CPU;
		librocket_enviarMensaje(cpuSocket, &tipoMensaje,
				sizeof(PROTOCOLO_KERNEL_A_CPU), infoLogger);
		librocket_enviarMensaje(cpuSocket, &longitudMensaje, sizeof(int32_t),
				infoLogger);
		librocket_enviarMensaje(cpuSocket, mensaje, longitudMensaje,
				infoLogger);
	}
	free(mensaje);

}

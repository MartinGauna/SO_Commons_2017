#include "headers/ejecucion.h"

void ejecutarPrograma() {

	pcbActual->banderas.cambio_proceso = false;

	while (!pcbActual->banderas.termino_programa
			&& !pcbActual->banderas.cambio_proceso
			&& !pcbActual->banderas.terminacion_anormal) {

		pcbActual->banderas.llamada_a_funcion = false;
		pcbActual->banderas.se_llamo_a_wait = false;

		pedirInstruccionAMemoria(pcbActual->PC);

		analizadorLinea(instruccionActual, &funciones, &kernel_operaciones);

		if (pcbActual->banderas.llamada_sin_retorno)
			volverAlMain(instruccionActual);

		free(instruccionActual);

		if (pcbActual->banderas.termino_programa)
			break;

		if (pcbActual->banderas.terminacion_anormal)
			break;

		if (!pcbActual->banderas.se_llamo_a_wait)
			termine_sentencia();

		if (!pcbActual->banderas.llamada_a_funcion)
			pcbActual->PC++;

	}

	if (pcbActual->banderas.desconexion && pcbActual->banderas.cambio_proceso) {
		pthread_mutex_unlock(&semaforoDesconexion);
		return;
	}

	if (pcbActual->banderas.terminacion_anormal) {
		log_info(infoEjecucion, "Se aborta el programa con PID: %i EC: %i.",
				pcbActual->PID, pcbActual->EC);
		PROTOCOLO_CPU_A_KERNEL mensaje = ABORTAR_PROGRAMA;
		librocket_enviarMensaje(kernelSocket, &mensaje,
				sizeof(PROTOCOLO_CPU_A_KERNEL), infoLogger);
	}

	pcbActual->banderas.ejecutando = false;

	enviarPcb(kernelSocket, pcbActual, infoLogger);
	destruir_pcb();

	pcb_ejecutando = false;

}

void termine_sentencia() {

	PROTOCOLO_CPU_A_KERNEL termineSentencia = TERMINE_SENTENCIA;
	PROTOCOLO_KERNEL_A_CPU respuestaKernel;

	librocket_enviarMensaje(kernelSocket, &termineSentencia,
			sizeof(PROTOCOLO_CPU_A_KERNEL), infoLogger);

	librocket_recibirMensaje(kernelSocket, &respuestaKernel,
			sizeof(PROTOCOLO_KERNEL_A_CPU), infoLogger);

	if (respuestaKernel == SEGUI_EJECUTANDO) {
		log_info(infoEjecucion, "Sigue la ejecucion del proceso con PID: %i.",
				pcbActual->PID);
	} else if (respuestaKernel == CAMBIAR_PROCESO) {
		log_info(infoEjecucion, "Se cambio el proceso con el PID: %i.",
				pcbActual->PID);
		pcbActual->banderas.cambio_proceso = true;
	}

}

void volverAlMain(char* instruccionEnd) {

	char* instruccionLimpia = limpiarNombre(instruccionEnd);

	if (string_contains(instruccionLimpia, "end")) {

		pcbActual->banderas.llamada_sin_retorno = false;
		pcbActual->banderas.termino_programa = false;

		t_registro_stack* registroActual = (t_registro_stack*) list_remove(
				pcbActual->indice_stack,
				pcbActual->cant_entradas_indice_stack - 1);

		pcbActual->cant_entradas_indice_stack--;
		pcbActual->PC = registroActual->pos_retorno;

		void destruirElemento(void* data) {
			free(data);
		}
		list_destroy_and_destroy_elements(registroActual->argumentos,
				destruirElemento);
		list_destroy_and_destroy_elements(registroActual->variables,
				destruirElemento);
		free(registroActual);

	}

}

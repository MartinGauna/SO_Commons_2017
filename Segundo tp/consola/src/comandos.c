#include "headers/comandos.h"

void crearDiccionarioComandos() {

	comandos = dictionary_create();

	dictionary_put(comandos, "INICIAR", (void*) INICIAR);
	dictionary_put(comandos, "FINALIZAR", (void*) FINALIZAR);
	dictionary_put(comandos, "DESCONECTAR", (void*) DESCONECTAR);
	dictionary_put(comandos, "LIMPIAR", (void*) LIMPIAR);
	dictionary_put(comandos, "HELP", (void*) HELP);

}

void iniciarPrograma(char* path) {

	t_programa* nuevoPrograma = malloc(sizeof(t_programa));

	nuevoPrograma->pid = 0;
	nuevoPrograma->tamanioCodigo = 0;
	nuevoPrograma->kernelSocket = -1;

	nuevoPrograma->codigo = string_new();
	nuevoPrograma->path = string_new();
	string_append(&(nuevoPrograma->path), path);

	nuevoPrograma->impresiones = 0;
	nuevoPrograma->time_init = string_new();
	nuevoPrograma->time_fin = string_new();
	nuevoPrograma->time_total = string_new();

	nuevoPrograma->time_init = temporal_get_string_time();

	if (access(nuevoPrograma->path, F_OK) == -1) {
		perror("NO EXISTE EL ARCHIVO!!!!");
		return;
	}

	pthread_create(&nuevoPrograma->hiloPrograma, NULL, enviarAEjecutarPrograma,
			nuevoPrograma);

	resultadoComando = 0;

}

void finalizarPrograma(char* parametro) {

	int32_t pidAFinalizar = atoi(parametro);

	// Funcion Local
	bool contienePid(void* programa) {

		t_programa* programaFinalizar = programa;

		return (programaFinalizar->pid == pidAFinalizar);

	}

	log_info(infoLogger, "SE LOCKEO EL SEMAFORO: semListaProgramas");
	pthread_mutex_lock(&semListaProgramas);
	t_programa* programaFinalizar = list_remove_by_condition(programas,
			&contienePid);
	log_info(infoLogger, "SE DESLOCKEO EL SEMAFORO: semListaProgramas");
	pthread_mutex_unlock(&semListaProgramas);

	if (programaFinalizar != NULL) {
		PROTOCOLO_CONSOLA_A_KERNEL mensaje = CONSOLA_PIDE_FINALIZAR_PROCESO;
		librocket_enviarMensaje(programaFinalizar->kernelSocket, &mensaje,
				sizeof(PROTOCOLO_CONSOLA_A_KERNEL), infoLogger);

		log_info(infoLogger, "Se elimino el programa con PID: %i.",
				pidAFinalizar);
		printf("Se elimino el programa con PID: %i.\n", pidAFinalizar);

		imprimirFinPrograma(programaFinalizar);
		pthread_cancel(programaFinalizar->hiloPrograma);
		free(programaFinalizar->time_fin);
		free(programaFinalizar->time_init);
		free(programaFinalizar->time_total);
		free(programaFinalizar->path);
		free(programaFinalizar->codigo);
		free(programaFinalizar);

		resultadoComando = 0;
	}

}

void desconectarConsola() {

	if (!list_is_empty(programas)) {

		eliminarProgramasEnEjecucion();

	} else {

		log_info(infoLogger,
				"Se solicita desconectar la consola, no hay programas en ejecucion.");

	}

	liberarMemoria();

	resultadoComando = 1;

}

void limpiarMensajes() {

	printf("\e[2J\e[H");

	resultadoComando = 0;

}

void help() {

	printf("Comandos posibles: \n");
	printf("\tiniciar [PATH] - Inicia un programa con un path especifico.\n");
	printf("\tfinalizar [PID] - Finaliza el programa con su PID.\n");
	printf("\tdesconectar - Desconecta la consola.\n");
	printf("\tlimpiar - Limpia los mensajes anteriores.\n");
	printf("\thelp - Muestra esta informacion.\n");

	resultadoComando = 0;

}

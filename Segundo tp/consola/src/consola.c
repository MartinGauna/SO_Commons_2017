#include "headers/consola.h"

int main(int argc, char* argv[]) {

	iniciarConsola(argc, argv);

	recibirComandos();

	return EXIT_SUCCESS;

}

void iniciarConsola(int argc, char* argv[]) {

	infoLogger = log_create(LOG_PATH, "consola", false, LOG_LEVEL_INFO);
	info_ejecucion = log_create(LOG_EJECUCION, "ejecucion", false,
			LOG_LEVEL_INFO);

	if (argc < 2) {
		printf("No se paso por parametro el path de config.\n");
		log_info(infoLogger, "No se paso por parametro el path de config.");
		exit(EXIT_FAILURE);
	}

	cargarConfig(argv);

	programas = list_create();

	crearDiccionarioComandos();

	pthread_mutex_init(&semListaProgramas, NULL);

	printf("Consola inicializada perfectamente.\n");
	log_info(infoLogger, "Consola inicializada perfectamente.");

}

void cargarConfig(char** argv) {

	consola_config = malloc(sizeof(t_consola_config));
	consola_config->ipKernel = string_new();
	consola_config->puertoKernel = string_new();

	char* path = string_new();

	string_append(&path, argv[1]);

	t_config* config = config_create(path);

	string_append(&(consola_config->ipKernel),
			config_get_string_value(config, "IP_KERNEL"));
	string_append(&(consola_config->puertoKernel),
			config_get_string_value(config, "PUERTO_KERNEL"));

	printf("IP Kernel: %s\n", consola_config->ipKernel);
	printf("Puerto Kernel: %s\n", consola_config->puertoKernel);
	log_info(infoLogger, "IP Kernel: %s", consola_config->ipKernel);
	log_info(infoLogger, "Puerto Kernel: %s", consola_config->puertoKernel);

	free(path);
	config_destroy(config);

}

void liberarMemoria() {

	free(consola_config->puertoKernel);
	free(consola_config->ipKernel);
	free(consola_config);

	if (list_is_empty(programas)) {
		list_destroy(programas);
	} else {
		list_destroy_and_destroy_elements(programas, eliminarPrograma);
	}

	dictionary_destroy(comandos);
	pthread_mutex_destroy(&semListaProgramas);
	log_destroy(infoLogger);
	log_destroy(info_ejecucion);

}

void eliminarPrograma(void* elemento) {

	t_programa* programa = elemento;

	free(programa->time_fin);
	free(programa->time_init);
	free(programa->time_total);
	free(programa->codigo);
	free(programa->path);
	free(elemento);

}

void conectarseAlKernel(t_programa* programa) {

	libconnections_conectar_a_servidor(consola_config->ipKernel,
			consola_config->puertoKernel, &programa->kernelSocket);

	handshake(programa->kernelSocket);

}

void handshake(int32_t kernelSocket) {

	PROTOCOLO_CONSOLA_A_KERNEL handshakeConsola =
			HANDSHAKE_CONECTAR_CONSOLA_A_KERNEL;

	librocket_enviarMensaje(kernelSocket, &handshakeConsola,
			sizeof(PROTOCOLO_CONSOLA_A_KERNEL), infoLogger);

	PROTOCOLO_KERNEL_A_CONSOLA handshakeKernel;

	int32_t resultado = librocket_recibirMensaje(kernelSocket, &handshakeKernel,
			sizeof(PROTOCOLO_KERNEL_A_CONSOLA), infoLogger);

	if (resultado == 0) {
		log_info(infoLogger, "Se cayo el Kernel.");
		exit(EXIT_FAILURE);
	}

	if (handshakeKernel == HANDSHAKE_CONEXION_DE_CONSOLA_A_KERNEL_ACEPTADA) {
		printf("Conectado al kernel.\n");
		log_info(infoLogger, "Conectado al kernel.");
	} else {
		printf("No me pude conectar al kernel.\n");
		log_info(infoLogger, "No me pude conectar al kernel.");
	}

}

int enviarTextoAlKernel(int32_t kernelSocket) {

	char * textoEntrada = malloc(100);
	printf("Escribi algo: \n");

	while (1) {
		fgets(textoEntrada, 100, stdin);

		if (string_equals_ignore_case(textoEntrada, "q\n")) {
			free(textoEntrada);
			return EXIT_SUCCESS;
		} else {
			PROTOCOLO_CONSOLA_A_KERNEL tipoMensaje = ENVIO_TEXTO_CONSOLA;
			int32_t tamanioTexto = string_length(textoEntrada);
			char textoAenviar[100];
			int32_t i = 0;
			char c = textoEntrada[i];
			while (c != '\n') {
				i++;
				c = textoEntrada[i];
			}
			memcpy(textoAenviar, textoEntrada, i);
			textoAenviar[i] = '\0';
			tamanioTexto = string_length(textoAenviar) + 1;

			librocket_enviarMensaje(kernelSocket, &tipoMensaje,
					sizeof(PROTOCOLO_CONSOLA_A_KERNEL), infoLogger);
			librocket_enviarMensaje(kernelSocket, &tamanioTexto,
					sizeof(int32_t), infoLogger);
			librocket_enviarMensaje(kernelSocket, textoAenviar, tamanioTexto,
					infoLogger);
		}

	}
}

void recibirComandos() {

	help();
	resultadoComando = 0;

	while (resultadoComando == 0) {

		printf("Ingrese un nuevo comando: \n");
		char* entradaEnCrudo = malloc(100);
		char** vectorEntrada;
		char* parametro = string_new();
		char* comando = string_new();

		fgets(entradaEnCrudo, 100, stdin);

		if (string_contains(entradaEnCrudo, " ")) {
			vectorEntrada = string_split(entradaEnCrudo, " ");
			string_append(&parametro,
					string_substring_until(vectorEntrada[1],
							string_length(vectorEntrada[1]) - 1));
			string_append(&comando, vectorEntrada[0]);
		} else {
			string_append(&comando,
					string_substring_until(entradaEnCrudo,
							string_length(entradaEnCrudo) - 1));
		}

		string_to_upper(comando);

		log_info(infoLogger, "Se ingreso el comando: %s con el parametro: %s.",
				comando, parametro);

		if (dictionary_has_key(comandos, comando)) {

			PROTOCOLO_COMANDOS_CONSOLA tipoComando =
					(PROTOCOLO_COMANDOS_CONSOLA) dictionary_get(comandos,
							comando);

			switch (tipoComando) {

			case INICIAR:

				iniciarPrograma(parametro);
				break;

			case FINALIZAR:

				finalizarPrograma(parametro);

				break;

			case DESCONECTAR:

				desconectarConsola();

				break;

			case LIMPIAR:

				limpiarMensajes();
				break;

			case HELP:

				help();
				break;

			default:

				break;

			}
		} else {

			printf("'%s' No es un comando valido.\n", comando);

		}

		free(entradaEnCrudo);
		free(parametro);
		free(comando);

	}

}

void* enviarAEjecutarPrograma(void* nuevoPrograma) {

	t_programa* programa = nuevoPrograma;

	conectarseAlKernel(programa);

	PROTOCOLO_CONSOLA_A_KERNEL mensajeIniciarProceso =
			CONSOLA_PIDE_INICIAR_PROCESO;

	librocket_enviarMensaje(programa->kernelSocket, &mensajeIniciarProceso,
			sizeof(PROTOCOLO_CONSOLA_A_KERNEL), infoLogger);

	contarCaracteresYObtenerCodigo(programa);

	librocket_enviarMensaje(programa->kernelSocket, &(programa->tamanioCodigo),
			sizeof(int32_t), infoLogger);

	librocket_enviarMensaje(programa->kernelSocket, programa->codigo,
			(programa->tamanioCodigo), infoLogger);

	PROTOCOLO_KERNEL_A_CONSOLA respuestaDelKernel;

	librocket_recibirMensaje(programa->kernelSocket, &respuestaDelKernel,
			sizeof(PROTOCOLO_KERNEL_A_CONSOLA), infoLogger);

	if (respuestaDelKernel == PROCESO_INICIADO) {

		printf("Se inicio el proceso en el primer intento.\n");
		log_info(infoLogger, "Se inicio el proceso en el primer intento.");

	} else if (respuestaDelKernel == PROCESO_EN_ESPERA) {

		printf("El proceso quedo en la cola de NEW en espera de ejecucion.\n");
		log_info(infoLogger,
				"El proceso quedo en la cola de NEW en espera de ejecucion.");

	}

	librocket_recibirMensaje(programa->kernelSocket, &(programa->pid),
			sizeof(int32_t), infoLogger);

	log_info(infoLogger, "SE LOCKEO EL SEMAFORO: semListaProgramas");
	pthread_mutex_lock(&semListaProgramas);
	list_add(programas, programa);
	log_info(infoLogger, "SE DESLOCKEO EL SEMAFORO: semListaProgramas");
	pthread_mutex_unlock(&semListaProgramas);
	programaEjecutando(programa);

	return NULL;

}

void contarCaracteresYObtenerCodigo(t_programa* programa) {

	FILE* archivo;
	int32_t tamanio;
	char* buffer;

	archivo = fopen(programa->path, "rb");
	if (!archivo)
		perror("Error abriendo archivo del programa.\n"), exit(1);

	fseek(archivo, 0L, SEEK_END);
	tamanio = ftell(archivo);
	rewind(archivo);

	buffer = malloc(tamanio + 1);
	if (!buffer)
		fclose(archivo), fputs("memory alloc fails", stderr), exit(1);

	if (1 != fread(buffer, tamanio, 1, archivo))
		fclose(archivo), free(buffer), fputs("entire read fails", stderr), exit(
				1);

	buffer[tamanio] = '\0';
	string_append(&(programa->codigo), buffer);
	programa->tamanioCodigo = string_length(programa->codigo) + 1;

	fclose(archivo);
	free(buffer);

}

void programaEjecutando(t_programa* programa) {

	/*
	 * Empiezo a escuchar los mensajes que envie el kernel
	 * aca el kernel primero me va a decir si se le asignaron las paginas o no al proceso si entro como listo
	 * Si entro como NEW me va a decir primero que paso a listo y le tengo que enviar el codigo del programa
	 * luego comenzara a mandar las impresiones a pantalla que haga el proceso
	 */
	PROTOCOLO_KERNEL_A_CONSOLA mensaje;

	while (1) {

		int32_t resultado = librocket_recibirMensaje(programa->kernelSocket,
				&mensaje, sizeof(PROTOCOLO_KERNEL_A_CONSOLA), infoLogger);

		if (resultado == 0) {
			log_info(infoLogger, "Se cayo el Kernel.");
			exit(EXIT_FAILURE);
		}

		if (mensaje == PROCESO_PASO_DE_NEW_A_READY) {

			librocket_enviarMensaje(programa->kernelSocket,
					&programa->tamanioCodigo, sizeof(int32_t), infoLogger);
			librocket_enviarMensaje(programa->kernelSocket, programa->codigo,
					programa->tamanioCodigo, infoLogger);

			printf("El proceso de PID: %i, paso de NEW a READY.\n",
					programa->pid);
			log_info(infoLogger, "El proceso de PID: %i, paso de NEW a READY.",
					programa->pid);

		} else if (mensaje == PROCESO_RECIBIO_PAGINAS_INICIALES) {

			printf("El proceso de PID: %i, obtuvo sus paginas iniciales.\n",
					programa->pid);
			log_info(infoLogger,
					"El proceso de PID: %i, obtuvo sus paginas iniciales.",
					programa->pid);

		} else if (mensaje == PROCESO_RECHAZADO) {

			EXIT_CODE_TYPE motivoRechazo;
			librocket_recibirMensaje(programa->kernelSocket, &motivoRechazo,
					sizeof(EXIT_CODE_TYPE), infoLogger);

			printf("Se rechazo el proceso en el sistema con EXIT CODE: %d.\n",
					motivoRechazo);
			log_info(infoLogger,
					"Se rechazo el proceso en el sistema con EXIT CODE: %d.",
					motivoRechazo);

			finalizarProcesoQueTerminoDeEjecutar(programa->pid, motivoRechazo);

			break;

		}

		else if (mensaje == NUEVA_IMPRESION) {

			impresionPrograma(programa);

		}

		else if (mensaje == PROCESO_TERMINADO) {

			EXIT_CODE_TYPE exitCode;
			librocket_recibirMensaje(programa->kernelSocket, &exitCode,
					sizeof(EXIT_CODE_TYPE), infoLogger);

			finalizarProcesoQueTerminoDeEjecutar(programa->pid, exitCode);

			break;

		}
	}

}

void impresionPrograma(t_programa* programa) {

	PROTOCOLO_CONSOLA_A_KERNEL resultadoImpresion;

	printf("Se solicito la impresion del programa con PID: %i SOCKET: %i\n",
			programa->pid, programa->kernelSocket);
	log_info(infoLogger,
			"Se solicito la impresion del programa con PID: %i SOCKET: %i.",
			programa->pid, programa->kernelSocket);

	t_valor_variable tamanio;

	librocket_recibirMensaje(programa->kernelSocket, &tamanio,
			sizeof(t_valor_variable), infoLogger);

	void* informacion = malloc(tamanio);
	librocket_recibirMensaje(programa->kernelSocket, informacion, tamanio,
			infoLogger);

	char* resultado = (char*) informacion;
	resultado[tamanio] = '\0';
	printf("Impresion: %s\n", resultado);

	if (informacion != NULL) {
		resultadoImpresion = IMPRESION_EXITOSA;
		programa->impresiones++;
	} else {
		resultadoImpresion = IMPRESION_FALLIDA;
	}

	librocket_enviarMensaje(programa->kernelSocket, &resultadoImpresion,
			sizeof(PROTOCOLO_CONSOLA_A_KERNEL), infoLogger);

	free(informacion);

}

void imprimirFinPrograma(t_programa* programa) {

	programa->time_fin = temporal_get_string_time();
	programa->time_total = calcular(programa->time_init, programa->time_fin);

	log_info(info_ejecucion, "=====INFORMACION FIN DE PROCESO=====");
	log_info(info_ejecucion, "PID: %i SOCKET: %i", programa->pid,
			programa->kernelSocket);
	log_info(info_ejecucion, "Inicio de ejecucion: %s", programa->time_init);
	log_info(info_ejecucion, "Fin de ejecucion: %s", programa->time_fin);
	log_info(info_ejecucion, "Impresiones por pantalla: %i",
			programa->impresiones);
	log_info(info_ejecucion, "Tiempo total de ejecucion: %s",
			programa->time_total);
	log_info(info_ejecucion, "====================================");

}

void eliminarProgramasEnEjecucion() {

	int32_t contador;
	int32_t cantidadDeProgramas = list_size(programas);
	for (contador = 0; contador < cantidadDeProgramas; contador++) {

		log_info(infoLogger, "SE LOCKEO EL SEMAFORO: semListaProgramas");
		pthread_mutex_lock(&semListaProgramas);
		t_programa* programaAEliminar = list_remove(programas, 0);
		log_info(infoLogger, "SE DESLOCKEO EL SEMAFORO: semListaProgramas");
		pthread_mutex_unlock(&semListaProgramas);

		imprimirFinPrograma(programaAEliminar);

		pthread_cancel(programaAEliminar->hiloPrograma);
		free(programaAEliminar->time_fin);
		free(programaAEliminar->time_init);
		free(programaAEliminar->time_total);
		free(programaAEliminar->path);
		free(programaAEliminar->codigo);
		free(programaAEliminar);

	}

}

void finalizarProcesoQueTerminoDeEjecutar(int32_t pid, EXIT_CODE_TYPE exitCode) {

	// Funcion Local
	bool contienePid(void* programa) {

		t_programa* programaFinalizar = programa;

		return (programaFinalizar->pid == pid);

	}

	log_info(infoLogger, "SE LOCKEO EL SEMAFORO: semListaProgramas");
	pthread_mutex_lock(&semListaProgramas);
	t_programa* programaFinalizar = list_remove_by_condition(programas,
			&contienePid);
	log_info(infoLogger, "SE DESLOCKEO EL SEMAFORO: semListaProgramas");
	pthread_mutex_unlock(&semListaProgramas);

	if (programaFinalizar != NULL) {
		log_info(infoLogger, "Se elimino el programa con PID: %i.", pid);
		printf("Se termino el proceso %d con exit code: %d \n", pid, exitCode);

		imprimirFinPrograma(programaFinalizar);
		pthread_cancel(programaFinalizar->hiloPrograma);
		free(programaFinalizar->time_fin);
		free(programaFinalizar->time_init);
		free(programaFinalizar->time_total);
		free(programaFinalizar->path);
		free(programaFinalizar->codigo);
		free(programaFinalizar);

	}

}

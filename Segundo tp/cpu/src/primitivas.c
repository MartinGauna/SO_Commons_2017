#include "headers/primitivas.h"

void inicializarPrimitivas() {

	// Funciones
	funciones = (AnSISOP_funciones ) {

			.AnSISOP_definirVariable = funciones_definirVariables,
					.AnSISOP_obtenerPosicionVariable =
							funciones_obtenerPosicionVariable,
					.AnSISOP_dereferenciar = funciones_dereferenciar,
					.AnSISOP_asignar = funciones_asignar,
					.AnSISOP_obtenerValorCompartida =
							funciones_obtenerValorCompartida,
					.AnSISOP_asignarValorCompartida =
							funciones_asignarValorCompartida,
					.AnSISOP_irAlLabel = funciones_irAlLabel,
					.AnSISOP_llamarSinRetorno = funciones_llamarSinRetorno,
					.AnSISOP_llamarConRetorno = funciones_llamarConRetorno,
					.AnSISOP_finalizar = funciones_finalizar,
					.AnSISOP_retornar = funciones_retornar

			};

	// Operaciones Kernel
	kernel_operaciones = (AnSISOP_kernel ) {

			.AnSISOP_wait = operaciones_kernel_wait, .AnSISOP_signal =
					operaciones_kernel_signal, .AnSISOP_reservar =
					operaciones_kernel_reservar, .AnSISOP_liberar =
					operaciones_kernel_liberar, .AnSISOP_abrir =
					operaciones_kernel_abrir, .AnSISOP_borrar =
					operaciones_kernel_borrar, .AnSISOP_cerrar =
					operaciones_kernel_cerrar, .AnSISOP_moverCursor =
					operaciones_kernel_moverCursor, .AnSISOP_escribir =
					operaciones_kernel_escribir, .AnSISOP_leer =
					operaciones_kernel_leer

			};

}

// Funciones
t_puntero funciones_definirVariables(t_nombre_variable identificador_variable) {

	log_info(infoEjecucion, "Se llamo a la primitiva: DEFINIR VARIABLES.");
	log_info(infoEjecucion, "Identificador variable: %c.",
			identificador_variable);

	if (pcbActual->banderas.terminacion_anormal)
		return -1;

	if (pcbActual->fin_stack.paginaId == pcbActual->cantidad_paginas_totales) {
		pcbActual->EC = STACKOVERFLOW;
		pcbActual->banderas.terminacion_anormal = true;
		return -1;
	}

	t_registro_stack* registroActual = (t_registro_stack*) list_get(
			pcbActual->indice_stack, pcbActual->cant_entradas_indice_stack - 1);

	t_puntero punteroMemoria;

	if (isalpha(identificador_variable)) {

		t_variable* nueva_variable = malloc(sizeof(t_variable));

		nueva_variable->id = identificador_variable;
		nueva_variable->posicion = pcbActual->fin_stack;

		list_add(registroActual->variables, nueva_variable);

		punteroMemoria = posicionMemoria_a_puntero(nueva_variable->posicion,
				tamanioPagina);

	}

	if (isdigit(identificador_variable)) {

		t_direccion_logica* nuevo_argumento = malloc(
				sizeof(t_direccion_logica));

		nuevo_argumento->offset = pcbActual->fin_stack.offset;
		nuevo_argumento->paginaId = pcbActual->fin_stack.paginaId;
		nuevo_argumento->pid = pcbActual->fin_stack.pid;
		nuevo_argumento->tamanio = pcbActual->fin_stack.tamanio;

		list_add(registroActual->argumentos, nuevo_argumento);

		punteroMemoria = posicionMemoria_a_puntero(pcbActual->fin_stack,
				tamanioPagina);

	}

	pcbActual->fin_stack.offset += 4;

	if (pcbActual->fin_stack.offset >= tamanioPagina) {
		pcbActual->fin_stack.offset = pcbActual->fin_stack.offset
				- tamanioPagina;
		pcbActual->fin_stack.paginaId++;
	}

	return punteroMemoria;

}

t_puntero funciones_obtenerPosicionVariable(
		t_nombre_variable identificador_variable) {

	log_info(infoEjecucion,
			"Se llamo a la primitiva: OBTENER POSICION VARIABLE.");
	log_info(infoEjecucion, "Identificador variable: %c.",
			identificador_variable);

	if (pcbActual->banderas.terminacion_anormal)
		return -1;

	t_registro_stack* registroActual = (t_registro_stack*) list_get(
			pcbActual->indice_stack, pcbActual->cant_entradas_indice_stack - 1);

	t_puntero punteroMemoria;

	if (isalpha(identificador_variable)) {

		int32_t contador;
		t_variable* variableBuscada;

		for (contador = 0; contador < list_size(registroActual->variables);
				contador++) {
			variableBuscada = list_get(registroActual->variables, contador);
			if (variableBuscada->id == identificador_variable)
				break;
		}

		punteroMemoria = posicionMemoria_a_puntero(variableBuscada->posicion,
				tamanioPagina);

	}

	if (isdigit(identificador_variable)) {

		int32_t numeroArgumento = identificador_variable - '0';
		t_direccion_logica* argumentoBuscado = list_get(
				registroActual->argumentos, numeroArgumento);

		t_direccion_logica argumento;

		argumento.offset = argumentoBuscado->offset;
		argumento.paginaId = argumentoBuscado->paginaId;
		argumento.pid = argumentoBuscado->pid;
		argumento.tamanio = argumentoBuscado->tamanio;

		punteroMemoria = posicionMemoria_a_puntero(argumento, tamanioPagina);

	}

	return punteroMemoria;

}

t_valor_variable funciones_dereferenciar(t_puntero direccion_variable) {

	log_info(infoEjecucion, "Se llamo a la primitiva: DEREFERENCIAR.");
	log_info(infoEjecucion, "Direccion variable: %i.", direccion_variable);

	if (pcbActual->banderas.terminacion_anormal)
		return -1;

	t_direccion_logica direccionLogica = puntero_a_posicionMemoria(
			direccion_variable, tamanioPagina, pcbActual->PID);

	return pedido_de_lectura_a_memoria(direccionLogica);

}

void funciones_asignar(t_puntero direccion_variable, t_valor_variable valor) {

	log_info(infoEjecucion, "Se llamo a la primitiva: ASIGNAR.");
	log_info(infoEjecucion, "Direccion variable: %i.", direccion_variable);
	log_info(infoEjecucion, "Valor variable: %i.", valor);

	if (pcbActual->banderas.terminacion_anormal)
		return;

	t_direccion_logica direccionLogica = puntero_a_posicionMemoria(
			direccion_variable, tamanioPagina, pcbActual->PID);

	pedido_de_asignacion_a_memoria(direccionLogica, valor);

}

t_valor_variable funciones_obtenerValorCompartida(t_nombre_compartida variable) {

	log_info(infoEjecucion,
			"Se llamo a la primitiva: OBTENER VALOR COMPARTIDA.");

	if (pcbActual->banderas.terminacion_anormal)
		return -1;

	t_valor_variable valor_compartida;
	PROTOCOLO_CPU_A_KERNEL mensaje = OBTENER_VALOR_COMPARTIDA;
	PROTOCOLO_KERNEL_A_CPU respuesta;

	char* nombreLimpio = limpiarNombre(variable);
	int32_t tamanio = strlen(nombreLimpio);
	tamanio++;

	log_info(infoEjecucion, "Nombre compartida: %s.", nombreLimpio);

	librocket_enviarMensaje(kernelSocket, &mensaje,
			sizeof(PROTOCOLO_CPU_A_KERNEL), infoLogger);
	librocket_enviarMensaje(kernelSocket, &tamanio, sizeof(int32_t),
			infoLogger);
	librocket_enviarMensaje(kernelSocket, nombreLimpio, tamanio, infoLogger);

	int resultadoRespuesta = librocket_recibirMensaje(kernelSocket, &respuesta,
			sizeof(PROTOCOLO_KERNEL_A_CPU), infoLogger);

	if (resultadoRespuesta == 0) {
		log_info(infoLogger, "Se cayo el Kernel.\n");
		exit(EXIT_FAILURE);
	}

	if (respuesta == COMPARTIDA_INEXISTENTE) {
		log_info(infoEjecucion,
				"Se intento acceder a una variable compartida inexistente.");
		pcbActual->EC = INTENTO_ACCEDER_A_UNA_VARIABLE_COMPARTIDA_INEXISTENTE;
		pcbActual->banderas.terminacion_anormal = true;
		free(nombreLimpio);
		return -1;
	} else if (respuesta == ENVIADO_VALOR_COMPARTIDA) {
		librocket_recibirMensaje(kernelSocket, &valor_compartida,
				sizeof(t_valor_variable), infoLogger);

	}

	free(nombreLimpio);

	return valor_compartida;

}

t_valor_variable funciones_asignarValorCompartida(t_nombre_compartida variable,
		t_valor_variable valor_variable) {

	log_info(infoEjecucion,
			"Se llamo a la primitiva: ASIGNAR VALOR COMPARTIDA.");

	if (pcbActual->banderas.terminacion_anormal)
		return -1;

	t_valor_variable valorAsignado;
	PROTOCOLO_CPU_A_KERNEL mensaje = ASIGNAR_VALOR_COMPARTIDA;
	PROTOCOLO_KERNEL_A_CPU respuesta;

	char* nombreLimpio = limpiarNombre(variable);
	int32_t tamanio = strlen(nombreLimpio);
	tamanio++;

	log_info(infoEjecucion, "Nombre compartida: %s.", nombreLimpio);
	log_info(infoEjecucion, "Valor: %i.", valor_variable);

	librocket_enviarMensaje(kernelSocket, &mensaje,
			sizeof(PROTOCOLO_CPU_A_KERNEL), infoLogger);
	librocket_enviarMensaje(kernelSocket, &tamanio, sizeof(int32_t),
			infoLogger);
	librocket_enviarMensaje(kernelSocket, nombreLimpio, tamanio, infoLogger);
	librocket_enviarMensaje(kernelSocket, &valor_variable,
			sizeof(t_valor_variable), infoLogger);

	int resultadoRespuesta = librocket_recibirMensaje(kernelSocket, &respuesta,
			sizeof(PROTOCOLO_KERNEL_A_CPU), infoLogger);

	if (resultadoRespuesta == 0) {
		log_info(infoLogger, "Se cayo el Kernel.");
		exit(EXIT_FAILURE);
	}

	if (respuesta == COMPARTIDA_INEXISTENTE) {
		log_info(infoEjecucion,
				"Se intento acceder a una variable compartida inexistente.");
		pcbActual->EC = INTENTO_ACCEDER_A_UNA_VARIABLE_COMPARTIDA_INEXISTENTE;
		pcbActual->banderas.terminacion_anormal = true;
		free(nombreLimpio);
		return -1;
	} else if (respuesta == VALOR_COMPARTIDA_ASIGNADO) {
		librocket_recibirMensaje(kernelSocket, &valorAsignado,
				sizeof(t_valor_variable), infoLogger);
	}

	free(nombreLimpio);

	return valorAsignado;

}

void funciones_irAlLabel(t_nombre_etiqueta etiqueta) {

	log_info(infoEjecucion, "Se llamo a la primitiva: IR A LABEL.");

	if (pcbActual->banderas.terminacion_anormal)
		return;

	pcbActual->banderas.llamada_a_funcion = true;

	char* nombreLimpio = limpiarNombre(etiqueta);
	log_info(infoEjecucion, "Etiqueta: %s.", nombreLimpio);

	pcbActual->PC = metadata_buscar_etiqueta(nombreLimpio,
			pcbActual->indice_etiquetas, pcbActual->etiquetas_size);

	free(nombreLimpio);

}

void funciones_llamarSinRetorno(t_nombre_etiqueta etiqueta) {

	log_info(infoEjecucion, "Se llamo a la primitiva: LLAMAR SIN RETORNO.");

	char* nombreLimpio = limpiarNombre(etiqueta);
	log_info(infoEjecucion, "Etiqueta: %s.", nombreLimpio);
	free(nombreLimpio);

	if (pcbActual->banderas.terminacion_anormal)
		return;

	pcbActual->banderas.llamada_sin_retorno = true;

	pcbActual->cant_entradas_indice_stack++;

	t_registro_stack* nuevoRegistro = malloc(sizeof(t_registro_stack));

	nuevoRegistro->posicion = pcbActual->cant_entradas_indice_stack;
	nuevoRegistro->argumentos = list_create();
	nuevoRegistro->variables = list_create();
	nuevoRegistro->pos_retorno = pcbActual->PC;
	nuevoRegistro->pos_var_retorno = puntero_a_posicionMemoria(0, tamanioPagina,
			pcbActual->PID);

	list_add(pcbActual->indice_stack, nuevoRegistro);

	funciones_irAlLabel(etiqueta);

}

void funciones_llamarConRetorno(t_nombre_etiqueta etiqueta,
		t_puntero donde_retornar) {

	log_info(infoEjecucion, "Se llamo a la primitiva: LLAMAR CON RETORNO.");

	char* nombreLimpio = limpiarNombre(etiqueta);
	log_info(infoEjecucion, "Etiqueta: %s.", nombreLimpio);
	free(nombreLimpio);

	log_info(infoEjecucion, "Donde retornar: %i.", donde_retornar);

	if (pcbActual->banderas.terminacion_anormal)
		return;

	pcbActual->cant_entradas_indice_stack++;

	t_registro_stack* nuevoRegistro = malloc(sizeof(t_registro_stack));

	nuevoRegistro->posicion = pcbActual->cant_entradas_indice_stack;
	nuevoRegistro->argumentos = list_create();
	nuevoRegistro->variables = list_create();
	nuevoRegistro->pos_retorno = pcbActual->PC;
	nuevoRegistro->pos_var_retorno = puntero_a_posicionMemoria(donde_retornar,
			tamanioPagina, pcbActual->PID);

	list_add(pcbActual->indice_stack, nuevoRegistro);

	funciones_irAlLabel(etiqueta);

}

void funciones_finalizar(void) {

	log_info(infoEjecucion, "Se llamo a la primitiva: FINALIZAR.");

	if (pcbActual->banderas.terminacion_anormal)
		return;

	if (pcbActual->banderas.llamada_sin_retorno)
		return;

	pcbActual->banderas.termino_programa = true;

	PROTOCOLO_CPU_A_KERNEL mensajeFinalizar = PROGRAMA_TERMINADO;

	librocket_enviarMensaje(kernelSocket, &mensajeFinalizar,
			sizeof(PROTOCOLO_CPU_A_KERNEL), infoLogger);

	pcbActual->EC = FINALIZADO_CORRECTAMENTE;

}

void funciones_retornar(t_valor_variable retorno) {

	log_info(infoEjecucion, "Se llamo a la primitiva: RETORNAR.");
	log_info(infoEjecucion, "Donde retornar: %i.", retorno);

	if (pcbActual->banderas.terminacion_anormal)
		return;

	t_registro_stack* registroActual = (t_registro_stack*) list_remove(
			pcbActual->indice_stack, pcbActual->cant_entradas_indice_stack - 1);

	t_puntero posicion_fisica = posicionMemoria_a_puntero(
			registroActual->pos_var_retorno, tamanioPagina);

	funciones_asignar(posicion_fisica, retorno);

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

// Operaciones Kernel
void operaciones_kernel_wait(t_nombre_semaforo identificador_semaforo) {

	log_info(infoEjecucion, "Se llamo a la primitiva: WAIT.");

	pcbActual->banderas.se_llamo_a_wait = true;

	if (pcbActual->banderas.terminacion_anormal)
		return;

	PROTOCOLO_CPU_A_KERNEL mensaje = WAIT;
	PROTOCOLO_KERNEL_A_CPU respuesta;

	char* nombreLimpio = limpiarNombre(identificador_semaforo);
	int32_t tamanio = strlen(nombreLimpio);
	tamanio++;

	log_info(infoEjecucion, "Semaforo: %s.", nombreLimpio);

	librocket_enviarMensaje(kernelSocket, &mensaje,
			sizeof(PROTOCOLO_CPU_A_KERNEL), infoLogger);
	librocket_enviarMensaje(kernelSocket, &tamanio, sizeof(int32_t),
			infoLogger);
	librocket_enviarMensaje(kernelSocket, nombreLimpio, tamanio, infoLogger);

	int resultadoRespuesta = librocket_recibirMensaje(kernelSocket, &respuesta,
			sizeof(PROTOCOLO_KERNEL_A_CPU), infoLogger);

	if (resultadoRespuesta == 0) {
		log_info(infoLogger, "Se cayo el Kernel.");
		exit(EXIT_FAILURE);
	}

	if (respuesta == SEMAFORO_INEXISTENTE) {
		log_info(infoEjecucion, "Se intento operar un semaforo inexistente.");
		pcbActual->EC = INTENTO_OPERAR_UN_SEMAFORO_INEXISTENTE;
		pcbActual->banderas.terminacion_anormal = true;
		free(nombreLimpio);
		return;
	} else if (respuesta == SEMAFORO_OPERADO_CORRECTAMENTE) {

		PROTOCOLO_KERNEL_A_CPU queHago;
		librocket_recibirMensaje(kernelSocket, &queHago,
				sizeof(PROTOCOLO_KERNEL_A_CPU), infoLogger);
		if (queHago == SEGUI_EJECUTANDO) {
			log_info(infoEjecucion,
					"Sigue la ejecucion del proceso con PID: %i.",
					pcbActual->PID);
		} else if (queHago == CAMBIAR_PROCESO) {
			log_info(infoEjecucion, "Se cambio el proceso con el PID: %i.",
					pcbActual->PID);
			pcbActual->banderas.cambio_proceso = true;
		}

	}

	free(nombreLimpio);

}

void operaciones_kernel_signal(t_nombre_semaforo identificador_semaforo) {

	log_info(infoEjecucion, "Se llamo a la primitiva: SIGNAL.");

	if (pcbActual->banderas.terminacion_anormal)
		return;

	PROTOCOLO_CPU_A_KERNEL mensaje = SIGNAL;
	PROTOCOLO_KERNEL_A_CPU respuesta;

	char* nombreLimpio = limpiarNombre(identificador_semaforo);
	int32_t tamanio = strlen(nombreLimpio);
	tamanio++;

	log_info(infoEjecucion, "Semaforo: %s.", nombreLimpio);

	librocket_enviarMensaje(kernelSocket, &mensaje,
			sizeof(PROTOCOLO_CPU_A_KERNEL), infoLogger);
	librocket_enviarMensaje(kernelSocket, &tamanio, sizeof(int32_t),
			infoLogger);
	librocket_enviarMensaje(kernelSocket, nombreLimpio, tamanio, infoLogger);

	int32_t resultadoRespuesta = librocket_recibirMensaje(kernelSocket,
			&respuesta, sizeof(PROTOCOLO_KERNEL_A_CPU), infoLogger);

	if (resultadoRespuesta == 0) {
		log_info(infoLogger, "Se cayo el Kernel.\n");
		exit(EXIT_FAILURE);
	}

	if (respuesta == SEMAFORO_INEXISTENTE) {
		log_info(infoEjecucion, "Se intento operar un semaforo inexistente.");
		pcbActual->EC = INTENTO_OPERAR_UN_SEMAFORO_INEXISTENTE;
		pcbActual->banderas.terminacion_anormal = true;
		free(nombreLimpio);
		return;
	} else if (respuesta == SEMAFORO_OPERADO_CORRECTAMENTE) {

	}

	free(nombreLimpio);

}

t_puntero operaciones_kernel_reservar(t_valor_variable espacio) {

	log_info(infoEjecucion, "Se llamo a la primitiva: RESERVAR.");
	log_info(infoEjecucion, "Espacio pedido en bytes: %i.", espacio);

	if (pcbActual->banderas.terminacion_anormal)
		return -1;

	PROTOCOLO_CPU_A_KERNEL pedidoReserva = PEDIDO_BLOQUE_MEMORIA;
	PROTOCOLO_KERNEL_A_CPU respuestaReserva;
	t_puntero posicionMemoriaAsignada;

	librocket_enviarMensaje(kernelSocket, &pedidoReserva,
			sizeof(PROTOCOLO_CPU_A_KERNEL), infoLogger);
	librocket_enviarMensaje(kernelSocket, &pcbActual->PID, sizeof(int32_t),
			infoLogger);
	librocket_enviarMensaje(kernelSocket, &espacio, sizeof(t_valor_variable),
			infoLogger);

	int32_t resultadoRespuesta = librocket_recibirMensaje(kernelSocket,
			&respuestaReserva, sizeof(PROTOCOLO_KERNEL_A_CPU), infoLogger);

	if (resultadoRespuesta == 0) {
		log_info(infoLogger, "Se cayo el Kernel.");
		exit(EXIT_FAILURE);
	}

	if (respuestaReserva == PEDIDO_MEMORIA_RECHAZADO) {
		log_info(infoEjecucion, "Reserva invalida.");
		pcbActual->EC = NO_SE_PUEDEN_DAR_MAS_PAGINAS;
		pcbActual->banderas.terminacion_anormal = true;
		return -1;
	} else if (respuestaReserva == DIRECCION_ENVIADA) {
		librocket_recibirMensaje(kernelSocket, &posicionMemoriaAsignada,
				sizeof(int32_t), infoLogger);
	}

	return posicionMemoriaAsignada;

}

void operaciones_kernel_liberar(t_puntero puntero) {

	log_info(infoEjecucion, "Se llamo a la primitiva: LIBERAR.");
	log_info(infoEjecucion, "Puntero a liberar: %i.", puntero);

	if (pcbActual->banderas.terminacion_anormal)
		return;

	PROTOCOLO_CPU_A_KERNEL pedidoLiberar = LIBERAR_BLOQUE;
	PROTOCOLO_KERNEL_A_CPU respuestaLiberar;

	librocket_enviarMensaje(kernelSocket, &pedidoLiberar,
			sizeof(PROTOCOLO_CPU_A_KERNEL), infoLogger);
	librocket_enviarMensaje(kernelSocket, &pcbActual->PID, sizeof(int32_t),
			infoLogger);
	librocket_enviarMensaje(kernelSocket, &puntero, sizeof(t_valor_variable),
			infoLogger);

	int32_t resultadoRespuesta = librocket_recibirMensaje(kernelSocket,
			&respuestaLiberar, sizeof(PROTOCOLO_KERNEL_A_CPU), infoLogger);

	if (resultadoRespuesta == 0) {
		log_info(infoLogger, "Se cayo el Kernel.");
		exit(EXIT_FAILURE);
	}

	if (respuestaLiberar == LIBERACION_INVALIDA) {
		log_info(infoLogger, "Liberacion invalida.\n");
		pcbActual->EC = EXCEPCION_MEMORIA;
		pcbActual->banderas.terminacion_anormal = true;
		return;
	} else if (respuestaLiberar == LIBERACION_EXITOSA) {

	}

}

t_descriptor_archivo operaciones_kernel_abrir(t_direccion_archivo direccion,
		t_banderas flags) {

	log_info(infoEjecucion, "Se llamo a la primitiva: ABRIR.");
	log_info(infoEjecucion, "Creacion: %i.", flags.creacion);
	log_info(infoEjecucion, "Lectura: %i.", flags.lectura);
	log_info(infoEjecucion, "Escritura: %i.", flags.escritura);

	if (pcbActual->banderas.terminacion_anormal)
		return -1;

	char* path = limpiarNombre(direccion);
	int32_t tamanio = strlen(path);
	tamanio++;

	log_info(infoEjecucion, "Archivo: %s.", path);

	t_descriptor_archivo descriptor_del_archivo_abierto;
	PROTOCOLO_CPU_A_KERNEL pedidoApertura = PRIMITIVA_ABRIR_ARCHIVO;
	PROTOCOLO_KERNEL_A_CPU respuestaApertura;

	librocket_enviarMensaje(kernelSocket, &pedidoApertura,
			sizeof(PROTOCOLO_CPU_A_KERNEL), infoLogger);
	librocket_enviarMensaje(kernelSocket, &pcbActual->PID, sizeof(int32_t),
			infoLogger);
	librocket_enviarMensaje(kernelSocket, &tamanio, sizeof(int32_t),
			infoLogger);
	librocket_enviarMensaje(kernelSocket, path, tamanio, infoLogger);
	librocket_enviarMensaje(kernelSocket, &flags, sizeof(t_banderas),
			infoLogger);

	int32_t resultado = librocket_recibirMensaje(kernelSocket,
			&respuestaApertura, sizeof(PROTOCOLO_KERNEL_A_CPU), infoLogger);

	if (resultado == 0) {
		log_info(infoLogger, "Se cayo el Kernel.");
		exit(EXIT_FAILURE);
	}

	if (respuestaApertura == ARCHIVO_INVALIDO) {
		log_info(infoEjecucion, "Archivo invalido.");
		pcbActual->EC = ACCESO_ARCHIVO_INEXISTENTE;
		pcbActual->banderas.terminacion_anormal = true;
		return -1;
	} else if (respuestaApertura == ESPACIO_INSUFICIENTE_EN_FS) {
		log_info(infoEjecucion, "Espacio insuficiente en Filesystem.");
		pcbActual->EC = ESPACIO_INSUFICIENTE_PARA_CREAR_ARCHIVO;
		pcbActual->banderas.terminacion_anormal = true;
		return -1;
	} else if (respuestaApertura == APERTURA_EXITOSA) {
		log_info(infoEjecucion, "Archivo abierto de manera exitosa.");
		librocket_recibirMensaje(kernelSocket, &descriptor_del_archivo_abierto,
				sizeof(t_descriptor_archivo), infoLogger);
	}

	free(path);

	return descriptor_del_archivo_abierto;

}

void operaciones_kernel_borrar(t_descriptor_archivo direccion) {

	log_info(infoEjecucion, "Se llamo a la primitiva: BORRAR.");
	log_info(infoEjecucion, "Descriptor: %i.", direccion);

	if (pcbActual->banderas.terminacion_anormal)
		return;

	PROTOCOLO_CPU_A_KERNEL pedidoBorrado = PRIMITIVA_BORRAR_ARCHIVO;
	PROTOCOLO_KERNEL_A_CPU respuestaBorrado;

	librocket_enviarMensaje(kernelSocket, &pedidoBorrado,
			sizeof(PROTOCOLO_CPU_A_KERNEL), infoLogger);
	librocket_enviarMensaje(kernelSocket, &pcbActual->PID, sizeof(int32_t),
			infoLogger);
	librocket_enviarMensaje(kernelSocket, &direccion,
			sizeof(t_descriptor_archivo), infoLogger);

	int32_t resultado = librocket_recibirMensaje(kernelSocket,
			&respuestaBorrado, sizeof(PROTOCOLO_KERNEL_A_CPU), infoLogger);

	if (resultado == 0) {
		log_info(infoLogger, "Se cayo el Kernel.");
		exit(EXIT_FAILURE);
	}

	if (respuestaBorrado == BORRADO_ERRONEO) {
		log_info(infoEjecucion, "Archivo invalido.");
		pcbActual->EC = ACCESO_ARCHIVO_INEXISTENTE;
		pcbActual->banderas.terminacion_anormal = true;
		return;
	} else if (respuestaBorrado == BORRADO_EXITOSO) {
		log_info(infoEjecucion, "Archivo borrado de manera exitosa.");
	}

}

void operaciones_kernel_cerrar(t_descriptor_archivo descriptor_archivo) {

	log_info(infoEjecucion, "Se llamo a la primitiva: CERRAR.");
	log_info(infoEjecucion, "Descriptor: %i.", descriptor_archivo);

	if (pcbActual->banderas.terminacion_anormal)
		return;

	PROTOCOLO_CPU_A_KERNEL pedidoCerrado = PRIMITIVA_CERRAR_ARCHIVO;
	PROTOCOLO_KERNEL_A_CPU respuestaCerrado;

	librocket_enviarMensaje(kernelSocket, &pedidoCerrado,
			sizeof(PROTOCOLO_CPU_A_KERNEL), infoLogger);
	librocket_enviarMensaje(kernelSocket, &pcbActual->PID, sizeof(int32_t),
			infoLogger);
	librocket_enviarMensaje(kernelSocket, &descriptor_archivo,
			sizeof(t_descriptor_archivo), infoLogger);

	int32_t resultado = librocket_recibirMensaje(kernelSocket,
			&respuestaCerrado, sizeof(PROTOCOLO_KERNEL_A_CPU), infoLogger);

	if (resultado == 0) {
		log_info(infoLogger, "Se cayo el Kernel.");
		exit(EXIT_FAILURE);
	}

	if (respuestaCerrado == ARCHIVO_INVALIDO) {
		log_info(infoEjecucion, "Archivo invalido.");
		pcbActual->EC = ACCESO_ARCHIVO_INEXISTENTE;
		pcbActual->banderas.terminacion_anormal = true;
		return;
	} else if (respuestaCerrado == CERRADO_EXITOSO) {
		log_info(infoEjecucion, "Archivo cerrado de manera exitosa.");
	}

}

void operaciones_kernel_moverCursor(t_descriptor_archivo descriptor_archivo,
		t_valor_variable posicion) {

	log_info(infoEjecucion, "Se llamo a la primitiva: MOVER CURSOR.");
	log_info(infoEjecucion, "Descriptor: %i.", descriptor_archivo);
	log_info(infoEjecucion, "Posicion: %i.", posicion);

	if (pcbActual->banderas.terminacion_anormal)
		return;

	PROTOCOLO_CPU_A_KERNEL pedidoMoverCursor = PRIMITIVA_MOVER_CURSOR_ARCHIVO;
	PROTOCOLO_KERNEL_A_CPU respuestaMoverCursor;

	librocket_enviarMensaje(kernelSocket, &pedidoMoverCursor,
			sizeof(PROTOCOLO_CPU_A_KERNEL), infoLogger);
	librocket_enviarMensaje(kernelSocket, &pcbActual->PID, sizeof(int32_t),
			infoLogger);
	librocket_enviarMensaje(kernelSocket, &descriptor_archivo,
			sizeof(t_descriptor_archivo), infoLogger);
	librocket_enviarMensaje(kernelSocket, &posicion, sizeof(t_valor_variable),
			infoLogger);

	int32_t resultado = librocket_recibirMensaje(kernelSocket,
			&respuestaMoverCursor, sizeof(PROTOCOLO_KERNEL_A_CPU), infoLogger);

	if (resultado == 0) {
		log_info(infoLogger, "Se cayo el Kernel.");
		exit(EXIT_FAILURE);
	}

	if (respuestaMoverCursor == ARCHIVO_INVALIDO) {
		log_info(infoEjecucion, "Archivo invalido.");
		pcbActual->EC = ACCESO_ARCHIVO_INEXISTENTE;
		pcbActual->banderas.terminacion_anormal = true;
		return;
	} else if (respuestaMoverCursor == MOVER_CURSOR_EXITOSO) {
		log_info(infoEjecucion, "Cursor movido de manera exitosa.");
	}

}

void operaciones_kernel_escribir(t_descriptor_archivo descriptor_archivo,
		void* informacion, t_valor_variable tamanio) {

	log_info(infoEjecucion, "Se llamo a la primitiva: ESCRIBIR.");
	log_info(infoEjecucion, "Descriptor: %i.", descriptor_archivo);
	log_info(infoEjecucion, "Informacion: %s.", informacion);
	log_info(infoEjecucion, "Tamanio: %i.", tamanio);

	if (pcbActual->banderas.terminacion_anormal)
		return;

	PROTOCOLO_CPU_A_KERNEL pedidoEscritura = PRIMITIVA_ESCRIBIR_ARCHIVO;
	PROTOCOLO_KERNEL_A_CPU respuestaEscritura;

	librocket_enviarMensaje(kernelSocket, &pedidoEscritura,
			sizeof(PROTOCOLO_CPU_A_KERNEL), infoLogger);
	librocket_enviarMensaje(kernelSocket, &pcbActual->PID, sizeof(int32_t),
			infoLogger);
	librocket_enviarMensaje(kernelSocket, &descriptor_archivo,
			sizeof(t_descriptor_archivo), infoLogger);
	librocket_enviarMensaje(kernelSocket, &tamanio, sizeof(t_valor_variable),
			infoLogger);
	librocket_enviarMensaje(kernelSocket, informacion, tamanio, infoLogger);

	int32_t resultado = librocket_recibirMensaje(kernelSocket,
			&respuestaEscritura, sizeof(PROTOCOLO_KERNEL_A_CPU), infoLogger);

	if (resultado == 0) {
		log_info(infoLogger, "Se cayo el Kernel.");
		exit(EXIT_FAILURE);
	}

	if (respuestaEscritura == ARCHIVO_INVALIDO) {
		log_info(infoEjecucion, "Archivo invalido.");
		pcbActual->EC = ACCESO_ARCHIVO_INEXISTENTE;
		pcbActual->banderas.terminacion_anormal = true;
		return;
	} else if (respuestaEscritura == PERMISO_ARCHIVO_INVALIDO) {
		log_info(infoEjecucion,
				"El proceso no tiene los permisos necesarios para escribir.");
		pcbActual->EC = INTENTO_ESCRIBIR_ARCHIVO_SIN_PERMISO;
		pcbActual->banderas.terminacion_anormal = true;
		return;
	} else if (respuestaEscritura == ESCRITURA_EXITOSA) {
		log_info(infoEjecucion, "Escritura exitosa.");
	}

}

void operaciones_kernel_leer(t_descriptor_archivo descriptor_archivo,
		t_puntero informacion, t_valor_variable tamanio) {

	log_info(infoEjecucion, "Se llamo a la primitiva: LEER.");
	log_info(infoEjecucion, "Descriptor: %i.", descriptor_archivo);
	log_info(infoEjecucion, "Tamanio: %i.", tamanio);

	if (pcbActual->banderas.terminacion_anormal)
		return;

	t_valor_variable valor;

	PROTOCOLO_CPU_A_KERNEL pedidoLectura = PRIMITIVA_LEER_ARCHIVO;
	PROTOCOLO_KERNEL_A_CPU respuestaLectura;

	librocket_enviarMensaje(kernelSocket, &pedidoLectura,
			sizeof(PROTOCOLO_CPU_A_KERNEL), infoLogger);
	librocket_enviarMensaje(kernelSocket, &pcbActual->PID, sizeof(int32_t),
			infoLogger);
	librocket_enviarMensaje(kernelSocket, &descriptor_archivo,
			sizeof(t_descriptor_archivo), infoLogger);
	librocket_enviarMensaje(kernelSocket, &tamanio, sizeof(t_valor_variable),
			infoLogger);

	int32_t resultado = librocket_recibirMensaje(kernelSocket,
			&respuestaLectura, sizeof(PROTOCOLO_KERNEL_A_CPU), infoLogger);

	if (resultado == 0) {
		log_info(infoLogger, "Se cayo el Kernel.");
		exit(EXIT_FAILURE);
	}

	if (respuestaLectura == ARCHIVO_INVALIDO) {
		log_info(infoEjecucion, "Archivo invalido.");
		pcbActual->EC = ACCESO_ARCHIVO_INEXISTENTE;
		pcbActual->banderas.terminacion_anormal = true;
		return;
	} else if (respuestaLectura == PERMISO_ARCHIVO_INVALIDO) {
		log_info(infoEjecucion,
				"El proceso no tiene los permisos necesarios para leer.");
		pcbActual->EC = INTENTO_LEER_ARCHIVO_SIN_PERMISO;
		pcbActual->banderas.terminacion_anormal = true;
		return;
	} else if (respuestaLectura == LECTURA_EXITOSA) {

		librocket_recibirMensaje(kernelSocket, &valor, tamanio, infoLogger);

		funciones_asignar(informacion, valor);

		log_info(infoEjecucion, "Lectura exitosa.");
		log_info(infoEjecucion, "Se cargo la lectura en: %i.", informacion);
		log_info(infoEjecucion, "Lectura: %i.", valor);

	}

}


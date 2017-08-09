#include "structs.h"

t_puntero posicionMemoria_a_puntero(t_direccion_logica posMemoria,
		int32_t tamanio_pagina) {
	return posMemoria.paginaId * tamanio_pagina + posMemoria.offset;
}

t_direccion_logica puntero_a_posicionMemoria(t_puntero poslogica,
		int32_t tamanio_pagina, int32_t pid) {
	t_direccion_logica respuesta;
	respuesta.paginaId = poslogica / tamanio_pagina;
	respuesta.offset = poslogica % tamanio_pagina;
	respuesta.tamanio = 4;
	respuesta.pid = pid;
	return respuesta;
}

t_pcb* recibirPcb(int socket, t_log* infoLogger) {

	t_pcb* pcb = malloc(sizeof(t_pcb));

	pcb->indice_codigo = list_create();
	pcb->indice_stack = list_create();

	librocket_recibirMensaje(socket, &(pcb->PID), sizeof(int32_t), infoLogger);
	librocket_recibirMensaje(socket, &(pcb->PC), sizeof(int32_t), infoLogger);
	librocket_recibirMensaje(socket, &(pcb->SP), sizeof(int32_t), infoLogger);
	librocket_recibirMensaje(socket, &(pcb->cantidad_paginas_totales),
			sizeof(int32_t), infoLogger);
	librocket_recibirMensaje(socket, &(pcb->estado), sizeof(ESTADO_DE_PROCESO),
			infoLogger);
	librocket_recibirMensaje(socket, &(pcb->EC), sizeof(EXIT_CODE_TYPE),
			infoLogger);
	librocket_recibirMensaje(socket, &(pcb->instruccion_inicio),
			sizeof(t_puntero_instruccion), infoLogger);

	// Recibo indice_codigo ----------------------------------------------------------
	librocket_recibirMensaje(socket, &(pcb->instrucciones_size), sizeof(t_size),
			infoLogger);
	int32_t contador_codigo;
	for (contador_codigo = 0; contador_codigo < pcb->instrucciones_size;
			contador_codigo++) {
		t_intructions* instruccion = malloc(sizeof(t_intructions));
		librocket_recibirMensaje(socket, instruccion, sizeof(t_intructions),
				infoLogger);
		list_add(pcb->indice_codigo, instruccion);
	}
	// --------------------------------------------------------------------------------

	// Recibo indice_etiquetas --------------------------------------------------------
	librocket_recibirMensaje(socket, &(pcb->etiquetas_size), sizeof(t_size),
			infoLogger);
	if (pcb->etiquetas_size > 0) {
		pcb->indice_etiquetas = malloc(pcb->etiquetas_size);
		librocket_recibirMensaje(socket, pcb->indice_etiquetas,
				pcb->etiquetas_size, infoLogger);
	}
	// --------------------------------------------------------------------------------

	librocket_recibirMensaje(socket, &(pcb->cantidad_de_funciones),
			sizeof(int32_t), infoLogger);
	librocket_recibirMensaje(socket, &(pcb->cantidad_de_etiquetas),
			sizeof(int32_t), infoLogger);

	// Recibo indice_stack --------------------------------------------------------
	librocket_recibirMensaje(socket, &(pcb->cant_entradas_indice_stack),
			sizeof(int32_t), infoLogger);
	int32_t contador_registro_stack;
	for (contador_registro_stack = 0;
			contador_registro_stack < pcb->cant_entradas_indice_stack;
			contador_registro_stack++) {
		t_registro_stack* registro = malloc(sizeof(t_registro_stack));

		librocket_recibirMensaje(socket, &(registro->posicion), sizeof(int32_t),
				infoLogger);

		registro->argumentos = list_create();
		registro->variables = list_create();

		// Recibo argumentos ------------------------------------------------------
		int32_t cantidad_argumentos;
		int32_t contador_argumentos;
		librocket_recibirMensaje(socket, &(cantidad_argumentos),
				sizeof(int32_t), infoLogger);
		for (contador_argumentos = 0; contador_argumentos < cantidad_argumentos;
				contador_argumentos++) {

			t_direccion_logica* direccion_argumento = malloc(
					sizeof(t_direccion_logica));
			librocket_recibirMensaje(socket, &direccion_argumento,
					sizeof(t_direccion_logica), infoLogger);
			list_add(registro->argumentos, direccion_argumento);

		}
		// ------------------------------------------------------------------------

		// Recibo variables -------------------------------------------------------
		int32_t cantidad_variables;
		int32_t contador_variables;
		librocket_recibirMensaje(socket, &(cantidad_variables), sizeof(int32_t),
				infoLogger);
		for (contador_variables = 0; contador_variables < cantidad_variables;
				contador_variables++) {

			t_variable* variable = malloc(sizeof(t_variable));
			librocket_recibirMensaje(socket, &variable->id, sizeof(char),
					infoLogger);
			librocket_recibirMensaje(socket, &variable->posicion,
					sizeof(t_direccion_logica), infoLogger);

			list_add(registro->variables, variable);

		}
		// ------------------------------------------------------------------------

		librocket_recibirMensaje(socket, &(registro->pos_retorno),
				sizeof(int32_t), infoLogger);
		librocket_recibirMensaje(socket, &(registro->pos_var_retorno),
				sizeof(t_direccion_logica), infoLogger);

		list_add(pcb->indice_stack, registro);

	}
	// -----------------------------------------------------------------------------

	// Recibo fin de stack ---------------------------------------------------------
	librocket_recibirMensaje(socket, &(pcb->fin_stack),
			sizeof(t_direccion_logica), infoLogger);
	// -----------------------------------------------------------------------------

	// Recibo banderas de ejecucion -------------------------------------------------
	librocket_recibirMensaje(socket, &(pcb->banderas),
			sizeof(t_banderas_ejecucion), infoLogger);
	// -----------------------------------------------------------------------------

	return pcb;

}

void enviarPcb(int socket, t_pcb* pcb, t_log* infoLogger) {

	librocket_enviarMensaje(socket, &(pcb->PID), sizeof(int32_t), infoLogger);

	librocket_enviarMensaje(socket, &(pcb->PC), sizeof(int32_t), infoLogger);
	librocket_enviarMensaje(socket, &(pcb->SP), sizeof(int32_t), infoLogger);
	librocket_enviarMensaje(socket, &(pcb->cantidad_paginas_totales),
			sizeof(int32_t), infoLogger);
	librocket_enviarMensaje(socket, &(pcb->estado), sizeof(ESTADO_DE_PROCESO),
			infoLogger);
	librocket_enviarMensaje(socket, &(pcb->EC), sizeof(EXIT_CODE_TYPE),
			infoLogger);
	librocket_enviarMensaje(socket, &(pcb->instruccion_inicio),
			sizeof(t_puntero_instruccion), infoLogger);

	// Envio indice_codigo ----------------------------------------------------------
	librocket_enviarMensaje(socket, &(pcb->instrucciones_size), sizeof(t_size),
			infoLogger);
	int32_t contador_codigo;
	for (contador_codigo = 0; contador_codigo < pcb->instrucciones_size;
			contador_codigo++) {
		t_intructions* instruccion = (t_intructions*) list_get(
				pcb->indice_codigo, contador_codigo);
		librocket_enviarMensaje(socket, instruccion, sizeof(t_intructions),
				infoLogger);
	}
	// --------------------------------------------------------------------------------

	// Envio indice_etiquetas --------------------------------------------------------
	librocket_enviarMensaje(socket, &(pcb->etiquetas_size), sizeof(t_size),
			infoLogger);
	if (pcb->etiquetas_size > 0) {
		librocket_enviarMensaje(socket, pcb->indice_etiquetas,
				pcb->etiquetas_size, infoLogger);
	}
	// --------------------------------------------------------------------------------

	librocket_enviarMensaje(socket, &(pcb->cantidad_de_funciones),
			sizeof(int32_t), infoLogger);
	librocket_enviarMensaje(socket, &(pcb->cantidad_de_etiquetas),
			sizeof(int32_t), infoLogger);

	// Envio indice_stack --------------------------------------------------------
	librocket_enviarMensaje(socket, &(pcb->cant_entradas_indice_stack),
			sizeof(int32_t), infoLogger);
	int32_t contador_registro_stack;
	for (contador_registro_stack = 0;
			contador_registro_stack < pcb->cant_entradas_indice_stack;
			contador_registro_stack++) {
		t_registro_stack* registro = (t_registro_stack*) list_get(
				pcb->indice_stack, contador_registro_stack);

		librocket_enviarMensaje(socket, &(registro->posicion), sizeof(int32_t),
				infoLogger);

		// Envio argumentos ------------------------------------------------------
		int32_t cantidad_argumentos = list_size(registro->argumentos);
		int32_t contador_argumentos;
		librocket_enviarMensaje(socket, &cantidad_argumentos, sizeof(int32_t),
				infoLogger);
		for (contador_argumentos = 0; contador_argumentos < cantidad_argumentos;
				contador_argumentos++) {

			t_direccion_logica* direccion_argumento = list_get(
					registro->argumentos, contador_argumentos);

			librocket_enviarMensaje(socket, &direccion_argumento,
					sizeof(t_direccion_logica), infoLogger);

		}
		// ------------------------------------------------------------------------

		// Envio variables --------------------------------------------------------
		int32_t cantidad_variables = list_size(registro->variables);
		int32_t contador_variables;
		librocket_enviarMensaje(socket, &(cantidad_variables), sizeof(int32_t),
				infoLogger);
		for (contador_variables = 0; contador_variables < cantidad_variables;
				contador_variables++) {

			t_variable* variable = (t_variable*) list_get(registro->variables,
					contador_variables);
			librocket_enviarMensaje(socket, &variable->id, sizeof(char),
					infoLogger);
			librocket_enviarMensaje(socket, &variable->posicion,
					sizeof(t_direccion_logica), infoLogger);

		}
		// ------------------------------------------------------------------------

		librocket_enviarMensaje(socket, &(registro->pos_retorno),
				sizeof(int32_t), infoLogger);
		librocket_enviarMensaje(socket, &(registro->pos_var_retorno),
				sizeof(t_direccion_logica), infoLogger);

	}
	// -----------------------------------------------------------------------------

	// Envio fin de stack ----------------------------------------------------------
	librocket_enviarMensaje(socket, &(pcb->fin_stack),
			sizeof(t_direccion_logica), infoLogger);
	// -----------------------------------------------------------------------------

	// Envio banderas de ejecucion -------------------------------------------------
	librocket_enviarMensaje(socket, &(pcb->banderas),
			sizeof(t_banderas_ejecucion), infoLogger);
	// -----------------------------------------------------------------------------

}

char* itoa(int32_t n) {
	char* string = malloc(32);
	sprintf(string, "%d", n);
	return string;
}

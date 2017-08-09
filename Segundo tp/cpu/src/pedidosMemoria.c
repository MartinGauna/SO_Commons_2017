#include "headers/pedidosMemoria.h"

void pedirInstruccionAMemoria(int32_t numeroDeInstruccion) {

	if (pcbActual->banderas.terminacion_anormal)
		return;

	t_intructions* instruccion = (t_intructions*) list_get(
			pcbActual->indice_codigo, numeroDeInstruccion);

	int32_t bytes_a_pedir = 0;
	int32_t bytes_leidos = 0;

	t_direccion_logica pedido;
	pedido.paginaId = instruccion->start / tamanioPagina;
	int32_t posicion_en_la_pagina = instruccion->start
			- (pedido.paginaId) * tamanioPagina;
	pedido.pid = pcbActual->PID;

	instruccionActual = malloc(instruccion->offset);
	while (bytes_leidos < instruccion->offset) {

		if (instruccion->offset - bytes_leidos > tamanioPagina) {

			bytes_a_pedir = tamanioPagina - posicion_en_la_pagina;

			pedido.offset = posicion_en_la_pagina;
			posicion_en_la_pagina = 0;
			pedido.tamanio = bytes_a_pedir;

		} else {

			if (instruccion->offset <= tamanioPagina
					&& instruccion->offset + posicion_en_la_pagina
							> tamanioPagina) {
				bytes_a_pedir = tamanioPagina - posicion_en_la_pagina;
				pedido.offset = posicion_en_la_pagina;
				posicion_en_la_pagina = 0;
			} else {
				bytes_a_pedir = instruccion->offset - bytes_leidos;
				pedido.offset = posicion_en_la_pagina;
			}

			pedido.tamanio = bytes_a_pedir;

		}

		PROTOCOLO_KERNEL_Y_CPU_A_MEMORIA mensajePedido = LEER_DE_MEMORIA;

		librocket_enviarMensaje(memoriaSocket, &mensajePedido,
				sizeof(PROTOCOLO_KERNEL_Y_CPU_A_MEMORIA), infoLogger);

		librocket_enviarMensaje(memoriaSocket, &pedido,
				sizeof(t_direccion_logica), infoLogger);

		PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU respuestaPedido = 0;

		int resultadoRespuesta = librocket_recibirMensaje(memoriaSocket,
				&respuestaPedido, sizeof(PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU),
				infoLogger);

		if (resultadoRespuesta == 0) {
			log_info(infoLogger, "Se cayo la Memoria.");
			exit(EXIT_FAILURE);
		}

		if (respuestaPedido == ERROR_EN_LECTURA) {
			log_info(infoEjecucion, "Lectura invalida.");
			pcbActual->EC = EXCEPCION_MEMORIA;
			pcbActual->banderas.terminacion_anormal = true;
			return;
		} else if (respuestaPedido == LECTURA_SATISFACTORIA) {

			char* recepcion_de_memoria = malloc(pedido.tamanio);
			librocket_recibirMensaje(memoriaSocket, recepcion_de_memoria,
					pedido.tamanio, infoLogger);
			memcpy(instruccionActual + bytes_leidos, recepcion_de_memoria,
					pedido.tamanio);
			bytes_leidos += bytes_a_pedir;
			free(recepcion_de_memoria);

		}

		pedido.paginaId++;

	}

}

t_valor_variable pedido_de_lectura_a_memoria(t_direccion_logica pedidoEntero) {

	if (pcbActual->banderas.terminacion_anormal)
		return -1;

	int32_t bytes_a_pedir = 0;
	int32_t bytes_leidos = 0;

	t_valor_variable valorPedido;
	t_direccion_logica pedidoEnPartes;
	pedidoEnPartes.paginaId = pedidoEntero.paginaId;
	int32_t posicion_en_la_pagina = pedidoEntero.offset;
	pedidoEnPartes.pid = pedidoEntero.pid;

	while (bytes_leidos < pedidoEntero.tamanio) {

		if (pedidoEntero.tamanio - bytes_leidos > tamanioPagina) {

			bytes_a_pedir = tamanioPagina - posicion_en_la_pagina;

			pedidoEnPartes.offset = posicion_en_la_pagina;
			posicion_en_la_pagina = 0;
			pedidoEnPartes.tamanio = bytes_a_pedir;

		} else {

			if (pedidoEntero.tamanio < tamanioPagina
					&& pedidoEntero.tamanio + posicion_en_la_pagina
							> tamanioPagina) {
				bytes_a_pedir = tamanioPagina - posicion_en_la_pagina;
				pedidoEnPartes.offset = posicion_en_la_pagina;
				posicion_en_la_pagina = 0;
			} else {
				bytes_a_pedir = pedidoEntero.tamanio - bytes_leidos;
				pedidoEnPartes.offset = posicion_en_la_pagina;
			}

			pedidoEnPartes.tamanio = bytes_a_pedir;

		}

		PROTOCOLO_KERNEL_Y_CPU_A_MEMORIA mensajePedido = LEER_DE_MEMORIA;

		librocket_enviarMensaje(memoriaSocket, &mensajePedido,
				sizeof(PROTOCOLO_KERNEL_Y_CPU_A_MEMORIA), infoLogger);

		librocket_enviarMensaje(memoriaSocket, &pedidoEnPartes,
				sizeof(t_direccion_logica), infoLogger);

		PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU respuestaPedido = 0;

		int resultadoRespuesta = librocket_recibirMensaje(memoriaSocket,
				&respuestaPedido, sizeof(PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU),
				infoLogger);

		if (resultadoRespuesta == 0) {
			log_info(infoLogger, "Se cayo la Memoria.");
			exit(EXIT_FAILURE);
		}

		if (respuestaPedido == ERROR_EN_LECTURA) {
			log_info(infoEjecucion, "Lectura invalida.");
			pcbActual->EC = EXCEPCION_MEMORIA;
			pcbActual->banderas.terminacion_anormal = true;
			return -1;
		} else if (respuestaPedido == LECTURA_SATISFACTORIA) {

			char* valorEnPartes = malloc(pedidoEnPartes.tamanio);
			librocket_recibirMensaje(memoriaSocket, valorEnPartes,
					pedidoEnPartes.tamanio, infoLogger);
			memcpy(&valorPedido + bytes_leidos, valorEnPartes,
					pedidoEnPartes.tamanio);
			bytes_leidos += bytes_a_pedir;
			free(valorEnPartes);

		}

		pedidoEnPartes.paginaId++;

	}

	return valorPedido;

}

t_valor_variable respuesta_de_lectura_a_memoria() {

	t_valor_variable valor;
	PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU respuestaPedido;

	int resultadoRespuesta = librocket_recibirMensaje(memoriaSocket,
			&respuestaPedido, sizeof(PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU),
			infoLogger);

	if (resultadoRespuesta == 0) {
		log_info(infoLogger, "Se cayo la Memoria.");
		exit(EXIT_FAILURE);
	}

	if (respuestaPedido == ERROR_EN_LECTURA) {
		log_info(infoEjecucion, "Lectura invalida.");
		exit(EXIT_FAILURE);
	} else if (respuestaPedido == LECTURA_SATISFACTORIA) {
		librocket_recibirMensaje(memoriaSocket, &valor,
				sizeof(t_valor_variable), infoLogger);
	}

	return valor;

}

void pedido_de_asignacion_a_memoria(t_direccion_logica direccionLogica,
		t_valor_variable valor) {

	if (pcbActual->banderas.terminacion_anormal)
		return;

	int32_t bytes_a_asignar = 0;
	int32_t bytes_asignados = 0;

	t_direccion_logica pedidoEnPartes;
	pedidoEnPartes.paginaId = direccionLogica.paginaId;
	int32_t posicion_en_la_pagina = direccionLogica.offset;
	pedidoEnPartes.pid = direccionLogica.pid;

	while (bytes_asignados < direccionLogica.tamanio) {

		if (direccionLogica.tamanio - bytes_asignados > tamanioPagina) {

			bytes_a_asignar = tamanioPagina - posicion_en_la_pagina;

			pedidoEnPartes.offset = posicion_en_la_pagina;
			posicion_en_la_pagina = 0;
			pedidoEnPartes.tamanio = bytes_a_asignar;

		} else {

			if (direccionLogica.tamanio < tamanioPagina
					&& direccionLogica.tamanio + posicion_en_la_pagina
							> tamanioPagina) {
				bytes_a_asignar = tamanioPagina - posicion_en_la_pagina;
				pedidoEnPartes.offset = posicion_en_la_pagina;
				posicion_en_la_pagina = 0;
			} else {
				bytes_a_asignar = direccionLogica.tamanio - bytes_asignados;
				pedidoEnPartes.offset = posicion_en_la_pagina;
			}

			pedidoEnPartes.tamanio = bytes_a_asignar;

		}

		PROTOCOLO_KERNEL_Y_CPU_A_MEMORIA mensajePedido = ESCRIBIR_A_MEMORIA;

		librocket_enviarMensaje(memoriaSocket, &mensajePedido,
				sizeof(PROTOCOLO_KERNEL_Y_CPU_A_MEMORIA), infoLogger);

		librocket_enviarMensaje(memoriaSocket, &pedidoEnPartes,
				sizeof(t_direccion_logica), infoLogger);

		librocket_enviarMensaje(memoriaSocket, &valor, pedidoEnPartes.tamanio,
				infoLogger);

		PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU respuestaPedido = 0;

		int resultadoRespuesta = librocket_recibirMensaje(memoriaSocket,
				&respuestaPedido, sizeof(PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU),
				infoLogger);

		if (resultadoRespuesta == 0) {
			log_info(infoLogger, "Se cayo la Memoria.");
			exit(EXIT_FAILURE);
		}

		if (respuestaPedido == ERROR_EN_ESCRITURA) {
			log_info(infoEjecucion, "Escritura invalida.");
			pcbActual->EC = EXCEPCION_MEMORIA;
			pcbActual->banderas.terminacion_anormal = true;
			return;
		} else if (respuestaPedido == ESCRITURA_SATISFACTORIA) {

			bytes_asignados += bytes_a_asignar;

		}

		pedidoEnPartes.paginaId++;

	}

}

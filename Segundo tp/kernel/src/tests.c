/*
 * tests.c
 *
 *  Created on: 17/4/2017
 *      Author: utnso
 */
#include "headers/tests.h"

void testRedondeoHaciaArriba() {
	int x = 59;
	int y = 15;

	int result = redondearDivisionPaArriba(59, 15);
	if (result == 4) {
		printf(
				"Testeo de division redondeada hacia arriba exitoso: %d dividido %d da como resultado %d \n",
				x, y, result);
	} else {
		printf(
				"AVISO: FALLO EL  Testeo de division redondeado hacia arriba : %d dividido %d da como resultado %d \n",
				x, y, result);
	}
	fflush(stdout);
}

void testObtenerCantidadDePaginasParaIniciarProceso() {
	//93 bytes
	char* codigo = string_new();
	string_append(&codigo,
			"#!/usr/bin/ansisop begin 31 # primero declaro las variables variables a, b a = 20 print a end");

	int tamanioCodigo = string_length(codigo);
	int result = obtenerCantidadDePaginasParaIniciarProceso(codigo);

	//con un page size de 50 bytes y un stack size de 2 necesito 4 paginas para almacenar stack y codigo
	if (result == 4) {
		printf(
				"Testeo de obtener cant de paginas iniciales exitoso: %d bytes de codigo y  %d paginas \n",
				tamanioCodigo, result);
	} else {
		printf(
				"AVISO: FALLO EL  Testeo de obtener cant de paginas iniciales exitoso: %d bytes de codigo y  %d paginas \n",
				tamanioCodigo, result);
	}
	fflush(stdout);

}

//crea un pcb y lo envia a la primera cpu que este conectada
void testEnviarPcb() {
	t_pcb* pcb = crearPcb();
	PROTOCOLO_KERNEL_A_CPU ejecutarPrograma = EJECUTAR_PROGRAMA;
	CPU* cpu = list_get(cpusLibres, 0);

	librocket_enviarMensaje(cpu->socket, &ejecutarPrograma,
			sizeof(PROTOCOLO_KERNEL_A_CPU), infoLogger);
	librocket_enviarMensaje(cpu->socket, pcb, sizeof(t_pcb), infoLogger);

}

void testCheckSiHayEspacioParaAsignarBloque() {
	nodo_tabla_heap* entrada = malloc(sizeof(nodo_tabla_heap));
	entrada->PID = 1;
	entrada->espacioDisponible = 50;
	entrada->numeroPagina = 2;
	list_add(tablaHeap, entrada);

	/*int result = checkSiExistePaginaConEspacio(45, 1);
	 if (result == 2) {
	 printf(
	 "Test: se chequeo exitosamente si el proceso tiene espacio suficiente para el pedido reaizado\n");
	 } else {
	 printf(
	 "FALLO EL TEST: chequeo si el proceso tiene espacio suficiente para el pedido realizado\n");
	 }
	 */
}

void crearListasDelPcb(t_pcb** pcb) {
	int i = 0;
	for (; i < 5; i++) {
		t_registro_stack * registro = malloc(sizeof(t_registro_stack));
		registro->argumentos = list_create();
		registro->variables = list_create();
		registro->posicion = i + 2;
		registro->pos_retorno = i;
		t_variable* var = malloc(sizeof(t_variable));
		var->id = i + 65;
		var->posicion.tamanio = 10;
		var->posicion.offset = 10;
		var->posicion.paginaId = 10;
		var->posicion.pid = 10;
		//registro->pos_var_retorno = *var;

		t_direccion_logica* argumento = malloc(sizeof(t_direccion_logica));
		argumento->offset = 60;
		argumento->paginaId = 10;
		argumento->tamanio = 10;
		argumento->pid = 10;

		list_add(registro->argumentos, argumento);
		list_add((*pcb)->indice_stack, registro);

	}
	i = 0;
	for (; i < 5; i++) {
		t_intructions* instruccion = malloc(sizeof(t_intructions));
		instruccion->start = 57;
		instruccion->offset = 6;
		list_add((*pcb)->indice_codigo, instruccion);
	}

	(*pcb)->indice_etiquetas = "";

}

void testEnviarCodigoDePrograma(int32_t pid, char* codigo, int pageSize) {
	FILE *fichero;
	char nombre[20] = "enviado.txt";
	fichero = fopen(nombre, "r+");
	if (fichero == NULL)
		perror("ASDASD");

	//PROTOCOLO_KERNEL_Y_CPU_A_MEMORIA envioCodigoPrograma = ESCRIBIR_A_MEMORIA;
	int32_t tamanioCodigo = string_length(codigo);
	if (pageSize == 0) {
		printf("----AVISO---TAMAÑO DE PAGINA 0, CERRANDO");
		log_error(infoLogger, "----AVISO---TAMAÑO DE PAGINA 0, CERRANDO");
		free(codigo);
		exit(1);
	}
	int32_t paginasOcupadasPorCodigo = redondearDivisionPaArriba(tamanioCodigo,
			pageSize);

	int i = 0;

	int32_t bytesTotalesAEnviar = tamanioCodigo;
	int bytesEnviados = 0;
	for (; i < paginasOcupadasPorCodigo; i++) {
		int offset = 0;
		t_direccion_logica* direccion = malloc(sizeof(t_direccion_logica));

		direccion->pid = pid;
		direccion->offset = offset;
		direccion->paginaId = i;
		char* codigoAEnviar = string_new();
		if (bytesTotalesAEnviar > pageSize) { // se escribe mas de una pagina
			int bytesAEscribir = pageSize;
			direccion->tamanio = bytesAEscribir;
			char* codigoCortado = string_substring_from(codigo, bytesEnviados);
			codigoAEnviar = string_substring_until(codigoCortado,
					bytesAEscribir);
			//memcpy(codigoAEnviar,codigoCortado , bytesAEscribir);
			free(codigoCortado);
			printf("%s", codigoAEnviar);
			fflush(stdout);

			bytesEnviados += bytesAEscribir;
			bytesTotalesAEnviar -= bytesAEscribir;
			free(codigoAEnviar);

		} else {
			//mando  de una ya que entra en una pagina
			direccion->tamanio = bytesTotalesAEnviar;
			char* codigoCortado = string_substring_from(codigo, bytesEnviados);
			codigoAEnviar = string_substring_until(codigoCortado,
					bytesTotalesAEnviar);
			//memcpy(codigoAEnviar,codigo + bytesEnviados,bytesTotalesAEnviar);
			printf("%s", codigoAEnviar);
			fflush(stdout);

		}
	}
	fclose(fichero);
}


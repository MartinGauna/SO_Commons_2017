#include "../headers/tests.h"

void correrTestReciboPcb() {

}

/*
void testRecibirPcb() {

	PROTOCOLO_KERNEL_A_CPU mensaje;

	librocket_recibirMensaje(kernelSocket, &mensaje, sizeof(int32_t),
			infoLogger);

	if (mensaje == EJECUTAR_PROGRAMA) {

		pcbActual = recibirPcb(kernelSocket, infoLogger);
		printf("Se recibio un programa para ejecutar. \n");

	}

	ejecutarPrograma();

}

void crearPcbTrucho() {

	cargarCodigo();

	pcbEjecutandoTest.PID = 1;
	pcbEjecutandoTest.PC = 0;
	pcbEjecutandoTest.SP = 0;
	pcbEjecutandoTest.cantidad_paginas_totales = 4;
	pcbEjecutandoTest.estado = EJECUTANDO;

	pcbEjecutandoTest.EC = -20;

	t_metadata_program* metadata = metadata_desde_literal(codigoPrograma);

	pcbEjecutandoTest.instruccion_inicio = metadata->instruccion_inicio;

	pcbEjecutandoTest.instrucciones_size = metadata->instrucciones_size;
	pcbEjecutandoTest.instrucciones_serializado =
			metadata->instrucciones_serializado;

	pcbEjecutandoTest.etiquetas_size = metadata->etiquetas_size;
	pcbEjecutandoTest.etiquetas = metadata->etiquetas;

	pcbEjecutandoTest.cantidad_de_etiquetas = metadata->cantidad_de_etiquetas;
	pcbEjecutandoTest.cantidad_de_funciones = metadata->cantidad_de_funciones;

	pcbEjecutandoTest.cant_entradas_indice_stack = 1;

	pcbEjecutandoTest.indice_stack = list_create();
	registro_indice_stack_para_serializar* indiceStack = malloc(
			sizeof(registro_indice_stack_para_serializar));
	indiceStack->posicion = 0;
	indiceStack->cant_argumentos = 1;
	indiceStack->argumentos = malloc(
			indiceStack->cant_argumentos * sizeof(t_direccion_logica));
	t_direccion_logica direccion_argumento;
	direccion_argumento.offset = 0;
	direccion_argumento.paginaId = 0;
	direccion_argumento.pid = 0;
	direccion_argumento.tamanio = 0;
	indiceStack->argumentos[0] = direccion_argumento;

	indiceStack->cant_variables = 1;
	indiceStack->variables = malloc(
			indiceStack->cant_variables * sizeof(t_variable));
	t_variable variable;
	t_direccion_logica variable_posicion;
	variable_posicion.offset = 0;
	variable_posicion.paginaId = 0;
	variable_posicion.pid = 0;
	variable_posicion.tamanio = 0;
	variable.posicion = variable_posicion;
	variable.id = 'a';
	indiceStack->variables[0] = variable;

	indiceStack->pos_retorno = 0;
	indiceStack->pos_var_retorno.offset = 0;
	indiceStack->pos_var_retorno.paginaId = 0;
	indiceStack->pos_var_retorno.pid = 0;
	indiceStack->pos_var_retorno.tamanio = 4;
	list_add(pcbEjecutandoTest.indice_stack, indiceStack);

	pcbEjecutandoTest.fin_stack.offset = 0;
	pcbEjecutandoTest.fin_stack.paginaId = 0;
	pcbEjecutandoTest.fin_stack.pid = pcbEjecutandoTest.PID;
	pcbEjecutandoTest.fin_stack.tamanio = 4;

	inicializarPrimitivas();

}
*/

/*
void testSerializar() {

	int32_t i;

	crearPcbTrucho();

	t_pcb_serializado asf = serializar(pcbEjecutandoTest);

	t_pcb_para_serializar* pcbBien = (t_pcb_para_serializar*) deserializar(
			asf.contenido_pcb);

	printf("%i\n", pcbBien->PID);
	printf("%i\n", pcbBien->PC);
	printf("%i\n", pcbBien->SP);
	printf("%i\n", pcbBien->cantidad_paginas_totales);
	printf("%i\n", pcbBien->estado);
	printf("%i\n", pcbBien->EC);
	printf("%i\n", pcbBien->instruccion_inicio);
	printf("%i\n", pcbBien->instrucciones_size);
	printf("%s\n", pcbBien->instrucciones_serializado);

	int contador;
	for (contador = 0; contador < pcbBien->instrucciones_size; contador++) {
		printf("%i\n", pcbBien->instrucciones_serializado[contador].offset);
		printf("%i\n", pcbBien->instrucciones_serializado[contador].start);
	}

	printf("%i\n", pcbBien->etiquetas_size);
	printf("%s\n", pcbBien->etiquetas);
	printf("%i\n", pcbBien->cantidad_de_funciones);
	printf("%i\n", pcbBien->cantidad_de_etiquetas);

	for (i = 0; i < pcbBien->cant_entradas_indice_stack; i++) {

		int j;
		registro_indice_stack_para_serializar* registro =
				(registro_indice_stack_para_serializar*) list_get(
						pcbBien->indice_stack, i);

		printf("%i\n", registro->posicion);
		printf("%i\n", registro->cant_argumentos);

		for (j = 0; j < registro->cant_argumentos; j++) {
			printf("%i\n", registro->argumentos[j].offset);
			printf("%i\n", registro->argumentos[j].paginaId);
			printf("%i\n", registro->argumentos[j].pid);
			printf("%i\n", registro->argumentos[j].tamanio);
		}

		for (j = 0; j < registro->cant_variables; j++) {
			printf("%c\n", registro->variables[j].id);
			printf("%i\n", registro->variables[j].posicion.offset);
			printf("%i\n", registro->variables[j].posicion.paginaId);
			printf("%i\n", registro->variables[j].posicion.pid);
			printf("%i\n", registro->variables[j].posicion.tamanio);
		}

		printf("%i\n", registro->pos_retorno);
		printf("%i\n", registro->pos_var_retorno.offset);
		printf("%i\n", registro->pos_var_retorno.paginaId);
		printf("%i\n", registro->pos_var_retorno.pid);
		printf("%i\n", registro->pos_var_retorno.tamanio);

	}

	printf("%i\n", pcbBien->cant_entradas_indice_stack);
	printf("%i\n", pcbBien->fin_stack.offset);
	printf("%i\n", pcbBien->fin_stack.paginaId);
	printf("%i\n", pcbBien->fin_stack.pid);
	printf("%i\n", pcbBien->fin_stack.tamanio);

}
*/

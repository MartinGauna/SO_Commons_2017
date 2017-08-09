#include "../headers/tests.h"
#include "../headers/testEjecucion.h"

void testEjecucion() {

	inicializarPCB();
	inicializarPrimitivasTest();
	paginasTruchas();

	//testDefinirVariablesEnStack();
	//testAsignarValorAUnaVariable();
	//testDevolverValorAsignadoAUnaVariable();

	//testEjecutarSentenciasDeCorrido();

}

void inicializarPCB() {

	pcbEjecutandoTest = malloc(sizeof(t_pcb));

	cargarCodigo();
	t_metadata_program* metadata = metadata_desde_literal(codigoPrograma);

	pcbEjecutandoTest->PID = 1;
	pcbEjecutandoTest->PC = 0;
	pcbEjecutandoTest->SP = 0;
	pcbEjecutandoTest->cantidad_paginas_totales = 4;
	pcbEjecutandoTest->estado = EJECUTANDO;
	pcbEjecutandoTest->EC = NO_DEFINIDO;

	pcbEjecutandoTest->instruccion_inicio = metadata->instruccion_inicio;

	cargarInstrucciones(metadata);
	cargarEtiquetas(metadata);

	pcbEjecutandoTest->cantidad_de_funciones = metadata->cantidad_de_funciones;
	pcbEjecutandoTest->cantidad_de_etiquetas = metadata->cantidad_de_etiquetas;

	pcbEjecutandoTest->cant_entradas_indice_stack = 1;

	t_registro_stack* registroInicial = malloc(sizeof(t_registro_stack));
	registroInicial->posicion = pcbEjecutandoTest->cant_entradas_indice_stack;
	registroInicial->argumentos = list_create();
	registroInicial->variables = list_create();
	registroInicial->pos_retorno = 0;

	registroInicial->pos_var_retorno.offset = 0;
	registroInicial->pos_var_retorno.paginaId = 0;
	registroInicial->pos_var_retorno.pid = 0;
	registroInicial->pos_var_retorno.tamanio = 4;

	pcbEjecutandoTest->indice_stack = list_create();
	list_add(pcbEjecutandoTest->indice_stack, registroInicial);

	pcbEjecutandoTest->fin_stack.offset = 0;
	pcbEjecutandoTest->fin_stack.paginaId = 0;
	pcbEjecutandoTest->fin_stack.pid = 1;
	pcbEjecutandoTest->fin_stack.tamanio = 4;

}

void cargarInstrucciones(t_metadata_program* metadata) {

	pcbEjecutandoTest->instrucciones_size = metadata->instrucciones_size;
	pcbEjecutandoTest->indice_codigo = list_create();

	t_intructions instruccionSerializada;
	t_intructions* instruccion;

	int32_t contador;
	for (contador = 0; contador < pcbEjecutandoTest->instrucciones_size;
			contador++) {

		instruccion = malloc(sizeof(t_intructions));
		instruccionSerializada = metadata->instrucciones_serializado[contador];
		instruccion->offset = instruccionSerializada.offset;
		instruccion->start = instruccionSerializada.start;
		list_add(pcbEjecutandoTest->indice_codigo, instruccion);

	}

}

void cargarEtiquetas(t_metadata_program* metadata) {

	pcbEjecutandoTest->etiquetas_size = metadata->etiquetas_size;
	pcbEjecutandoTest->indice_etiquetas = malloc(metadata->etiquetas_size);
	pcbEjecutandoTest->indice_etiquetas = metadata->etiquetas;

}

void inicializarPrimitivasTest() {

// Funciones
	funciones_test = (AnSISOP_funciones ) {

			.AnSISOP_definirVariable = funciones_definirVariables_test,
					.AnSISOP_obtenerPosicionVariable =
							funciones_obtenerPosicionVariable_test,
					.AnSISOP_dereferenciar = funciones_dereferenciar_test,
					.AnSISOP_asignar = funciones_asignar_test,
					.AnSISOP_obtenerValorCompartida =
							funciones_obtenerValorCompartida_test,
					.AnSISOP_asignarValorCompartida =
							funciones_asignarValorCompartida_test,
					.AnSISOP_irAlLabel = funciones_irAlLabel_test,
					.AnSISOP_llamarSinRetorno = funciones_llamarSinRetorno_test,
					.AnSISOP_llamarConRetorno = funciones_llamarConRetorno_test,
					.AnSISOP_finalizar = funciones_finalizar_test,
					.AnSISOP_retornar = funciones_retornar_test

			};

// Operaciones Kernel

	kernel_operaciones_test = (AnSISOP_kernel ) {

			.AnSISOP_wait = operaciones_kernel_wait_test, .AnSISOP_signal =
					operaciones_kernel_signal_test, .AnSISOP_reservar =
					operaciones_kernel_reservar_test, .AnSISOP_liberar =
					operaciones_kernel_liberar_test, .AnSISOP_abrir =
					operaciones_kernel_abrir_test, .AnSISOP_borrar =
					operaciones_kernel_borrar_test, .AnSISOP_cerrar =
					operaciones_kernel_cerrar_test, .AnSISOP_moverCursor =
					operaciones_kernel_moverCursor_test, .AnSISOP_escribir =
					operaciones_kernel_escribir_test, .AnSISOP_leer =
					operaciones_kernel_leer_test

			};

}

void paginasTruchas() {

	paginas_truchas = list_create();

	tamanioPagina = 40;

	int32_t contador;
	for (contador = 0; contador < pcbEjecutandoTest->cantidad_paginas_totales;
			contador++) {

		t_pagina_trucha* pagina = malloc(sizeof(t_pagina_trucha));
		pagina->pid = pcbEjecutandoTest->PID;
		pagina->paginaID = contador;
		pagina->buffer = malloc(tamanioPagina);

		list_add(paginas_truchas, pagina);

	}

}

void asignarEnPaginasTruchas(t_direccion_logica direccion,
		t_valor_variable valor) {

	t_pagina_trucha* paginaTrucha = list_get(paginas_truchas,
			direccion.paginaId);

	paginaTrucha->buffer[direccion.offset] = valor;

}

t_valor_variable pedidoAPaginasTruchas(t_direccion_logica direccion) {

	t_pagina_trucha* paginaTrucha = list_get(paginas_truchas,
			direccion.paginaId);

	return (t_valor_variable) paginaTrucha->buffer[direccion.offset];

}

void destruirElemento_test(void* data) {
	free(data);
}

void destruir_pcb_test() {

	list_destroy_and_destroy_elements(pcbEjecutandoTest->indice_codigo,
			destruirElemento_test);
	list_destroy_and_destroy_elements(pcbEjecutandoTest->indice_stack,
			destruirElemento_test);
	free(pcbEjecutandoTest->indice_etiquetas);
	free(pcbEjecutandoTest);
	free(codigoPrograma);

}

/*
void testEjecutarSentenciasDeCorrido() {

	do {

		if (llamada_a_funcion) {
			llamada_a_funcion = false;
			registro_stack_actual = list_get(pcbEjecutandoTest->indice_stack,
					pcbEjecutandoTest->cant_entradas_indice_stack - 1);
		}

		t_intructions* instruccion = list_get(pcbEjecutandoTest->indice_codigo,
				pcbEjecutandoTest->PC);

		analizadorLinea(
				string_substring(codigoPrograma, instruccion->start,
						instruccion->offset), &funciones_test,
				&kernel_operaciones_test);

		if (!llamada_a_funcion) {
			pcbEjecutandoTest->PC++;
		}

	} while (!termino_programa);

	t_valor_variable valor = funciones_dereferenciar_test(
			funciones_obtenerPosicionVariable_test('a'));
	printf("%i\n", valor);

	pcbEjecutandoTest->EC = FINALIZADO_CORRECTAMENTE;
	//enviarPcb(pcbEjecutandoTest);
	destruir_pcb_test();

}*/

void testDefinirVariablesEnStack() {

	analizadorLinea("variables a, g, 0, 1", &funciones_test,
			&kernel_operaciones_test);

	//t_puntero punteroA = funciones_obtenerPosicionVariable_test('a');
	//t_puntero punteroG = funciones_obtenerPosicionVariable_test('g');

	//t_puntero puntero0 = funciones_obtenerPosicionVariable_test('1');
	//t_puntero puntero1 = funciones_obtenerPosicionVariable_test('0');

}

void testAsignarValorAUnaVariable() {

	analizadorLinea("variables a", &funciones_test, &kernel_operaciones_test);
	analizadorLinea("a = 1", &funciones_test, &kernel_operaciones_test);

}

void testDevolverValorAsignadoAUnaVariable() {

	analizadorLinea("a = 1", &funciones_test, &kernel_operaciones_test);
	analizadorLinea("b = 1", &funciones_test, &kernel_operaciones_test);
	analizadorLinea("c = a + b", &funciones_test, &kernel_operaciones_test);

	//t_valor_variable valorC = funciones_dereferenciar_test(16);

}

// Funciones
t_puntero funciones_definirVariables_test(
		t_nombre_variable identificador_variable) {

	if (stack_overflow)
		return -1;

	if (pcbEjecutandoTest->fin_stack.paginaId
			== pcbEjecutandoTest->cantidad_paginas_totales) {
		stack_overflow = true;
		return -1;
	}

	t_registro_stack* registroActual = list_get(pcbEjecutandoTest->indice_stack,
			pcbEjecutandoTest->cant_entradas_indice_stack - 1);

	t_puntero punteroMemoria;

	if (isalpha(identificador_variable)) {

		t_variable* nueva_variable = malloc(sizeof(t_variable));

		nueva_variable->id = identificador_variable;
		nueva_variable->posicion = pcbEjecutandoTest->fin_stack;

		list_add(registroActual->variables, nueva_variable);

		punteroMemoria = posicionMemoria_a_puntero(nueva_variable->posicion,
				tamanioPagina);

	}

	if (isdigit(identificador_variable)) {

		t_direccion_logica* nuevo_argumento = malloc(
				sizeof(t_direccion_logica));

		nuevo_argumento->offset = pcbEjecutandoTest->fin_stack.offset;
		nuevo_argumento->paginaId = pcbEjecutandoTest->fin_stack.paginaId;
		nuevo_argumento->pid = pcbEjecutandoTest->fin_stack.pid;
		nuevo_argumento->tamanio = pcbEjecutandoTest->fin_stack.tamanio;

		list_add(registroActual->argumentos, nuevo_argumento);

		punteroMemoria = posicionMemoria_a_puntero(pcbEjecutandoTest->fin_stack,
				tamanioPagina);

	}

	pcbEjecutandoTest->fin_stack.offset += 4;

	if (pcbEjecutandoTest->fin_stack.offset >= tamanioPagina) {
		pcbEjecutandoTest->fin_stack.offset = 0;
		pcbEjecutandoTest->fin_stack.paginaId++;
	}

	return punteroMemoria;

}

t_puntero funciones_obtenerPosicionVariable_test(
		t_nombre_variable identificador_variable) {

	t_registro_stack* registroActual = (t_registro_stack*) list_get(
			pcbEjecutandoTest->indice_stack,
			pcbEjecutandoTest->cant_entradas_indice_stack - 1);

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

t_valor_variable funciones_dereferenciar_test(t_puntero direccion_variable) {

	t_direccion_logica direccionLogica = puntero_a_posicionMemoria(
			direccion_variable, tamanioPagina, pcbEjecutandoTest->PID);

	return pedidoAPaginasTruchas(direccionLogica);

}

void funciones_asignar_test(t_puntero direccion_variable,
		t_valor_variable valor) {

	t_direccion_logica direccionLogica = puntero_a_posicionMemoria(
			direccion_variable, tamanioPagina, pcbEjecutandoTest->PID);

	asignarEnPaginasTruchas(direccionLogica, valor);

}

t_valor_variable funciones_obtenerValorCompartida_test(
		t_nombre_compartida variable) {

	return (t_valor_variable) NULL;

}

t_valor_variable funciones_asignarValorCompartida_test(
		t_nombre_compartida variable, t_valor_variable valor_variable) {

	return (t_valor_variable) NULL;

}

void funciones_irAlLabel_test(t_nombre_etiqueta etiqueta) {

	pcbEjecutandoTest->PC = metadata_buscar_etiqueta(etiqueta,
			pcbEjecutandoTest->indice_etiquetas,
			pcbEjecutandoTest->etiquetas_size);

}

// TODO Testear
void funciones_llamarSinRetorno_test(t_nombre_etiqueta etiqueta) {

	llamada_a_funcion = true;

	if (stack_overflow)
		return;

	pcbEjecutandoTest->cant_entradas_indice_stack++;

	t_registro_stack* nuevoRegistro = malloc(sizeof(t_registro_stack));

	nuevoRegistro->argumentos = list_create();
	nuevoRegistro->variables = list_create();
	nuevoRegistro->pos_retorno = pcbEjecutandoTest->PC;
	nuevoRegistro->posicion = pcbEjecutandoTest->cant_entradas_indice_stack;

	list_add(pcbEjecutandoTest->indice_stack, nuevoRegistro);

	funciones_irAlLabel_test(etiqueta);

}

// TODO Testear
void funciones_llamarConRetorno_test(t_nombre_etiqueta etiqueta,
		t_puntero donde_retornar) {

	llamada_a_funcion = true;

	if (stack_overflow)
		return;

	pcbEjecutandoTest->cant_entradas_indice_stack++;

	t_registro_stack* nuevoRegistro = malloc(sizeof(t_registro_stack));

	nuevoRegistro->posicion = pcbEjecutandoTest->cant_entradas_indice_stack;
	nuevoRegistro->argumentos = list_create();
	nuevoRegistro->variables = list_create();
	nuevoRegistro->pos_retorno = pcbEjecutandoTest->PC;
	nuevoRegistro->pos_var_retorno = puntero_a_posicionMemoria(donde_retornar,
			tamanioPagina, pcbEjecutandoTest->PID);

	list_add(pcbEjecutandoTest->indice_stack, nuevoRegistro);

	funciones_irAlLabel_test(etiqueta);

}

// TODO Testear
void funciones_finalizar_test(void) {

	termino_programa = true;

}

// TODO Testear
void funciones_retornar_test(t_valor_variable retorno) {

	t_registro_stack* registroActual = (t_registro_stack*) list_remove(
			pcbEjecutandoTest->indice_stack,
			pcbEjecutandoTest->cant_entradas_indice_stack - 1);

	list_destroy(registroActual->argumentos);
	list_destroy(registroActual->variables);

	t_puntero posicion_fisica = posicionMemoria_a_puntero(
			registroActual->pos_var_retorno, tamanioPagina);

	funciones_asignar_test(posicion_fisica, retorno);

	pcbEjecutandoTest->cant_entradas_indice_stack--;
	pcbEjecutandoTest->PC = registroActual->pos_retorno;

}

// Operaciones Kernel
// TODO Todas, solo estan los encabezados.

// TODO Terminar intercambio de mensajes con Kernel
void operaciones_kernel_wait_test(t_nombre_semaforo identificador_semaforo) {

}

// TODO Terminar intercambio de mensajes con Kernel
void operaciones_kernel_signal_test(t_nombre_semaforo identificador_semaforo) {

}

// TODO Terminar intercambio de mensajes con Kernel
t_puntero operaciones_kernel_reservar_test(t_valor_variable espacio) {

	return (t_puntero) NULL;

}

void operaciones_kernel_liberar_test(t_puntero puntero) {

}

t_descriptor_archivo operaciones_kernel_abrir_test(
		t_direccion_archivo direccion, t_banderas flags) {

	return 0;

}

void operaciones_kernel_borrar_test(t_descriptor_archivo direccion) {

}

void operaciones_kernel_cerrar_test(t_descriptor_archivo descriptor_archivo) {

}

void operaciones_kernel_moverCursor_test(
		t_descriptor_archivo descriptor_archivo, t_valor_variable posicion) {

}

void operaciones_kernel_escribir_test(t_descriptor_archivo descriptor_archivo,
		void* informacion, t_valor_variable tamanio) {

	int32_t a;

	a = 0;

}

void operaciones_kernel_leer_test(t_descriptor_archivo descriptor_archivo,
		t_puntero informacion, t_valor_variable tamanio) {

}


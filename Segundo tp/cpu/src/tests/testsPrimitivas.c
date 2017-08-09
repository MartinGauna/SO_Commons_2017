#include "../headers/tests.h"

void correrTestPrimitivas() {

	tamanioPagina = 64;

	testAbrirArchivo();

}

void testAbrirArchivo() {

}

/*
 void crearPcbTrucho() {

 cargarCodigo();

 pcbActualTest.PID = 1;
 pcbActualTest.PC = 0;
 pcbActualTest.SP = 0;
 pcbActualTest.cantidad_paginas_totales = 4;
 pcbActualTest.estado = EJECUTANDO;
 pcbActualTest.cant_entradas_indice_stack = 0;

 pcbActualTest.fin_stack.offset = 0;
 pcbActualTest.fin_stack.paginaId = 0;
 pcbActualTest.fin_stack.pid = pcbActualTest.PID;
 pcbActualTest.fin_stack.tamanio = 4;

 pcbActualTest.EC = -20;

 pcbActualTest.indice_stack = list_create();

 t_registro_stack* indiceStack = malloc(sizeof(t_registro_stack));

 list_add(pcbActual->indice_stack, indiceStack);

 inicializarPrimitivas();

 }
 */

void cargarCodigo() {

	FILE* archivo;
	int32_t tamanio;
	char* buffer;

	archivo =
			fopen(
					"/home/utnso/workspacePractica/Parser/ansisop-parser/programas-ejemplo/facil.ansisop",
					"rb");
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
	codigoPrograma = malloc(tamanio + 1);
	string_append(&codigoPrograma, buffer);

	fclose(archivo);
	free(buffer);

}

void testDefinirVariableYObtenerPosicion() {

	t_puntero puntero0, puntero1, puntero2, punteroA, punteroB, punteroC;

	puntero0 = funciones_definirVariables('0');
	puntero1 = funciones_definirVariables('1');
	puntero2 = funciones_definirVariables('2');
	punteroA = funciones_definirVariables('a');
	punteroB = funciones_definirVariables('b');
	punteroC = funciones_definirVariables('c');

	puntero0 = funciones_obtenerPosicionVariable('0');
	puntero1 = funciones_obtenerPosicionVariable('1');
	puntero2 = funciones_obtenerPosicionVariable('2');
	punteroA = funciones_obtenerPosicionVariable('a');
	punteroB = funciones_obtenerPosicionVariable('b');
	punteroC = funciones_obtenerPosicionVariable('c');

	printf("%i\n", puntero0);
	printf("%i\n", puntero1);
	printf("%i\n", puntero2);
	printf("%i\n", punteroA);
	printf("%i\n", punteroB);
	printf("%i\n", punteroC);

}

/*
 void imprimirTodasLasInstrucciones() {

 int32_t indexInstruccion;

 printf("El puntero de la instruccion inicio es: %i\n",
 pcbActual->metadata_program->instruccion_inicio);

 for (indexInstruccion = 0;
 pcbActual->metadata_program->instrucciones_size > indexInstruccion;
 indexInstruccion++) {

 printf("La instruccion: %d\n", indexInstruccion);
 printf("Arranca en el byte: %i\n",
 pcbActual->metadata_program->instrucciones_serializado[indexInstruccion].start);
 printf("Tiene una longitud de: %i\n",
 pcbActual->metadata_program->instrucciones_serializado[indexInstruccion].offset);

 }

 }
 */

/*
 void testLeerDeMemoria() {

 conectarseAMemoria();

 //Pido paginas a la memoria
 PROTOCOLO_KERNEL_Y_CPU_A_MEMORIA mensaje = INICIALIZAR_PROGRAMA;
 librocket_enviarMensaje(memoriaSocket, &mensaje, sizeof(int32_t),
 infoLogger);
 int32_t pid = 1;
 librocket_enviarMensaje(memoriaSocket, &pid, sizeof(int32_t), infoLogger);

 int32_t paginasFijas = 2;
 librocket_enviarMensaje(memoriaSocket, &paginasFijas, sizeof(int32_t),
 infoLogger);

 PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU respuestaInicializacion;

 librocket_recibirMensaje(memoriaSocket, &respuestaInicializacion,
 sizeof(int32_t), infoLogger);

 //escribo en memoria
 t_direccion_logica direccionLogica;

 direccionLogica.offset = 0;
 direccionLogica.pid = 1;
 direccionLogica.paginaId = 0;
 direccionLogica.tamanio = 5;

 int32_t valor = 4;

 pedido_de_asignacion_a_memoria(direccionLogica, valor);

 //leo de memoria
 int32_t valorLeido = pedido_de_lectura_a_memoria(direccionLogica);

 printf("valor loco %d\n", valorLeido);

 }

 void testPedirInstruccion() {

 //Pido paginas a la memoria
 PROTOCOLO_KERNEL_Y_CPU_A_MEMORIA mensaje = INICIALIZAR_PROGRAMA;
 librocket_enviarMensaje(memoriaSocket, &mensaje, sizeof(int32_t),
 infoLogger);
 int32_t pid = 1;
 librocket_enviarMensaje(memoriaSocket, &pid, sizeof(int32_t), infoLogger);

 int32_t paginasFijas = 50;
 librocket_enviarMensaje(memoriaSocket, &paginasFijas, sizeof(int32_t),
 infoLogger);

 PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU respuestaInicializacion;

 librocket_recibirMensaje(memoriaSocket, &respuestaInicializacion,
 sizeof(int32_t), infoLogger);

 //escribo en memoria
 t_direccion_logica direccionLogica;

 direccionLogica.offset = 0;
 direccionLogica.pid = 1;
 direccionLogica.paginaId = 0;
 direccionLogica.tamanio = strlen(codigoPrograma);

 pedido_de_asignacion_a_memoria(direccionLogica, codigoPrograma);

 t_intructions instruccion;
 instruccion.offset = 20;
 instruccion.start = 72;

 t_direccion_logica direccionInstruccion = puntero_a_posicionMemoria(
 instruccion.start, tamanioPagina, 1);
 ;
 direccionInstruccion.tamanio = instruccion.offset;

 char* valorLeido;
 valorLeido = pedido_de_lectura_a_memoria(direccionInstruccion);

 valorLeido[direccionLogica.tamanio + 1] = '\0';

 printf("valor loco %d\n", valorLeido);

 }

 void testVariablesCompartidas(){

 int32_t valor_compartida;
 t_nombre_compartida variable = "!Global";

 valor_compartida = funciones_obtenerValorCompartida(variable);

 }
 */
/*
 void testIrALabel(){

 pcbActual->PC = metadata_buscar_etiqueta("doble",
 pcbActual->metadata_program->etiquetas, pcbActual->metadata_program->etiquetas_size);

 }
 */

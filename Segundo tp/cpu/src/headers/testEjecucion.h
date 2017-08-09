#ifndef HEADERS_TESTEJECUCION_H_
#define HEADERS_TESTEJECUCION_H_

typedef struct t_pagina_trucha {
	int32_t pid;
	int32_t paginaID;
	char* buffer;
} t_pagina_trucha;

void testEjecucion();
void inicializarPCB();
void inicializarPrimitivasTest();
void testEjecutarSentenciasDeCorrido();
void testDefinirVariablesEnStack();
void paginasTruchas();
void cargarInstrucciones(t_metadata_program*);
void cargarEtiquetas(t_metadata_program*);
void testAsignarValorAUnaVariable();
void testDevolverValorAsignadoAUnaVariable();

// Funciones
void inicializarPrimitivas_test();
t_puntero funciones_definirVariables_test(t_nombre_variable);
t_puntero funciones_obtenerPosicionVariable_test(t_nombre_variable);
t_valor_variable funciones_dereferenciar_test(t_puntero);
void funciones_asignar_test(t_puntero, t_valor_variable);
t_valor_variable funciones_obtenerValorCompartida_test(t_nombre_compartida);
t_valor_variable funciones_asignarValorCompartida_test(t_nombre_compartida,
		t_valor_variable);
void funciones_irAlLabel_test(t_nombre_etiqueta);
void funciones_llamarSinRetorno_test(t_nombre_etiqueta);
void funciones_llamarConRetorno_test(t_nombre_etiqueta, t_puntero);
void funciones_finalizar_test(void);
void funciones_retornar_test(t_valor_variable);

// Operaciones Kernel
void operaciones_kernel_wait_test(t_nombre_semaforo);
void operaciones_kernel_signal_test(t_nombre_semaforo);
t_puntero operaciones_kernel_reservar_test(t_valor_variable);
void operaciones_kernel_liberar_test(t_puntero);
t_descriptor_archivo operaciones_kernel_abrir_test(t_direccion_archivo,
		t_banderas);
void operaciones_kernel_borrar_test(t_descriptor_archivo);
void operaciones_kernel_cerrar_test(t_descriptor_archivo);
void operaciones_kernel_moverCursor_test(t_descriptor_archivo, t_valor_variable);
void operaciones_kernel_escribir_test(t_descriptor_archivo, void*,
		t_valor_variable);
void operaciones_kernel_leer_test(t_descriptor_archivo, t_puntero,
		t_valor_variable);

// Variables
AnSISOP_funciones funciones_test;
AnSISOP_kernel kernel_operaciones_test;
bool stack_overflow;
bool termino_programa;
bool llamada_a_funcion;
t_list* paginas_truchas;

#endif

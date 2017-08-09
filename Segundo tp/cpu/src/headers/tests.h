#ifndef HEADERS_TESTS_H_
#define HEADERS_TESTS_H_

#include "structs/structs.h"
#include "ctype.h"
#include "stdbool.h"
#include <parser/parser.h>
#include "primitivas.h"

void correrTest();
void init();
void testDefinirVariableYObtenerPosicion();
void ejecutarPCB();
void cargar_codigo_programa();
void liberar();
void imprimirTodasLasInstrucciones();
void cargarIndiceDeCodigo();
void imprimirCadaSentencia();
void testEjecutarInstruccion();
void obtenerInstruccion(int32_t, char*);
void testLeerDeMemoria();
void testLeerInstruccionDeMemoria();
void crearPcbTrucho();
void cargarCodigo();
void ejecutar();
void correrTestPrimitivas();
void testSerializar();
void inicializarPCB();
void testAbrirArchivo();

char* codigoPrograma;
int32_t tamanioPagina;
t_pcb* pcbEjecutandoTest;

#endif

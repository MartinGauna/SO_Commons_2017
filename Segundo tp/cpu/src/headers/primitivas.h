#ifndef HEADERS_PRIMITIVAS_H_
#define HEADERS_PRIMITIVAS_H_

// Incluciones
#include <stdio.h>
#include <stdlib.h>
#include <parser/parser.h>
#include <parser/metadata_program.h>
#include <ctype.h>
#include <structs/structs.h>
#include "cpu.h"
#include <commons/collections/list.h>
#include "pedidosMemoria.h"
#include "auxiliares.h"

// Funciones
void inicializarPrimitivas();
t_puntero funciones_definirVariables(t_nombre_variable);
t_puntero funciones_obtenerPosicionVariable(t_nombre_variable);
t_valor_variable funciones_dereferenciar(t_puntero);
void funciones_asignar(t_puntero, t_valor_variable);
t_valor_variable funciones_obtenerValorCompartida(t_nombre_compartida);
t_valor_variable funciones_asignarValorCompartida(t_nombre_compartida,
		t_valor_variable);
void funciones_irAlLabel(t_nombre_etiqueta);
void funciones_llamarSinRetorno(t_nombre_etiqueta);
void funciones_llamarConRetorno(t_nombre_etiqueta, t_puntero);
void funciones_finalizar(void);
void funciones_retornar(t_valor_variable);

// Operaciones Kernel
void operaciones_kernel_wait(t_nombre_semaforo);
void operaciones_kernel_signal(t_nombre_semaforo);
t_puntero operaciones_kernel_reservar(t_valor_variable);
void operaciones_kernel_liberar(t_puntero);
t_descriptor_archivo operaciones_kernel_abrir(t_direccion_archivo, t_banderas);
void operaciones_kernel_borrar(t_descriptor_archivo);
void operaciones_kernel_cerrar(t_descriptor_archivo);
void operaciones_kernel_moverCursor(t_descriptor_archivo, t_valor_variable);
void operaciones_kernel_escribir(t_descriptor_archivo, void*, t_valor_variable);
void operaciones_kernel_leer(t_descriptor_archivo, t_puntero, t_valor_variable);

// Variables
AnSISOP_funciones funciones;
AnSISOP_kernel kernel_operaciones;
int32_t tamanioPagina;

#endif

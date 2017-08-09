#ifndef HEADERS_PEDIDOSMEMORIA_H_
#define HEADERS_PEDIDOSMEMORIA_H_

// Inclusiones
#include "cpu.h"
#include "ejecucion.h"

// Funciones
void pedirInstruccionAMemoria(int32_t);
t_valor_variable pedido_de_lectura_a_memoria(t_direccion_logica);
t_valor_variable respuesta_de_lectura_a_memoria();
void pedido_lectura_de_codigo_a_memoria(t_direccion_logica, char*);
void pedido_de_asignacion_a_memoria(t_direccion_logica,
		t_valor_variable);

#endif

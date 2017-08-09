#ifndef HEADERS_CONEXIONCPUMEMORIA_H_
#define HEADERS_CONEXIONCPUMEMORIA_H_

#include "cpu.h"

void conectarseAMemoria();
void escucharMensajesDeMemoria();
void procesarMensajeDeMemoria(PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU);
t_valor_variable pedido_de_lectura_a_memoria(t_direccion_logica);
t_valor_variable respuesta_de_lectura_a_memoria();

#endif

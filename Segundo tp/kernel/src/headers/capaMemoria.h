#ifndef HEADERS_CAPAMEMORIA_H_
#define HEADERS_CAPAMEMORIA_H_

#include "kernel.h"
#include "commons/string.h"

#define METADATA_BLOQUE_SIZE  5

void pedido_bloque_memoria(int32_t);
void liberar_bloque(int32_t);

#endif


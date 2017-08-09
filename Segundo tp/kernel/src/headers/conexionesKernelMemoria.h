#ifndef HEADERS_CONEXIONESKERNELMEMORIA_H_
#define HEADERS_CONEXIONESKERNELMEMORIA_H_

#include "kernel.h"
#include <structs/structs.h>
#include <parser/metadata_program.h>

int redondearDivisionPaArriba(int, int);
int pedirAMemoriaInicializacionDeProceso(t_pcb**, char*);
int obtenerCantidadDePaginasParaIniciarProceso(char*);
bool escrituraExitosa(t_direccion_logica*, char*,
		PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU);
void enviarCodigoDePrograma(int32_t, char*);
void pedirAMemoriaFinalizarProceso(int32_t);

#endif

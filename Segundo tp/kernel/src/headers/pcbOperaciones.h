#ifndef HEADERS_PCBOPERACIONES_H_
#define HEADERS_PCBOPERACIONES_H_

// Inclusiones
#include "kernel.h"
#include <structs/structs.h>
#include "planificacion.h"

// Funciones
t_pcb* crearPcb();
void pasarPcbDeEjecutandoABloqueados(t_pcb**);
void pasarPcbDeNuevoAListos(t_pcb**);
void pasarPcbDeListosAEjecutando(t_pcb**);
void pasarPcbDeEjecutandoAListos(t_pcb**);
void pasarPcbDeEjecutandoABloqueados(t_pcb**);
void pasarPcbDeEjecutandoAFinalizados(t_pcb**);
void pasarPcbDeListoAFinalizados(t_pcb**);
void pasarPcbDeNuevoAFinalizados(t_pcb**, EXIT_CODE_TYPE);
int32_t buscarPidByProgramaSocket(int32_t);
void eliminarPidProgramaSocket(int32_t);
void cargarIndiceCodigo(t_pcb**);
void reemplazarPcbEnEjecutando(t_pcb*);
t_pcb* buscarPcbPorPID(int32_t);
int32_t buscarProgramaSocketByPid(int32_t);
void pasarPcbAFinalizados(t_pcb**);
void pasarPcbDeBloqueadoAfinalizados(t_pcb**, EXIT_CODE_TYPE);
int estaElProcesoEnEjecucion(int32_t);
void pasarPcbDeBloqueadoAListo(t_pcb**);
entrada_info_proceso* obtenerInfoProceso(t_pcb* pcb);

#endif

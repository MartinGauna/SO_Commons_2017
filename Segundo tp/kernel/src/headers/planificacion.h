#ifndef HEADERS_PLANIFICACION_H_
#define HEADERS_PLANIFICACION_H_

#include "kernel.h"
#include "pcbOperaciones.h"
#include "conexionesKernelMemoria.h"

void crearNuevoProcesoListo(t_pcb**);
void planificar();
t_pcb* obtenerProxProcesoAEjecutar();
void moverCpuAOcupadas(int);
bool checkNivelMultiprogramacion();
void ejecutarSiguientePcbEnCpu(CPU*);
void pasarProximoProcDeNewAReady();
bool estaElflagDeTerminacionDeProcesoActivo(int32_t);
bool checkTengoQueMatarPIDEnEjecucion(int);
void pedidoAbortarProgramaDeConsola(int32_t, bool);
void eliminarCpuDeOcupadas(CPU*);
void informarAconsolaTerminacionDeProceso(int32_t, EXIT_CODE_TYPE);
void agregarRafagaEjecutadaAPcb(int32_t);
void matarProceso(int32_t);
void agregarSyscallAPcb(int32_t);
entrada_programaSocket_pid* buscarHiloProgramaByPID(int32_t);
void checkSiDejoArchivosAbiertos(int32_t);
void borrarPrograma(entrada_programaSocket_pid*);
void pedidoAbortarProgramaDeCpu(int32_t);

#endif

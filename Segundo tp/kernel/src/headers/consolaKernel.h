#ifndef HEADERS_CONSOLAKERNEL_H_
#define HEADERS_CONSOLAKERNEL_H_

#include "kernel.h"

void printListos();
int cantProcesosListos();
int cantProcesosFinalizados();
int cantProcesosEjecutando();
int cantProcesosBloqueados();
int cantProcesosNuevos();
void mostrarTodosLosProcesos();
void printEjecutados();
void mostrarTablaArchivosDeProceso(int32_t);
void mostrarTablaArchivosGlobal();
void imprimirRafagasEjecutadasPorPid(int32_t);
void imprimirSyscallsEjecutadasPorPid(int32_t);
void printBloqueados();
void printFinalizados();
void printNuevos();
void mostrarHeapProceso(int32_t);
int obtenerCantidadPaginasHeap(int32_t);
void procesarInput2(char*);

#endif

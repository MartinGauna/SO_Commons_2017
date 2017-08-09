#ifndef HEADERS_CONEXIONESKERNELCPU_H_
#define HEADERS_CONEXIONESKERNELCPU_H_

#include <stdio.h>
#include <stdlib.h>
#include <connections/networkHandler.h>
#include <protocolos/protocols.h>
#include <structs/structs.h>
#include "capaMemoria.h"
#include "planificacion.h"
#include "capaFs.h"
#include "manejo_heap.h"

void * nuevoCpuConectado(int);
void removerCpuDeLaLista(int);
void agregarCpuALaLista(int);
void mensajeRecibidoDeCpu(int);
CPU* buscarCPUBySocket(int32_t);
void terminarProcesoEjecutandoEnCpu(int);
void obtener_valor_compartida(int32_t, PROTOCOLO_CPU_A_KERNEL);
void asignar_valor_compartida(int32_t, PROTOCOLO_CPU_A_KERNEL);
void pedido_wait(int32_t);
void pedido_signal(int32_t);
t_semaforo* existeSemaforo(int32_t);
bool existeCompartida(int32_t, t_valor_variable*, PROTOCOLO_CPU_A_KERNEL);

#endif

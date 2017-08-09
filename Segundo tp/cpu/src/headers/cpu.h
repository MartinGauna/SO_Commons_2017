#ifndef CPU_H_
#define CPU_H_

// Definiciones
#define LOG_PATH "info_logs"
#define LOG_EJECUCION "info_ejecucion"

// Inclusiones
#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/string.h>
#include <protocolos/protocols.h>
#include <commons/log.h>
#include <connections/networkHandler.h>
#include <connections/cliente.h>
#include <parser/metadata_program.h>
#include <parser/parser.h>
#include <structs/structs.h>
#include "conexionCpuMemoria.h"
#include "conexionCpuKernel.h"
#include "primitivas.h"

// Estructuras
typedef struct {
	char* puertoKernel;
	char* ipKernel;
	char* puertoMemoria;
	char* ipMemoria;
} t_cpu_config;

// Funciones
void cargarConfig(char**);
void liberarMemoria();
void imprimirConfig();
void logearConfig();
void iniciarCpu(int32_t, char**);
void destruir_pcb();
void desconectarse();
void inicializarSeniales();
void inicializarSemaforos();

// Variables Globales
t_cpu_config* cpu_config;
t_log* infoLogger;
t_log* infoEjecucion;
t_pcb* pcbActual;

bool desconexion, pcb_ejecutando;

int32_t kernelSocket;
int32_t memoriaSocket;

pthread_mutex_t semaforoDesconexion;

#endif

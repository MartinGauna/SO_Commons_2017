#ifndef CONSOLA_H_
#define CONSOLA_H_

// Inclusiones
#include <protocolos/protocols.h>
#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/log.h>
#include <connections/cliente.h>
#include <connections/networkHandler.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <commons/temporal.h>
#include <commons/txt.h>
#include <commons/collections/dictionary.h>
#include <structs/structs.h>
#include <unistd.h>
#include "tests.h"
#include "comandos.h"
#include "time/time.h"

// Definiciones
#define LOG_PATH "info_logs"
#define LOG_EJECUCION "info_ejecucion"
#define MAX_STRING_SIZE 256

// Protocolos
typedef enum PROTOCOLO_COMANDOS_CONSOLA {

	INICIAR, FINALIZAR, DESCONECTAR, LIMPIAR, HELP

} PROTOCOLO_COMANDOS_CONSOLA;

// Estructuras
typedef struct {
	char* ipKernel;
	char* puertoKernel;
} t_consola_config;

typedef struct {
	int32_t pid;
	int32_t tamanioCodigo;
	int32_t kernelSocket;
	char* path;
	char* codigo;
	pthread_t hiloPrograma;
	int32_t impresiones;
	char* time_init;
	char* time_fin;
	char* time_total;
} t_programa;

// Funciones
void cargarConfig(char**);
void liberarMemoria();
void conectarseAlKernel();
void handshake();
void cargarConfig(char**);
void liberarMemoria();
int32_t conectarKernel();
void imprimirInfoFinal();
int32_t enviarTextoAlKernel(int32_t);
void recibirComandos();
void* enviarAEjecutarPrograma(void*);
void iniciarConsola(int32_t, char**);
void contarCaracteresYObtenerCodigo(t_programa*);
void programaEjecutando();
void eliminarProgramasEnEjecucion();
void impresionPrograma(t_programa* programa);
void eliminarPrograma(void*);
void finalizarProcesoQueTerminoDeEjecutar(int32_t, EXIT_CODE_TYPE);
void freeVector(char**);
void imprimirFinPrograma(t_programa*);

// Variables Globales
t_consola_config* consola_config;
t_log* infoLogger;
t_log* info_ejecucion;
t_list* programas;
t_dictionary* comandos;

int32_t resultadoComando;

// Semaforos
pthread_mutex_t semEliminarPrograma;
pthread_mutex_t semListaProgramas;

#endif

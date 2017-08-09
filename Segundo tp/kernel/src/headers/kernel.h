#ifndef KERNEL_H_
#define KERNEL_H_

#include "conexionesKernelConsola.h"
#include "tests.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <connections/networkHandler.h>
#include <connections/cliente.h>
#include <protocolos/protocols.h>
#include <structs/structs.h>
#include <sys/inotify.h>
#include "consolaKernel.h"

#define LOG_PATH "info_logs"
#define SUCCESS 1
#define FAILURE 0

#define FILE_EVENT_SIZE ( sizeof (struct inotify_event) + 40 )
#define BUF_LEN     ( 1024 * FILE_EVENT_SIZE )

typedef struct {
	char* puertoProg;
	char* puertoCPU;
	char* ipMemoria;
	char* puertoMemoria;
	char* ipFS;
	char* puertoFS;
	int32_t quantum;
	int32_t quantumSpeed;
	char* algoritmo;
	int32_t gradoMultiprog;
	char** semIDs;
	char** semInit;
	char** sharedVars;
	int32_t stackSize;
	int32_t pageSize;
} t_kernel_config;

typedef struct {
	t_list* tabla;
	t_pcb * pcb;
} tabla_archivos_proceso;

//lista_de_bloques tiene t_bloque;

typedef enum {
	C_PROCESOS_EN_SISTEMA = 1,
	C_INFO_DE_PROCESO = 2,
	C_TABLA_GLOBAL_ARCHIVOS = 3,
	C_MODIFICAR_MULTIPROG = 4,
	C_FINALIZAR_PID = 5,
	C_DETENER_PLANIFICACION = 6,

} comandos_kernel;

typedef enum {
	C_TODOS = 1,
	C_NUEVOS = 2,
	C_EJECUTANDO = 3,
	C_TERMINADOS = 4,
	C_BLOQUEADOS = 5,
	C_LISTOS = 6
} subcomandos_procesos_en_sistema;

typedef enum {
	C_RAFAGAS_EJECUTADAS = 1,
	C_OPERACIONES_PRIVILEGIADAS_EJECUTADAS = 2,
	C_TABLA_DE_ARCHIVOS_ABIERTOS = 3,
	C_PAGS_HEAP = 4,
	C_CANTIDAD_ALLOCS = 5,
	C_CANTIDAD_FREES = 6,
	C_CANTIDAD_SYSCALLS = 7,
} subcomandos_info_de_proceso;

typedef struct {
	int32_t pid;
	t_list* pft; //process file table
} pid_pft;


typedef struct{
	int32_t pid;
}entrada_destruir_proceso;

void conectarAMemoria();
void conectarAfilesystem();
void cargarConfig();
void liberarMemoria();
void freeVector(char**);
void imprimirConfig();
void freeAll(); //libera toda la memoria reservada
void levantarServidorKernelConsola();
void correrTests();
void* procesarConexionEntrante(int socket, char* puerto);
void* procesarMensajeEntrante(int socket);
void mostrarPorPantallaComandos();
t_bloque* buscar_ultimo_bloque(t_list*);
void inicializarVariablesGlobales();
void inicializarSemaforos();
void escucharEventosDeArchivoDeConfiguracion(char* path);
int escucharCambiosArchivoConfig();
void eventoEnLosArchivosDetectado(int file_descriptor);
int cuantosSlashesTiene(char* path);

int watch_descriptor; // descriptor devuelto por inotify con add_watch()
int inotify_descriptor; //sobre este se escuchan los cambios

fd_set socketsSet;
fd_set socketsCpuSet;
fd_set socketsConsolaSet;
int memoriaSocket;
int fsSocket;
bool testSinMemoria;
char* configPath;
char* nombreArchivoConfiguracion;
bool recargarConfig;

t_list* lista_de_bloques;
t_list* cpusLibres;
t_list* cpusOcupadas;
t_list* colaListos;
t_list* colaEjecutando;
t_list* colaFinalizados;
t_list* colaBloqueados;
t_list* tablaHeap;
t_list* listaProgramaSocketPid;
t_list* colaNuevos;
t_list* globalFileTable;
t_dictionary* dicPID_PFT;
t_list* variables_globales;
t_list* semaforos;
t_list* listaInfoProcesos; //mantiene informacion para devolver en la consola del kernel
t_list* procesosADestruir; //lista que contiene LOS PID de los procesos que deben matarse


t_kernel_config* kernel_config;
t_log* infoLogger;

int32_t maxPID; //almacena el PID mas alto para la creacion de proximos PCBs
int32_t cantidadDeProcesosEnSistema;
bool planificacionActivada;

#endif

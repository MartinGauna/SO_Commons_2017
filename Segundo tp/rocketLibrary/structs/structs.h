#ifndef STRUCTS_STRUCTS_H_
#define STRUCTS_STRUCTS_H_

// Inclusiones
#include <commons/collections/list.h>
#include <parser/parser.h>
#include <commons/log.h>
#include <parser/metadata_program.h>

// Enums
typedef enum {
	FINALIZADO_CORRECTAMENTE = 0,
	NO_SE_PUDO_ASIGNAR_RECURSO = -1,
	ACCESO_ARCHIVO_INEXISTENTE = -2,
	INTENTO_LEER_ARCHIVO_SIN_PERMISO = -3,
	INTENTO_ESCRIBIR_ARCHIVO_SIN_PERMISO = -4,
	EXCEPCION_MEMORIA = -5,
	DESCONEXION_CONSOLA = -6,
	FINALIZADO_POR_COMANDO_DE_CONSOLA = -7,
	MALLOC_MAYOR_A_PAGESIZE = -8,
	NO_SE_PUEDEN_DAR_MAS_PAGINAS = -9,
	FALLO_MULTIPROGRAMACION = -10,
	DESCONEXION_CPU = -11,
	INTENTO_ACCEDER_A_UNA_VARIABLE_COMPARTIDA_INEXISTENTE = -12,
	INTENTO_OPERAR_UN_SEMAFORO_INEXISTENTE = -13,
	STACKOVERFLOW = -14,
	ESPACIO_INSUFICIENTE_PARA_CREAR_ARCHIVO = -15,
	NO_DEFINIDO = -19,
	FINALIZADO_POR_COMANDO = -20,

} EXIT_CODE_TYPE;

typedef enum {
	EJECUTANDO, LISTO, NUEVO, BLOQUEADO, TERMINADO
} ESTADO_DE_PROCESO;

typedef enum {
	CODIGO, STACK, HEAP, VACIO
} TIPO_DE_PAGINA;

typedef enum {
	FRAME_VACIO = -1, FRAME_RESERVADO = -2
} NOMENCLATURA_DE_FRAMES;

typedef enum{
	CHAR, INT, METADATA
} TIPO_DE_DATOS;

// Estructuras
typedef struct {
	int32_t pid;
	int32_t paginaId;
	int32_t offset;
	int32_t tamanio;
}__attribute__ ((__packed__)) t_direccion_logica;

typedef struct {
	int32_t pid;
	int32_t programaSocket;
} entrada_programaSocket_pid;

typedef struct {
	int32_t pid;
	int32_t rafagas_ejecutadas;
	int32_t syscall_ejecutadas;
	int32_t cantidadAllocs;
	int32_t totalAllocado;
	int32_t  totalLiberado;
	int32_t cantidadLiberaciones;
	char* sem_bloqueado_por;
} entrada_info_proceso;

typedef struct {
	char* nombre;
	int32_t abiertos;
} entrada_gft; //global file table

typedef char t_bool;

typedef struct {
	int32_t size;
	t_bool isFree;
} heap_metadata;

typedef struct {
	int32_t PID;
	int32_t numeroPagina;
	int32_t espacioDisponible;
	t_list * lista_de_bloques;
	int32_t espacio_bloque_maximo;
} nodo_tabla_heap;

typedef struct {
	int32_t nro_bloque;
	int32_t size;
	bool isUsed;
	int offset;
	int fin;
} t_bloque;

typedef struct {
	int32_t cursor;
	t_descriptor_archivo global_fd;
	char* flags;
} entrada_pft; //process file table

typedef struct {
	char id;
	t_direccion_logica posicion;
}__attribute__ ((__packed__)) t_variable;

typedef struct {
	t_nombre_variable* identificador;
	t_valor_variable valor;
}__attribute__ ((__packed__)) t_variable_global;

typedef struct{
	t_nombre_semaforo* nombreSemaforo;
	t_valor_variable valor;
}__attribute__ ((__packed__)) t_semaforo;

typedef struct {
	int32_t posicion;
	t_list* argumentos; //Lista de variables argumentos que recibe la funcion.
	t_list* variables; //Lista de variables locales de la funcion.
	int32_t pos_retorno; //Numero de Linea de codigo al que debe volver el program counter al finalizar la funcion
	t_direccion_logica pos_var_retorno; //Variable unica en donde se debe grabar el retorno de la funcion.
}__attribute__ ((__packed__)) t_registro_stack;

typedef struct {
	bool termino_programa;
	bool llamada_a_funcion;
	bool cambio_proceso;
	bool terminacion_anormal;
	bool se_llamo_a_wait;
	bool desconexion;
	bool ejecutando;
	bool llamada_sin_retorno;
}__attribute__ ((__packed__)) t_banderas_ejecucion;

typedef struct {

	int32_t PID; //process ID
	int32_t PC; //program counter
	int32_t SP; //stack pointer
	int32_t cantidad_paginas_totales;
	ESTADO_DE_PROCESO estado;
	EXIT_CODE_TYPE EC;

	t_puntero_instruccion instruccion_inicio;
	t_size instrucciones_size;
	t_list* indice_codigo;

	t_size etiquetas_size;
	char* indice_etiquetas;

	int32_t cantidad_de_funciones;
	int32_t cantidad_de_etiquetas;

	int32_t cant_entradas_indice_stack;
	t_list* indice_stack;
	t_direccion_logica fin_stack;

	t_banderas_ejecucion banderas;

}__attribute__((__packed__)) t_pcb;

typedef struct {
	char* puerto;
	int32_t marcos;
	int32_t marcoSize;
	int32_t entradasCache;
	int32_t cacheXProc;
	char* reemplazoCache;
	int32_t retardoMemoria;
	int32_t stackSize;
}__attribute__ ((__packed__)) t_memoria_config;

typedef struct {
	int32_t pid;
	int32_t paginaId;
}__attribute__ ((__packed__)) registroMarcoPagina;

typedef struct {
	int32_t cantidadPaginas;
	int32_t pid;
}__attribute__ ((__packed__)) registroCantidadPaginasPorProceso;

typedef struct {
	int32_t pid;
	int32_t paginaId;
	void* pagina;
}__attribute__ ((__packed__)) registroCacheMemoria;

typedef struct {
	int32_t pid;
	int32_t cantidadPaginasCacheadas;
	int32_t ultimaPaginaCacheada;
}__attribute__ ((__packed__)) registroAuxCache;

typedef struct {
	TIPO_DE_PAGINA tipo;
}__attribute__ ((__packed__)) registroTipoPagina;

typedef struct {
	void* memoria;
	t_list* cache;
	t_memoria_config* memoria_config;
	t_list* tablaPaginas;
	t_list* contadorAuxCache;
	t_log* logger;
	pthread_mutex_t mutex_cache;
	t_list* tipoDePaginas;
	int lecturas;
	int escrituras;
	int colisiones;
	int aciertosDeCache;

}__attribute__ ((__packed__)) t_estructurasMemoria;

typedef struct {
	int32_t socket;
	int32_t PID_en_ejecucion;
	int32_t quantumAcumulado;
}__attribute__ ((__packed__)) CPU;

// Funciones
t_puntero posicionMemoria_a_puntero(t_direccion_logica posMemoria,
		int32_t tamanio_pagina);
t_direccion_logica puntero_a_posicionMemoria(t_puntero poslogica,
		int32_t tamanio_pagina, int32_t pid);
void enviarPcb(int socket, t_pcb* pcb, t_log* infoLogger);
t_pcb* recibirPcb(int socket, t_log* infoLogger);
char* itoa(int32_t n);

#endif


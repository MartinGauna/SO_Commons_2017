#include "headers/memoria.h"

#include <protocolos/protocols.h>
#include <sys/types.h>
#include <pthread.h>
#include "headers/dumps.h"
#include "headers/gestionDeMemoria.h"
#include "headers/estructurasAdministrativas.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void imprimirAyuda() {
	printf(
			"-------------------------------------------------------------------\n");
	printf(
			"Bienvenido a la consola de la memoria, a continuacion se listan los comandos posibles: \n\n");
	printf(
			"-------------------------------------------------------------------\n");
	printf("1. Modificar el retardo de acceso a memoria: \n retardo [segundos]\n");
	printf("2. Realiza un dump completo de una de las tres opciones: (omitir FILE_PATH para imprimir por pantalla, pid=0 para todos los procesos) "
					"\n dump [cache | estructuras | contenido [pid] ][FILE_PATH]\n");
	printf("3. Realiza un flush de la memoria cache: \n flush\n");
	printf("4. Informa el size total de la memoria, o de un proceso en particular indicado en el parametro pid: "
					"\n size [memoria | pid]\n");
	printf("5. Informa las estadisticas de fallos de pagina y colisiones de la funcion [ESTADISTICAS] hash\n");
	printf("6. Ejecutar funcion hash para pid y pagina [HASH]\n");
	printf("\n");

}

void funcionConsola() {

	char* RETARDO = "RETARDO";
	char* DUMP = "DUMP";
	char* CACHE = "CACHE";
	char* FLUSH = "FLUSH";
	char* ESTRUCTURAS = "ESTRUCTURAS";
	char* CONTENIDO = "CONTENIDO";
	char* SIZE = "SIZE";
	char* MEMORIA = "MEMORIA";
	char* AYUDA = "AYUDA";
	char* ORDEN_CACHE = "ORDEN";
	char* ESTADISTICAS = "ESTADISTICAS";
	char* HASH = "HASH";

	printf("Consola iniciada");
	imprimirAyuda();
	char* msg = malloc(100);
	while (1) {
		fgets(msg, 40, stdin);
		char** primerSplit = string_split(msg, "\n");
		if (primerSplit[0] == NULL)
			continue;
		char** listaParametros = string_split(primerSplit[0], " ");
		if (string_equals_ignore_case(listaParametros[0], RETARDO)) {
			if (listaParametros[1] != NULL){
				int retardo = atoi(listaParametros[1]);
				if (retardo == 0 && *listaParametros[1] != '0') {
					printf("Por favor, ingrese un numero como valor de retardo\n");
				} else {
					cambiarRetardo(estructurasMemoria, retardo);
					imprimirConfig();
				}
			}else{
				printf("Por favor, ingrese un numero como valor de retardo\n");
			}
		} else if (string_equals_ignore_case(listaParametros[0], DUMP)) {
			if (listaParametros[1] != NULL) {
				if (string_equals_ignore_case(listaParametros[1], CACHE)) {
					if (listaParametros[2] == NULL) {
						dumpCache(estructurasMemoria, NULL);
					} else {
						if (listaParametros[3] == NULL) {
							FILE* archivoDump = fopen(listaParametros[2], "w");
							dumpCache(estructurasMemoria, archivoDump);
							fclose(archivoDump);
						} else {
							printf(
									"El cuarto parametro ingresado es invalido: %s\n",
									listaParametros[3]);
						}
					}
				} else if (string_equals_ignore_case(listaParametros[1],
						ESTRUCTURAS)) {
					if (listaParametros[2] == NULL) {
						dumpTablaPaginas(estructurasMemoria, 0, NULL);
						dumpPaginasPorProceso(estructurasMemoria, 0, NULL);
					} else {
						if (listaParametros[3] == NULL) {
							FILE* archivoDump = fopen(listaParametros[2], "w");
							dumpTablaPaginas(estructurasMemoria, 0, archivoDump);
							dumpPaginasPorProceso(estructurasMemoria, 0,
									archivoDump);
							fclose(archivoDump);
						} else {
							printf(
									"El cuarto parametro ingresado es invalido: %s\n",
									listaParametros[3]);
						}
					}
				} else if (string_equals_ignore_case(listaParametros[1],
						CONTENIDO)) {
					if (listaParametros[2] != NULL) {
						int pid = atoi(listaParametros[2]);
						if (pid == 0 && *listaParametros[2] != '0') {
							printf("El tercer parametro es invalido: %s\n",
									listaParametros[2]);
						} else {
							if (listaParametros[3] != NULL) {
								FILE* archivoDump = fopen(listaParametros[3], "w");
								dumpContenido(estructurasMemoria, pid, archivoDump);
								fclose(archivoDump);
							} else {
								dumpContenido(estructurasMemoria, pid, NULL);
							}
						}
					} else {
						printf("El tercer parametro ingresado es invalido: %s\n",
								listaParametros[2]);
					}
				} else {
					printf("El segundo parametro ingresado es invalido: %s\n",
							listaParametros[1]);
				}
			} else{
				printf("El segundo parametro ingresado es invalido:\n");
			}
		} else if (string_equals_ignore_case(listaParametros[0], FLUSH)) {
			if (listaParametros[1] != NULL) {
				printf(
						"El segundo parametro es invalido, debe estar vacio: %s\n",
						listaParametros[1]);
			} else {
				flushCache(estructurasMemoria);
			}
		} else if (string_equals_ignore_case(listaParametros[0], ESTADISTICAS)) {
			if (listaParametros[1] != NULL) {
				printf(
						"El segundo parametro es invalido, debe estar vacio: %s\n",
						listaParametros[1]);
			} else {
				estadisticas(estructurasMemoria);
			}
		} else if (string_equals_ignore_case(listaParametros[0], HASH)) {
			if (listaParametros[1] == NULL || listaParametros[2] == NULL ) {
				printf(
						"El segundo y tercer parametro deben ser numericos\n");
			} else {
				int pid = atoi(listaParametros[1]);
				int pagina = atoi(listaParametros[2]);
				printf("Hash: %d", funcionHash(estructurasMemoria, pid, pagina));
			}
		} else if (string_equals_ignore_case(listaParametros[0], SIZE)) {
			if (listaParametros[1] != NULL) {
				if (string_equals_ignore_case(listaParametros[1], MEMORIA)) {
					if (listaParametros[2] == NULL) {
						dumpTamanioMemoria(estructurasMemoria, NULL);
					} else {
						printf("El tercer parametro ingresado es invalido: %s\n",
								listaParametros[2]);
					}
				} else if (listaParametros[1] != NULL) {
					int pid = atoi(listaParametros[1]);
					if (pid == 0 && *listaParametros[1] != '0') {
						printf("El tercer parametro ingresado es invalido: %s\n",
								listaParametros[2]);
					} else {
						dumpPaginasPorProceso(estructurasMemoria, pid, NULL);
					}
				}
			}else{
				printf("El segundo parametro es invalido");
			}
		} else if (string_equals_ignore_case(listaParametros[0], AYUDA)) {
			imprimirAyuda();
			//Solo para test
		} else if (string_equals_ignore_case(listaParametros[0], ORDEN_CACHE)) {
			int pid = atoi(listaParametros[1]);
			int paginaId = atoi(listaParametros[2]);
			t_direccion_logica* puntero = malloc(sizeof(t_direccion_logica));
			puntero->paginaId = paginaId;
			puntero->offset = 0;
			puntero->pid = pid;
			puntero->tamanio = 3;
			char* punteroLeido = malloc(3);
			//leerDatoInterna(estructurasMemoria, puntero, &punteroLeido);
		}
	}
}

int main(int argc, char* argv[]) {
	pthread_t* hiloConsola;
	crearHilo(hiloConsola, &funcionConsola, NULL);
	iniciarMemoria(argc, argv);
//	testEscribirSucesivo(estructurasMemoria);
	librocket_levantarServidorConMultiplesHilos(memoria_config->puerto, &servidorMemoria);
	liberarMemoria();

	return EXIT_SUCCESS;

}

void iniciarMemoria(int argc, char* argv[]) {

	if (argc < 2) {
		printf("No se paso por parametro el path de config \n");
		exit(1);
	}

	infoLogger = log_create(LOG_PATH, "memoria", false, LOG_LEVEL_INFO);

	cargarConfig(argv);
	crearEstructuras();
}

//se encarga de tratar el handshake de las nuevas conexiones y de distinguir si es kernel o cpu
void* servidorMemoria(int sockfd) {

	//se conecto un kernel o un cpu?
	PROTOCOLO_CONECTAR_A_MEMORIA protocoloConexion;
	int result = librocket_recibirMensaje(sockfd, &protocoloConexion,
			sizeof(PROTOCOLO_CONECTAR_A_MEMORIA), infoLogger);

	if (result == 0) {
		printf("socket %d desconectado \n", sockfd);
		return NULL;
	}
	if (protocoloConexion == HANDSHAKE_CONECTAR_KERNEL_A_MEMORIA) {
		printf("Se conecto el kernel \n");

		funcionKernel(sockfd);
	} else if (protocoloConexion == HANDSHAKE_CONECTAR_CPU_A_MEMORIA) {
		printf("Se conecto un cpu \n");
		funcionCpu(sockfd);
	} else {
		printf("Fallo el handshake, mato al hilo hasta la vista baby \n");

	}

	return NULL;
}

void funcionKernel(int socket) {

	PROTOCOLO_CONECTAR_A_MEMORIA conexionAceptada =
			HANDSHAKE_CONEXION_A_MEMORIA_ACEPTADA;
	librocket_enviarMensaje(socket, &conexionAceptada,
			sizeof(PROTOCOLO_CONECTAR_A_MEMORIA), infoLogger);

	librocket_enviarMensaje(socket, &memoria_config->marcoSize, sizeof(int32_t),
			infoLogger);

	librocket_recibirMensaje(socket, &(memoria_config->stackSize),
			sizeof(int32_t), infoLogger);

	PROTOCOLO_KERNEL_Y_CPU_A_MEMORIA tipoMensaje;
	while (1) {
		int result = librocket_recibirMensaje(socket, &tipoMensaje,
				sizeof(PROTOCOLO_KERNEL_Y_CPU_A_MEMORIA), infoLogger);
		if (result == 0) {
			printf("Se desconecto el kernel \n");
			return;
		}

		if (tipoMensaje == ENVIO_TEXTO_A_MEMORIA) {
			int32_t tamanioTexto = 0;
			librocket_recibirMensaje(socket, &tamanioTexto, sizeof(int32_t),
					infoLogger);
			char* texto = malloc(tamanioTexto + 1);
			librocket_recibirMensaje(socket, texto, tamanioTexto, infoLogger);
			printf("Se recibio un texto del kernel: %s \n", texto);

		}

		if (tipoMensaje == INICIALIZAR_PROGRAMA) {
			int32_t pid;
			librocket_recibirMensaje(socket, &pid, sizeof(int32_t), infoLogger);

			int32_t paginasFijas;
			librocket_recibirMensaje(socket, &paginasFijas, sizeof(int32_t),
					infoLogger);

			PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU respuesta = crearPaginasIniciales(
					estructurasMemoria, pid, paginasFijas);

			PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU iniciado;
			if (respuesta == PAGINAS_ASIGNADAS) {
				iniciado = PROCESO_INICIADO_EN_MEMORIA;
			} else {
				iniciado = MEMORIA_INSUFICIENTE;
			}

			librocket_enviarMensaje(socket, &iniciado,
					sizeof(PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU), infoLogger);
		}

		if (tipoMensaje == PEDIDO_ASIGNAR_PAGINAS) {
			int32_t pid;
			librocket_recibirMensaje(socket, &pid, sizeof(int32_t), infoLogger);

			int32_t paginasHeap;
			librocket_recibirMensaje(socket, &paginasHeap, sizeof(int32_t),
					infoLogger);

			PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU respuesta = asignarPaginasDeHeap(estructurasMemoria, pid, paginasHeap);

			librocket_enviarMensaje(socket, &respuesta,
					sizeof(PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU), infoLogger);
		}

		if (tipoMensaje == LEER_DE_MEMORIA) { // [PID][PAGINA][OFFSET][TAMANIO]
			t_direccion_logica* direccionLogica = malloc(
					sizeof(t_direccion_logica));
			librocket_recibirMensaje(socket, direccionLogica,
					sizeof(t_direccion_logica), infoLogger);

			char* punteroAEnviar = malloc(direccionLogica->tamanio);
			PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU respuesta = leerDato(
					estructurasMemoria, &punteroAEnviar, direccionLogica, CHAR);

			if (respuesta == LECTURA_SATISFACTORIA) {
				librocket_enviarMensaje(socket, &respuesta,
						sizeof(PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU), infoLogger);
				librocket_enviarMensaje(socket, punteroAEnviar,
						direccionLogica->tamanio, infoLogger);
			} else {
				librocket_enviarMensaje(socket, &respuesta,
						sizeof(PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU), infoLogger);
			}
			free(direccionLogica);
			free(punteroAEnviar);
		}

		if (tipoMensaje == ESCRIBIR_A_MEMORIA) { // [PID][PAGINA][OFFSET][TAMANIO][CONTENIDO]

			t_direccion_logica* direccionLogica = malloc(
					sizeof(t_direccion_logica));
			librocket_recibirMensaje(socket, direccionLogica,
					sizeof(t_direccion_logica), infoLogger);

			void* dato = malloc(direccionLogica->tamanio);
			librocket_recibirMensaje(socket, dato, direccionLogica->tamanio,
					infoLogger);

			PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU respuesta = escribirDato(estructurasMemoria, dato, direccionLogica, CHAR);

			free(direccionLogica);
			free(dato);
			if (respuesta == ESCRITURA_SATISFACTORIA) {
				librocket_enviarMensaje(socket, &respuesta,
						sizeof(PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU), infoLogger);
			} else {
				librocket_enviarMensaje(socket, &respuesta,
						sizeof(PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU), infoLogger);
			}
		}

		if (tipoMensaje == FINALIZAR_PROGRAMA) {

			int32_t pid;
			librocket_recibirMensaje(socket, &pid, sizeof(int32_t), infoLogger);

			PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU respuesta =
					finalizarProgramaEnMemoria(estructurasMemoria, pid);

			librocket_enviarMensaje(socket, &respuesta,
					sizeof(PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU), infoLogger);

		}
		if (tipoMensaje == LIBERAR_PAGINA) {

			int32_t pid;
			librocket_recibirMensaje(socket, &pid, sizeof(int32_t), infoLogger);

			int32_t paginaId;
			librocket_recibirMensaje(socket, &paginaId, sizeof(int32_t),
					infoLogger);

			PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU respuesta = liberarPagina(estructurasMemoria, pid, paginaId);

			librocket_enviarMensaje(socket, &respuesta,
					sizeof(PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU), infoLogger);

		}
	}
}

void funcionCpu(int socket) {

	PROTOCOLO_CONECTAR_A_MEMORIA conexionAceptada =
			HANDSHAKE_CONEXION_A_MEMORIA_ACEPTADA;
	librocket_enviarMensaje(socket, &conexionAceptada,
			sizeof(PROTOCOLO_CONECTAR_A_MEMORIA), infoLogger);

	PROTOCOLO_KERNEL_Y_CPU_A_MEMORIA tipoMensaje;
	while (1) {
		int result = librocket_recibirMensaje(socket, &tipoMensaje,
				sizeof(PROTOCOLO_KERNEL_Y_CPU_A_MEMORIA), infoLogger);
		if (result == 0) {
			printf("Se desconecto la cpu \n");
			return;
		}

		if (tipoMensaje == LEER_DE_MEMORIA) { // [PID][PAGINA][OFFSET][TAMANIO]
			t_direccion_logica* direccionLogica = malloc(
					sizeof(t_direccion_logica));
			librocket_recibirMensaje(socket, direccionLogica,
					sizeof(t_direccion_logica), infoLogger);

			void* punteroAEnviar = malloc(direccionLogica->tamanio);
			TIPO_DE_DATOS tipo;
			if (direccionLogica->tamanio == 4){
				tipo = INT;
			}else{
				tipo = CHAR;
			}
			PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU respuesta = leerDato(
					estructurasMemoria, &punteroAEnviar, direccionLogica, tipo);

			if (respuesta == LECTURA_SATISFACTORIA) {
				librocket_enviarMensaje(socket, &respuesta,
						sizeof(PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU), infoLogger);
				librocket_enviarMensaje(socket, punteroAEnviar,
						direccionLogica->tamanio, infoLogger);

			} else {
				librocket_enviarMensaje(socket, &respuesta,
						sizeof(PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU), infoLogger);
			}
			free(direccionLogica);
			free(punteroAEnviar);
		}

		if (tipoMensaje == ESCRIBIR_A_MEMORIA) { // [PID][PAGINA][OFFSET][TAMANIO][CONTENIDO]

			t_direccion_logica* direccionLogica = malloc(
					sizeof(t_direccion_logica));
			librocket_recibirMensaje(socket, direccionLogica,
					sizeof(t_direccion_logica), infoLogger);

			void* dato = malloc(direccionLogica->tamanio);
			librocket_recibirMensaje(socket, dato, direccionLogica->tamanio,
					infoLogger);

			PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU respuesta = escribirDato(
					estructurasMemoria, dato, direccionLogica, INT);
			free(direccionLogica);
			free(dato);
			if (respuesta == ESCRITURA_SATISFACTORIA) {
				librocket_enviarMensaje(socket, &respuesta,
						sizeof(PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU), infoLogger);
			} else {
				librocket_enviarMensaje(socket, &respuesta,
						sizeof(PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU), infoLogger);
			}
		}
	}

}

void cargarConfig(char** argv) {

	memoria_config = malloc(sizeof(t_memoria_config));
	memoria_config->reemplazoCache = string_new();
	memoria_config->cacheXProc = 0;
	memoria_config->entradasCache = 0;
	memoria_config->marcoSize = 0;
	memoria_config->marcos = 0;
	memoria_config->puerto = string_new();
	memoria_config->retardoMemoria = 0;

	char* path = string_new();
	string_append(&path, argv[1]);

	t_config* config = config_create(path);

	string_append(&(memoria_config->puerto),
			config_get_string_value(config, "PUERTO"));
	memoria_config->marcos = config_get_int_value(config, "MARCOS");
	memoria_config->marcoSize = config_get_int_value(config, "MARCO_SIZE");
	memoria_config->entradasCache = config_get_int_value(config,
			"ENTRADAS_CACHE");
	memoria_config->cacheXProc = config_get_int_value(config, "CACHE_X_PROC");
	string_append(&(memoria_config->reemplazoCache),
			config_get_string_value(config, "REEMPLAZO_CACHE"));
	memoria_config->retardoMemoria = config_get_int_value(config,
			"RETARDO_MEMORIA");

	imprimirConfig();

	free(path);
	config_destroy(config);

}

void imprimirConfig() {

	printf("Puerto: %s\n", memoria_config->puerto);
	printf("Marcos: %i\n", memoria_config->marcos);
	printf("Tamaño de marco: %i\n", memoria_config->marcoSize);
	printf("Entradas de Cache: %i\n", memoria_config->entradasCache);
	printf("Cache por proc: %i\n", memoria_config->cacheXProc);
	printf("Reemplazo de Cache: %s\n", memoria_config->reemplazoCache);
	printf("Retardo de memoria: %i\n", memoria_config->retardoMemoria);

}

void liberarMemoria() {

	free(memoria);
	int indice;
	free(tablaPaginas);
	for (indice = 0; indice < memoria_config->entradasCache; indice++) {
		registroCacheMemoria* registro = list_get(cache, indice);
		free(registro->pagina);
		free(registro);
	}
	while (cache->elements_count > 0)
		list_remove_and_destroy_element(cache, 0, destructorCache);
	free(memoria_config);
	while (contadorAuxCache->elements_count > 0)
		list_remove_and_destroy_element(contadorAuxCache, 0, destructorAuxCache);
	free(contadorAuxCache);
}

void crearEstructuras() {
	pthread_mutex_t mutex_cache;
	tipoDePaginas = list_create();
	if (pthread_mutex_init(&mutex_cache, NULL) != 0) {
		printf("\n mutex init failed\n");
		liberarMemoria();
		exit(1);
	}
	int indice;
	memoria = malloc(memoria_config->marcos * memoria_config->marcoSize);
	//Creo la tabla de paginas con los frames vacios y la aloco en memoria
	tablaPaginas = list_create();
	for (indice = 0; indice < memoria_config->marcos; indice++) {
		registroMarcoPagina* registro = memoria
				+ indice * sizeof(registroMarcoPagina);
		registro->pid = FRAME_VACIO;
		list_add(tablaPaginas, registro);
		registroTipoPagina* estruct = malloc(sizeof(registroTipoPagina));
		estruct->tipo = VACIO;
		list_add(tipoDePaginas, estruct);
	}
	//Tamanio de la tabla de pagina invertida
	int32_t tamanioTabla = sizeof(registroMarcoPagina) * memoria_config->marcos;
	int32_t cantidadPaginasNecesarias = tamanioTabla
			/ memoria_config->marcoSize;
	if ((tamanioTabla % memoria_config->marcoSize)) {
		cantidadPaginasNecesarias++;
	}
	//Marco las paginas reservadas para alocar la tabla
	for (indice = 0; indice < cantidadPaginasNecesarias; indice++) {
		registroMarcoPagina* registro = (registroMarcoPagina*) list_get(
				tablaPaginas, indice);
		registro->pid = FRAME_RESERVADO;
		registro->paginaId = 0;
	}

	contadorAuxCache = list_create();
	registroAuxCache* registro = malloc(sizeof(registroAuxCache));
	registro->pid = FRAME_VACIO;
	registro->cantidadPaginasCacheadas = memoria_config->entradasCache;
	registro->ultimaPaginaCacheada = -1;
	list_add(contadorAuxCache, registro);

	cache = list_create();

	estructurasMemoria = malloc(sizeof(t_estructurasMemoria));
	estructurasMemoria->cache = cache;
	estructurasMemoria->contadorAuxCache = contadorAuxCache;
	estructurasMemoria->memoria = memoria;
	estructurasMemoria->memoria_config = memoria_config;
	estructurasMemoria->tablaPaginas = tablaPaginas;
	estructurasMemoria->logger = infoLogger;
	estructurasMemoria->mutex_cache = mutex_cache;
	estructurasMemoria->tipoDePaginas = tipoDePaginas;
	estructurasMemoria->aciertosDeCache = 0;
	estructurasMemoria->colisiones = 0;
	estructurasMemoria->lecturas = 0;
	estructurasMemoria->escrituras = 0;
}

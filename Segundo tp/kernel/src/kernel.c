#include "headers/kernel.h"

/*
 * TODO:
 *
 * Aumentar cantidad de syscalls de los procesos
 */

//int main_bis(int argc, char* argv[]) {
int main(int argc, char* argv[]) {

	//main_bis();
	//FD_ZERO(&socketsConsolaSet);
	FD_ZERO(&socketsSet);
	recargarConfig = false;
	infoLogger = log_create(LOG_PATH, "kernel", false, LOG_LEVEL_INFO);
	planificacionActivada = true;

	configPath = string_new();
	string_append(&configPath, argv[1]);

	cargarConfig(argv);
	imprimirConfig();

	cpusOcupadas = list_create();
	cpusLibres = list_create();
	colaBloqueados = list_create();
	colaListos = list_create();
	colaEjecutando = list_create();
	colaFinalizados = list_create();
	colaNuevos = list_create();
	tablaHeap = list_create();
	listaProgramaSocketPid = list_create();
	globalFileTable = list_create();
	dicPID_PFT = dictionary_create();
	listaInfoProcesos = list_create();
	procesosADestruir = list_create();

	log_info(infoLogger,
			"----KERNEL INICIADO con algoritmo %s, stack size %d y grado de multiprogramacion %d-----",
			kernel_config->algoritmo, kernel_config->stackSize,
			kernel_config->gradoMultiprog);

	//me conecto a la memoria
	conectarAMemoria();

	//test_sin_memoria_2();

	//me conecto al filesystem
	conectarAfilesystem();

	//correrTests();

	mostrarPorPantallaComandos();

	//testEnviarCodigoDePrograma(1,"#!/usr/bin/ansisop #Respuesta esperada: 1; 1; Hola Mundo!; 3; Bye begin variables f,  A,  g  A = 	0 !compartida = 1+A print !compartida jnz !compartida Siguiente  :Proximo",2);

	testSinMemoria = false;

	char* STDIN = string_new();
	string_append(&STDIN, "0");
	inotify_descriptor = escucharCambiosArchivoConfig();
	librocket_levantarServidorDelKernel(kernel_config->puertoCPU,
			kernel_config->puertoProg, STDIN, inotify_descriptor,
			&procesarConexionEntrante, &socketsSet, &procesarMensajeEntrante);

	liberarMemoria();

	return EXIT_SUCCESS;

}

void mostrarPorPantallaComandos() {
	printf(
			"-------------------------------------------------------------------\n");
	printf(
			"Bienvenido a la consola del kernel, inserta cualquiera de los comandos siguientes: \n");
	printf(
			"Mostrar procesos por estado: procesos [NEW|READY|END|BLOCKED|EXECUTING|ALL]  \n");
	printf(
			"Informacion de un proceso: 'info' PID [rafagas|syscalls|archivos|heap] \n");
	printf("Tabla global de archivos: 'global'\n");
	printf(
			"Modificar el nivel de multiprogramacion: 'multiprogramacion' 'nuevo grado' \n");
	printf("Finalizar un proceso: 'kill' PID \n");
	printf("Detener planificacion del kernel: 'stop' \n");
	printf("\n");
}

void* procesarConexionEntrante(int socket, char* puerto) {
	if (string_equals_ignore_case(puerto, kernel_config->puertoCPU)) {
		printf("Se conecto una cpu\n");
		fflush(stdout);
		FD_SET(socket, &socketsCpuSet);
		nuevoCpuConectado(socket);
	} else if (string_equals_ignore_case(puerto, kernel_config->puertoProg)) {
		printf("Se conecto una consola\n");
		fflush(stdout);
		FD_SET(socket, &socketsConsolaSet);

	}
	return NULL;
}

void* procesarMensajeEntrante(int socket) {
	if (FD_ISSET(socket, &socketsCpuSet)) {
		mensajeRecibidoDeCpu(socket);
	} else if (FD_ISSET(socket, &socketsConsolaSet)) {
		mensajeRecibidoDeConsola(socket);
	} else if (socket == 0) {
		//mensaje recibido desde la consola
		//TODO procesar consola kernel
		char* msj = malloc(80);
		fgets(msj, 80, stdin);
		procesarInput2(msj);
	} else if (socket == inotify_descriptor) {
		//es el descriptor de eventos de archivos (inotify)
		eventoEnLosArchivosDetectado(inotify_descriptor);
	}
	return NULL;
}

void correrTests() {
	testRedondeoHaciaArriba();
	testObtenerCantidadDePaginasParaIniciarProceso();
	testCheckSiHayEspacioParaAsignarBloque();
}

void conectarAfilesystem() {

	libconnections_conectar_a_servidor(kernel_config->ipFS,
			kernel_config->puertoFS, &fsSocket);

	PROTOCOLO_KERNEL_A_FS handshakeFS = HANDSHAKE_CONECTAR_A_FS;

	librocket_enviarMensaje(fsSocket, &handshakeFS,
			sizeof(PROTOCOLO_KERNEL_A_FS), infoLogger);
	PROTOCOLO_FS_A_KERNEL mensajeDelFS;

	librocket_recibirMensaje(fsSocket, &mensajeDelFS,
			sizeof(PROTOCOLO_FS_A_KERNEL), infoLogger);

	if (mensajeDelFS == HANDSHAKE_CONEXION_ACEPTADA_AL_KERNEL) {
		printf("Conectado al filesystem");
	} else {
		printf("No me pude conectar al filesystem");
	}

}

void conectarAMemoria() {

	libconnections_conectar_a_servidor(kernel_config->ipMemoria,
			kernel_config->puertoMemoria, &memoriaSocket);

	PROTOCOLO_CONECTAR_A_MEMORIA mensajeKernelMemoria =
			HANDSHAKE_CONECTAR_KERNEL_A_MEMORIA;

	librocket_enviarMensaje(memoriaSocket, &mensajeKernelMemoria,
			sizeof(PROTOCOLO_CONECTAR_A_MEMORIA), infoLogger);
	PROTOCOLO_CONECTAR_A_MEMORIA respuestaConexion;
	librocket_recibirMensaje(memoriaSocket, &respuestaConexion,
			sizeof(PROTOCOLO_CONECTAR_A_MEMORIA), infoLogger);

	/*
	 * el kernel espera recibir PAGE SIZE y luego envia STACK SIZE
	 */
	if (respuestaConexion == HANDSHAKE_CONEXION_A_MEMORIA_ACEPTADA) {
		int32_t pageSize;
		librocket_recibirMensaje(memoriaSocket, &pageSize, sizeof(int32_t),
				infoLogger);
		kernel_config->pageSize = pageSize;
		librocket_enviarMensaje(memoriaSocket, &(kernel_config->stackSize),
				sizeof(int32_t), infoLogger);
		printf("Kernel conectado a la memoria exitosamente \n");
		log_info(infoLogger, "Kernel conectado a la memoria exitosamente");
	} else {
		printf("No me pude conectar a memoria, cerrando \n");
		exit(1);
	}

}

char* obtenerDirectorioPadreConfig() {
	int i = 0;
	char* temp = string_new();
	string_append(&temp, configPath);
	int slashes = cuantosSlashesTiene(configPath);
	char* tok;
	char* directorio = string_new();
	string_append(&directorio, "/");
	tok = strtok(temp, "/");
	for (; i < slashes - 1 && tok != NULL; i++) {
		string_append(&directorio, tok);
		string_append(&directorio, "/");
		tok = strtok(NULL, "/");
	}
	free(temp);
	return directorio;
}

int cuantosSlashesTiene(char* path) {
	int c = 0;
	char* temp = string_new();
	string_append(&temp, path);
	char* tok = strtok(temp, "/");
	while (tok != NULL) {
		c++;
		tok = strtok(NULL, "/");
	}
	free(temp);
	return c;
}

void cargarConfig() {

	kernel_config = malloc(sizeof(t_kernel_config));

	kernel_config->gradoMultiprog = 0;
	kernel_config->algoritmo = string_new();
	kernel_config->ipFS = string_new();
	kernel_config->ipMemoria = string_new();
	kernel_config->puertoCPU = string_new();
	kernel_config->puertoFS = string_new();
	kernel_config->puertoMemoria = string_new();
	kernel_config->puertoProg = string_new();
	kernel_config->quantum = 0;
	kernel_config->quantumSpeed = 0;
	kernel_config->stackSize = 0;

	char* path = string_new();
	string_append(&path, configPath);

	char* temp = string_new();
	string_append(&temp, path);
	nombreArchivoConfiguracion = strrchr(temp, '/');
	nombreArchivoConfiguracion =
			string_split(nombreArchivoConfiguracion, "/")[0];
	free(temp);


	t_config* config = config_create(path);

	string_append(&(kernel_config->puertoProg),
			config_get_string_value(config, "PUERTO_PROG"));
	string_append(&(kernel_config->puertoCPU),
			config_get_string_value(config, "PUERTO_CPU"));

	string_append(&(kernel_config->ipMemoria),
			config_get_string_value(config, "IP_MEMORIA"));

	string_append(&(kernel_config->ipFS),
			config_get_string_value(config, "IP_FS"));

	string_append(&(kernel_config->puertoFS),
			config_get_string_value(config, "PUERTO_FS"));
	string_append(&(kernel_config->puertoMemoria),
			config_get_string_value(config, "PUERTO_MEMORIA"));

	kernel_config->quantum = config_get_int_value(config, "QUANTUM");
	kernel_config->quantumSpeed = (config_get_int_value(config, "QUANTUM_SLEEP") * 1000);
	string_append(&(kernel_config->algoritmo),
			config_get_string_value(config, "ALGORITMO"));
	kernel_config->gradoMultiprog = config_get_int_value(config,
			"GRADO_MULTIPROG");
	kernel_config->semIDs = config_get_array_value(config, "SEM_IDS");
	kernel_config->semInit = config_get_array_value(config, "SEM_INIT");
	kernel_config->sharedVars = config_get_array_value(config, "SHARED_VARS");
	kernel_config->stackSize = config_get_int_value(config, "STACK_SIZE");

	inicializarVariablesGlobales();
	inicializarSemaforos();

	free(path);
	config_destroy(config);

}

void recargarQuantumSpeed() {
	log_info(infoLogger,
			"Recargando quantum speed del archivo de configuracion");
	t_config* config = config_create(configPath);
	kernel_config->quantumSpeed = 0;
	kernel_config->quantumSpeed = config_get_int_value(config, "QUANTUM_SLEEP") * 1000;
	printf("El nuevo quantum speed es %d \n", kernel_config->quantumSpeed / 1000);
	config_destroy(config);


}

void liberarMemoria() {

	free(kernel_config->algoritmo);
	free(kernel_config->ipFS);
	free(kernel_config->ipMemoria);
	freeVector(kernel_config->semIDs);
	freeVector(kernel_config->semInit);
	freeVector(kernel_config->sharedVars);
	free(kernel_config);

	log_destroy(infoLogger);

}

void freeVector(char** vector) {

	int cantidad = 0;
	int posicion = 0;
	int total;

	while (vector[cantidad] != NULL) {
		cantidad++;
	}

	for (total = cantidad; total > 0; total--) {

		free(vector[posicion]);
		posicion++;

	}
}

void imprimirConfig() {

	printf("Puerto Prog: %s\n", kernel_config->puertoProg);
	printf("Puerto CPU: %s\n", kernel_config->puertoCPU);
	printf("IP Memoria: %s\n", kernel_config->ipMemoria);
	printf("Puerto Memoria: %s\n", kernel_config->puertoMemoria);
	printf("IP File System: %s\n", kernel_config->ipFS);
	printf("Puerto File System: %s\n", kernel_config->puertoFS);
	printf("Quantum: %i\n", kernel_config->quantum);
	printf("Quantum Speed: %i ms\n", kernel_config->quantumSpeed / 1000);
	printf("Algoritmo de planificacion: %s\n", kernel_config->algoritmo);
	printf("Grado de multiprogramacion: %i\n", kernel_config->gradoMultiprog);
	printf("Tamaño del Stack: %i\n", kernel_config->stackSize);

	int cantSemaforos = list_size(semaforos);
	int i = 0;
	for (; i < cantSemaforos; i++) {
		t_semaforo* sem = list_get(semaforos, i);
		printf("Semaforo %s con valor inicial %d \n", *(sem->nombreSemaforo),
				sem->valor);
	}
}

void inicializarVariablesGlobales() {

	variables_globales = list_create();

	int32_t contador = 0;
	t_variable_global* variable;
	while (kernel_config->sharedVars[contador] != NULL) {

		variable = malloc(sizeof(t_variable_global));

		variable->valor = 0;
		variable->identificador =
				(t_nombre_variable*) kernel_config->sharedVars[contador];

		list_add(variables_globales, variable);
		contador++;
	}

}

void inicializarSemaforos() {

	semaforos = list_create();

	int32_t contador = 0;
	t_semaforo* semaforo;
	while (kernel_config->semIDs[contador] != NULL) {
		semaforo = malloc(sizeof(t_semaforo) + 1);
		semaforo->valor = atoi(kernel_config->semInit[contador]);
		char* nombre = kernel_config->semIDs[contador];
		string_append(&nombre, "\0");
		semaforo->nombreSemaforo = &(kernel_config->semIDs[contador]);
		list_add(semaforos, semaforo);
		contador++;
	}

}

void eventoEnLosArchivosDetectado(int file_descriptor) {
	char buffer[BUF_LEN];
	int length = read(file_descriptor, buffer, BUF_LEN);
	if (length < 0) {
		perror("read");
	}

	int offset = 0;
	while (offset < length) {
		struct inotify_event* event = (struct inotify_event*) &buffer[offset];
		if (event->len) {
			if (event->mask & IN_MODIFY) {
				if (event->mask & IN_ISDIR) {
					log_info(infoLogger, "Directorio %s modificado",
							event->name);
				} else {
					if (string_equals_ignore_case(event->name,
							nombreArchivoConfiguracion)) {
						printf(
								"Se detecto que el archivo de configuracion  fue modificado \n");
						recargarConfig = true;
					}
				}
			}
		}
		offset += sizeof(struct inotify_event) + event->len;

	}

}

/*
 * Retorna el descriptor para escuchar eventos de archivos
 */
int escucharCambiosArchivoConfig() {
	int file_descriptor = inotify_init();
	if (file_descriptor < 0) {
		perror("inotify_init");
	}

	watch_descriptor = inotify_add_watch(file_descriptor,
			obtenerDirectorioPadreConfig(), IN_MODIFY);
	return file_descriptor;
}

void cerrarDescriptorInotify(int file_descriptor) {
	inotify_rm_watch(file_descriptor, watch_descriptor);
	close(file_descriptor);
}


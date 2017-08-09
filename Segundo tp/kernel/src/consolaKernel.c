#include "headers/consolaKernel.h"

void procesarInput2(char* input) {
	char* listaProcesos = "procesos";
	char* infoProceso = "info";
	char* tablaGlobal = "global";
	char* multiprogramacion = "multiprogramacion";
	char* killProceso = "kill";
	char* stopPlanificacion = "stop";
	char* reanudarPlanificacion = "restart";

	char** split1 = string_split(input, "\n");
	if (split1[0] == NULL)
		return;

	char** comando = string_split(split1[0], " ");

	if (string_equals_ignore_case(comando[0], listaProcesos)) {
		//LISTAR PROCESOS POR ESTADO
		if (comando[1] == NULL) {
			printf("No se pudo reconocer el comando \n");
			return;
		}
		if (string_equals_ignore_case(comando[1], "NEW")) {
			printNuevos();

		} else if (string_equals_ignore_case(comando[1], "READY")) {
			printListos();
		} else if (string_equals_ignore_case(comando[1], "END")) {
			printFinalizados();
		} else if (string_equals_ignore_case(comando[1], "BLOCKED")) {
			printBloqueados();
		} else if (string_equals_ignore_case(comando[1], "EXECUTING")) {
			printEjecutados();
		} else if (string_equals_ignore_case(comando[1], "ALL")) {
			mostrarTodosLosProcesos();
		} else {
			printf("No se pudo reconocer el comando \n");
		}
	} else if (string_equals_ignore_case(comando[0], infoProceso)) {
		//INFO DE UN PROCESO
		if (comando[1] == NULL || comando[2] == NULL) {
			printf("No se pudo reconocer el comando o esta incompleto \n");
			return;
		}
		int pid = atoi(comando[1]);
		if (string_equals_ignore_case(comando[2], "rafagas")) {
			imprimirRafagasEjecutadasPorPid(pid);
		} else if (string_equals_ignore_case(comando[2], "syscalls")) {
			//TODO
			imprimirSyscallsEjecutadasPorPid(pid);
		} else if (string_equals_ignore_case(comando[2], "archivos")) {
			mostrarTablaArchivosDeProceso(pid);
		} else if (string_equals_ignore_case(comando[2], "heap")) {
			mostrarHeapProceso(pid);
		} else {
			printf("No se pudo reconocer el comando \n");
			return;
		}
	} else if (string_equals_ignore_case(comando[0], tablaGlobal)) {
		//MOSTRAR TABLA GLOBAL DE ARCHIVOS
		mostrarTablaArchivosGlobal();
	} else if (string_equals_ignore_case(comando[0], multiprogramacion)) {
		if (comando[1] == NULL) {
			printf("Nuevo nivel de multiprogramacion no insertado \n");
		}
		int multiprog = atoi(comando[1]);
		kernel_config->gradoMultiprog = multiprog;
		printf("Modificado el grado de multiprogramacion a %d\n",
				kernel_config->gradoMultiprog);
		if(list_size(colaNuevos)> 0 && checkNivelMultiprogramacion()){
			int size = list_size(colaNuevos);
			int i = 0;
			for(;i<size && checkNivelMultiprogramacion();i++){
				pasarProximoProcDeNewAReady();
			}
		}
	} else if (string_equals_ignore_case(comando[0], killProceso)) {
		//MATAR UN PROCESO
		if (comando[1] == NULL) {
			printf("Falta el PID \n");
			return;
		}
		int32_t pid = atoi(comando[1]);
		if (pid == 0) {
			printf("PID invalido \n");
		}
		matarProceso(pid);
	} else if (string_equals_ignore_case(comando[0], stopPlanificacion)) {
		//DETENER PLANIFICACION
		planificacionActivada = false;
		printf("Planificacion detenida \n");
		log_info(infoLogger, "--SE DETUVO LA PLANIFICACION DE PROCESOS\n");
		//TODO
	} else if (string_equals_ignore_case(comando[0], reanudarPlanificacion)) {
		//REANUDAR PLANIFICACION
		printf("Planificacion reanudada \n");
		planificacionActivada = true;
		log_info(infoLogger, "---SE REANUDO LA PLANIFICACION \n");
		//TODO
	} else {
		printf("No se pudo reconocer el comando \n");
		return;
	}

}

void mostrarProcesosEnSistema() {

	char* msj = malloc(10);
	free(msj);
}

void mostrarTodosLosProcesos() {
	printNuevos();
	printListos();
	printEjecutados();
	printBloqueados();
	printFinalizados();
}

void printEjecutados() {
	int cantEjecutados = cantProcesosEjecutando();
	if (cantEjecutados == 0) {
		printf("No hay ningun proceso en ejecucion \n");
	} else {
		int i = 0;
		for (; i < cantEjecutados; i++) {
			t_pcb* pcb = list_get(colaEjecutando, i);

			printf("PID: %d, Estado: %s \n", pcb->PID, "Ejecutando");
			fflush(stdout);
		}
	}
}

void printNuevos() {
	int cantNuevos = cantProcesosNuevos();
	if (cantNuevos == 0) {
		printf("No hay ningun proceso en estado nuevo \n");
	} else {
		int i = 0;
		for (; i < cantNuevos; i++) {
			t_pcb* pcb = list_get(colaNuevos, i);

			printf("PID: %d, Estado: %s \n", pcb->PID, "Nuevo");
			fflush(stdout);
		}
	}
}

void printFinalizados() {
	int cantFinalizados = cantProcesosFinalizados();
	if (cantFinalizados == 0) {
		printf("No hay ningun proceso finalizado \n");
	} else {
		int i = 0;
		for (; i < cantFinalizados; i++) {
			t_pcb* pcb = list_get(colaFinalizados, i);

			printf("PID: %d, Estado: %s, Exit Code: %d \n", pcb->PID,
					"Finalizado", pcb->EC);
			fflush(stdout);
		}
	}
}

void printBloqueados() {
	int cantBloqueados = cantProcesosBloqueados();
	if (cantBloqueados == 0) {
		printf("No hay ningun proceso en estado bloqueado \n");
	} else {
		int i = 0;
		for (; i < cantBloqueados; i++) {
			t_pcb* pcb = list_get(colaBloqueados, i);
			printf("PID: %d, Estado: %s \n", pcb->PID, "Bloqueado");
			fflush(stdout);
		}
	}
}

void printListos() {
	int cantListos = cantProcesosListos();
	if (cantListos == 0) {
		printf("No hay ningun proceso en estado listo \n");

	} else {
		int i = 0;
		for (; i < cantListos; i++) {
			t_pcb* pcb = list_get(colaListos, i);
			printf("PID: %d, Estado: %s \n", pcb->PID, "LISTO");
			fflush(stdout);
		}
	}
}

int cantidadTotalDeProcesos() {
	int cantListos = cantProcesosListos();
	int cantFinalizados = cantProcesosFinalizados();
	int cantNuevos = cantProcesosNuevos();
	int cantBloqueados = cantProcesosBloqueados();
	int cantEjecutando = cantProcesosEjecutando();

	return cantListos + cantFinalizados + cantNuevos + cantBloqueados
			+ cantEjecutando;
}

int cantProcesosListos() {
	return list_size(colaListos);
}

int cantProcesosFinalizados() {
	return list_size(colaFinalizados);
}

int cantProcesosEjecutando() {
	return list_size(colaEjecutando);
}

int cantProcesosBloqueados() {
	return list_size(colaBloqueados);
}

int cantProcesosNuevos() {
	return list_size(colaNuevos);
}

void imprimirRafagasEjecutadasPorPid(int32_t pid) {
	int size = list_size(listaInfoProcesos);
	int i = 0;
	for (; i < size; i++) {
		entrada_info_proceso* entrada = list_get(listaInfoProcesos, i);
		if (entrada->pid == pid) {
			printf("PID: %d, Rafagas ejecutadas: %d \n", entrada->pid,
					entrada->rafagas_ejecutadas);
			return;
		}
	}
	printf("No hay ningun proceso con el pid %d \n", pid);
}

void imprimirSyscallsEjecutadasPorPid(int32_t pid) {
	int size = list_size(listaInfoProcesos);
	int i = 0;
	for (; i < size; i++) {
		entrada_info_proceso* entrada = list_get(listaInfoProcesos, i);
		if (entrada->pid == pid) {
			printf("PID: %d, Syscalls ejecutadas: %d \n", entrada->pid,
					entrada->syscall_ejecutadas);
			return;
		}
	}
	printf("No hay ningun proceso con el pid %d \n", pid);
}

void mostrarTablaArchivosDeProceso(int32_t pid) {
	t_pcb* pcb = buscarPcbPorPID(pid);
	if (pcb == NULL) {
		printf("El proceso %d no existe \n", pid);
		return;
	}
	log_info(infoLogger, "Mostrando tabla de archivos del proceso %d", pid);
	char* pidString = itoa(pid);
	t_list* pft = dictionary_get(dicPID_PFT, pidString);
	int cantidadArchivos = list_size(pft);
	int c = 3;
	for (; c < cantidadArchivos; c++) {
		entrada_pft* entrada = list_get(pft, c);
		printf("Local FD: %d, FLAGS: %s GLOBAL FD: %d CURSOR: %d \n ", c,
				entrada->flags, entrada->global_fd, entrada->cursor);
	}

}

void mostrarTablaArchivosGlobal() {
	int cantArchivos = list_size(globalFileTable);
	int c = 0;
	if (cantArchivos == 0) {
		printf("No hay archivos abiertos \n");
	} else {
		for (; c < cantArchivos; c++) {
			entrada_gft* entrada = list_get(globalFileTable, c);
			printf("Global FD: %d, Veces abierto: %d, Nombre: %s \n", c,
					entrada->abiertos, entrada->nombre);
		}
	}

}

void mostrarHeapProceso(int32_t pid) {
	int paginasHeap = obtenerCantidadPaginasHeap(pid);
	printf("---MOSTRANDO INFORMACION DEL HEAP DEL PROCESO %d \n", pid);
	printf("Paginas de Heap: %d \n", paginasHeap);

	int i = 0;
	for (; i < list_size(listaInfoProcesos); i++) {
		entrada_info_proceso* entrada = list_get(listaInfoProcesos, i);
		if (entrada->pid == pid) {
			printf("Cantidad de acciones alocar: %d, total allocs: %d \n",
					entrada->cantidadAllocs, entrada->totalAllocado);
			printf("Cantidad de acciones liberar: %d, total liberado: %d \n",
					entrada->cantidadLiberaciones, entrada->totalLiberado);
			break;
		}
	}

}

int obtenerCantidadPaginasHeap(int32_t pid) {
	int c = 0;
	int contadorEntradas = 0;
	for (; c < list_size(tablaHeap); c++) {
		nodo_tabla_heap* entrada = list_get(tablaHeap, c);
		if (entrada->PID == pid) {
			contadorEntradas++;
		}
	}
	log_info(infoLogger, "Se contaron %d paginas de heap para el proceso %d",
			contadorEntradas, pid);
	return contadorEntradas;
}

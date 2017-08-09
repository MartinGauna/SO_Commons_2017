#include "headers/planificacion.h"

//se encarga de crear un nuevo proceso listo en el sistema
//NO VERIFICA NIVEL DE MULTIPROG.
//agrega el proceso a la cola de listos
//si falla la creacion del proceso el mismo es rechazado y retorna FAILURE, sino SUCCESS

void crearNuevoProcesoListo(t_pcb** pcb) {
	*pcb = crearPcb();
	pasarPcbDeNuevoAListos(pcb);
	log_info(infoLogger, "Creado nuevo proceso en estado listo, PID: %d",
			(*pcb)->PID);
}

/*
 * False si no se permiten mas procesos en el sistema, true caso contrario
 */
bool checkNivelMultiprogramacion() {
	return (cantidadDeProcesosEnSistema < kernel_config->gradoMultiprog);

}

void pasarProximoProcDeNewAReady() {
	if (!planificacionActivada)
		return;
	if (list_size(colaNuevos) == 0)
		return;
	t_pcb* pcb = list_remove(colaNuevos, 0);
	log_info(infoLogger,
			"Se quito al proceso con pid %d de la cola de NEW para pasarlo a ready",
			pcb->PID);
	//necesito obtener el codigo del programa, y asignarle las paginas correspondientes
	PROTOCOLO_KERNEL_A_CONSOLA msj = PROCESO_PASO_DE_NEW_A_READY;
	entrada_programaSocket_pid* programa = buscarHiloProgramaByPID(pcb->PID);

	librocket_enviarMensaje(programa->programaSocket, &msj,
			sizeof(PROTOCOLO_KERNEL_A_CONSOLA), infoLogger);
	int32_t codigoSize;
	librocket_recibirMensaje(programa->programaSocket, &codigoSize,
			sizeof(int32_t), infoLogger);
	char* codigo = malloc(codigoSize + 1);
	librocket_recibirMensaje(programa->programaSocket, codigo, codigoSize,
			infoLogger);

	t_metadata_program* metadata = metadata_desde_literal(codigo);

	pcb->instruccion_inicio = metadata->instruccion_inicio;
	pcb->instrucciones_size = metadata->instrucciones_size;
	pcb->PC = pcb->instruccion_inicio;

	int32_t contador_instrucciones;
	for (contador_instrucciones = 0;
			contador_instrucciones < pcb->instrucciones_size;
			contador_instrucciones++) {

		t_intructions instruccion =
				metadata->instrucciones_serializado[contador_instrucciones];

		t_intructions* instruccionParaAgregar = malloc(sizeof(t_intructions));

		instruccionParaAgregar->offset = instruccion.offset;
		instruccionParaAgregar->start = instruccion.start;

		list_add(pcb->indice_codigo, instruccionParaAgregar);

	}

	pcb->etiquetas_size = metadata->etiquetas_size;

	char* bufferEtiquetas = malloc(metadata->etiquetas_size);
	memcpy(bufferEtiquetas, metadata->etiquetas, metadata->etiquetas_size);
	pcb->indice_etiquetas = bufferEtiquetas;
	pcb->cantidad_de_funciones = metadata->cantidad_de_funciones;
	pcb->cantidad_de_etiquetas = metadata->cantidad_de_etiquetas;

	metadata_destruir(metadata);

	int32_t resultadoAsignacionPaginasIniciales =
			pedirAMemoriaInicializacionDeProceso(&pcb, codigo);
	if (resultadoAsignacionPaginasIniciales
			== -2|| resultadoAsignacionPaginasIniciales == FAILURE) {
		//no hay memoria o no se creo por algun error
		pasarPcbDeNuevoAFinalizados(&pcb, NO_SE_PUDO_ASIGNAR_RECURSO);
		eliminarPidProgramaSocket(programa->programaSocket);

		PROTOCOLO_KERNEL_A_CONSOLA respuesta = PROCESO_RECHAZADO;
		librocket_enviarMensaje(programa->programaSocket, &respuesta,
				sizeof(PROTOCOLO_KERNEL_A_CONSOLA), infoLogger);
		librocket_enviarMensaje(programa->programaSocket, &(pcb->EC),
				sizeof(int32_t), infoLogger);
		free(codigo);
	} else {
		//el proceso ya esta con sus pagInas en memoria
		//INFORMO A CONSOLA QUE RECIBIO PAGINAS
		PROTOCOLO_KERNEL_A_CONSOLA respuesta = PROCESO_RECIBIO_PAGINAS_INICIALES;
		librocket_enviarMensaje(programa->programaSocket, &respuesta,
				sizeof(PROTOCOLO_KERNEL_A_CONSOLA), infoLogger);
		//MANDO EL CODIGO A LA MEMORIA
		enviarCodigoDePrograma(pcb->PID, codigo);
		pasarPcbDeNuevoAListos(&pcb);
		if (list_size(cpusLibres) > 0) {
			planificar(NULL);
		}
		free(codigo);
	}

}

entrada_programaSocket_pid* buscarHiloProgramaByPID(int32_t pid) {
	int size = list_size(listaProgramaSocketPid);
	int i = 0;
	for (; i < size; i++) {
		entrada_programaSocket_pid* entrada = list_get(listaProgramaSocketPid,
				i);
		if (entrada->pid == pid) {
			return entrada;
		}
	}
	log_error(infoLogger,
			"NO SE ENCONTRO NINGUN HILO PROGRAMA CON EL PID PEDIDO");
	return NULL;

}

void borrarPrograma(entrada_programaSocket_pid* programa) {
	int c = 0;
	for (; c < list_size(listaProgramaSocketPid); c++) {
		entrada_programaSocket_pid* entrada = list_get(listaProgramaSocketPid,
				c);
		if (entrada->programaSocket == programa->programaSocket) {
			list_remove(listaProgramaSocketPid, c);
			log_info(infoLogger, "Eliminado el hilo consola socket %d",
					programa->programaSocket);
			return;
		}
	}
	log_error(infoLogger,
			"Borrar hilo programa: no se encontro el hilo programa socket %d",
			programa->programaSocket);
}

/*
 * llamado cada que vez que se recibe un proceso nuevo y cuando una cpu ejecuto una sentencia
 * se recibe el cpuSocket cuando una cpu termino de ejecutar una sentencia
 */
void planificar(int cpuSocket) {
	if (!planificacionActivada) {
		log_info(infoLogger,
				"Se entro en planificar pero la planificacion esta detenida");
		return;
	}
	log_info(infoLogger, "-------PLANIFICANDO-----");
	if (recargarConfig) {
		recargarConfig = false;
		recargarQuantumSpeed();
	}

	CPU* cpu;
	int cantidadCpusLibres = list_size(cpusLibres);
	int cantidadCpusOcupadas = list_size(cpusOcupadas);
	int indexDeCpusOcupadas = 0;

	if (cpuSocket != 0) {
		usleep(kernel_config->quantumSpeed);
		//CPU acaba de terminar de ejecutar una sentencia
		//chequeo que se no haya matado el proceso por consola
		if (checkTengoQueMatarPIDEnEjecucion(cpuSocket)) {
			PROTOCOLO_KERNEL_A_CPU abortarProceso = CAMBIAR_PROCESO;
			librocket_enviarMensaje(cpuSocket, &abortarProceso, sizeof(int32_t),
					infoLogger);
			t_pcb* pcbActualizado = recibirPcb(cpuSocket, infoLogger);
			reemplazarPcbEnEjecutando(pcbActualizado);
			pasarPcbDeEjecutandoAFinalizados(&pcbActualizado);
			pedirAMemoriaFinalizarProceso(pcbActualizado->PID);
			informarAconsolaTerminacionDeProceso(pcbActualizado->PID,
					pcbActualizado->EC);
			informar_paginas_heap(pcbActualizado->PID);
			pasarProximoProcDeNewAReady();
			cpu = buscarCPUBySocket(cpuSocket);
			eliminarCpuDeOcupadas(cpu);
			list_add(cpusLibres, cpu);
			ejecutarSiguientePcbEnCpu(cpu);
			return;
		}

		//analizo si sigue ejecutando el proceso o no en la cpu
		if (string_equals_ignore_case(kernel_config->algoritmo, "FIFO")) {
			//*********FIFO******
			PROTOCOLO_KERNEL_A_CPU seguirEjecutando = SEGUI_EJECUTANDO;
			librocket_enviarMensaje(cpuSocket, &seguirEjecutando,
					sizeof(PROTOCOLO_KERNEL_A_CPU), infoLogger);
			cpu = buscarCPUBySocket(cpuSocket);
			agregarRafagaEjecutadaAPcb(cpu->PID_en_ejecucion);
		} else if (string_equals_ignore_case(kernel_config->algoritmo, "RR")) {
			//******RR*****
			//busco la cpu correspondiente que acaba de terminar quantum
			int i = 0;
			for (; i < cantidadCpusOcupadas; i++) {
				cpu = list_get(cpusOcupadas, i);
				if (cpu->socket == cpuSocket) {
					indexDeCpusOcupadas = i;
					break;
				}
			}
			if (cpu->quantumAcumulado >= kernel_config->quantum) {
				log_info(infoLogger,
						"Quantum terminado, cambiando proceso de la cpu socket %d y pid %d ",
						cpu->socket, cpu->PID_en_ejecucion);
				//se paso del quantum, cambiar el proceso
				cpu->quantumAcumulado = 0;
				PROTOCOLO_KERNEL_A_CPU cambiarProceso = CAMBIAR_PROCESO;
				librocket_enviarMensaje(cpuSocket, &cambiarProceso,
						sizeof(PROTOCOLO_KERNEL_A_CPU), infoLogger);
				//ahora espero recibir el pcb del proceso que esta ejecutando la CPU
				t_pcb* pcb = recibirPcb(cpuSocket, infoLogger);
				reemplazarPcbEnEjecutando(pcb);
				pasarPcbDeEjecutandoAListos(&pcb);
				//envio el proximo pcb a ejecutar
				cpu->quantumAcumulado++;
				ejecutarSiguientePcbEnCpu(cpu);
			} else {
				log_info(infoLogger,
						"Le digo a la CPU socket %d que siga ejecutando proceso %d, QUANTUM acumulado antes de incrementarlo: %d",
						cpu->socket, cpu->PID_en_ejecucion,
						cpu->quantumAcumulado);
				PROTOCOLO_KERNEL_A_CPU seguirEjecutando = SEGUI_EJECUTANDO;
				librocket_enviarMensaje(cpuSocket, &seguirEjecutando,
						sizeof(PROTOCOLO_KERNEL_A_CPU), infoLogger);
				cpu->quantumAcumulado++;
				agregarRafagaEjecutadaAPcb(cpu->PID_en_ejecucion);
			}
		}

	} else {
		if (cantidadCpusLibres <= 0) {
			log_error(infoLogger,
					"----NO HAY CPUS LIBRES PARA PLANIFICAR------------");
			//printf("----NO HAY CPUS LIBRES PARA PLANIFICAR------------\n");
			return;
		}
		cpu = list_get(cpusLibres, 0);
		moverCpuAOcupadas(indexDeCpusOcupadas);
		if (string_equals_ignore_case(kernel_config->algoritmo, "RR")) {
			cpu->quantumAcumulado++;
		}
		ejecutarSiguientePcbEnCpu(cpu);
	}
}

bool checkTengoQueMatarPIDEnEjecucion(int cpuSocket) {
	CPU * cpu = buscarCPUBySocket(cpuSocket);
	if (estaElflagDeTerminacionDeProcesoActivo(cpu->PID_en_ejecucion)) {
		return true;
	}
	return false;
}

//ELIMINA EL PID DE LA LISTA SI SE ENCONTRO
bool estaElflagDeTerminacionDeProcesoActivo(int32_t pid) {
	int i = 0;
	for (; i < list_size(procesosADestruir); i++) {
		entrada_destruir_proceso* entrada = list_get(procesosADestruir, i);
		if (entrada->pid == pid) {
			log_info(infoLogger,
					"Se detecto que el proceso %d tiene el flag de kill activado",
					pid);
			list_remove(procesosADestruir, i);
			return true;
		}
	}
	return false;
}

void ejecutarSiguientePcbEnCpu(CPU* cpu) {
	t_pcb* pcb = obtenerProxProcesoAEjecutar(); // pcb de proximo proceso a ejecutar
	if (pcb == NULL)
		return;
	pasarPcbDeListosAEjecutando(&pcb);

	agregarRafagaEjecutadaAPcb(pcb->PID);
	if (string_equals_ignore_case(kernel_config->algoritmo, "RR")) {
		log_info(infoLogger,
				"Enviando a ejecutar el proceso con pid %i en la cpu con socket %i, el quantum acumulado de la cpu es %i",
				pcb->PID, cpu->socket, cpu->quantumAcumulado);
	} else
		log_info(infoLogger,
				"Enviando a ejecutar el proceso con pid %i en la cpu con socket %i",
				pcb->PID, cpu->socket);

	//envio el PCB a la CPU
	PROTOCOLO_KERNEL_A_CPU ejecutarPrograma = EJECUTAR_PROGRAMA;
	librocket_enviarMensaje(cpu->socket, &ejecutarPrograma,
			sizeof(PROTOCOLO_KERNEL_A_CPU), infoLogger);

	cpu->PID_en_ejecucion = pcb->PID;
	//crearListasDelPcb(&pcb); //TODO REMOVER

	enviarPcb(cpu->socket, pcb, infoLogger);

}

t_pcb* obtenerProxProcesoAEjecutar() {
	return list_get(colaListos, 0);
}

void agregarSyscallAPcb(int32_t pid) {
	int i = 0;
	for (; i < list_size(listaInfoProcesos); i++) {
		entrada_info_proceso* entrada = list_get(listaInfoProcesos, i);
		if (entrada->pid == pid) {
			entrada->syscall_ejecutadas++;
			return;
		}
	}
}

void agregarRafagaEjecutadaAPcb(int32_t pid) {
	int tamanioTabla = list_size(listaInfoProcesos);
	int i = 0;
	for (; i < tamanioTabla; i++) {
		entrada_info_proceso* entrada = list_get(listaInfoProcesos, i);
		if (entrada->pid == pid) {
			entrada->rafagas_ejecutadas++;
			log_info(infoLogger,
					"EL pcb con pid %d aumento su cantidad de rafagas ejecutadas a %d",
					pid, entrada->rafagas_ejecutadas);
			return;
		}
	}
	log_error(infoLogger,
			"Se quiso agregar una rafaga para el proceso con pid %d pero no se encontro en listaInfoProcesos",
			pid);
}

void moverCpuAOcupadas(int indexEnLibres) {
	CPU* cpu = list_get(cpusLibres, indexEnLibres);
	list_remove(cpusLibres, indexEnLibres);
	list_add(cpusOcupadas, cpu);
}

void eliminarCpuDeOcupadas(CPU* cpu) {
	int c = 0;
	for (; c < list_size(cpusOcupadas); c++) {
		CPU* otraCpu = list_get(cpusOcupadas, c);
		if (otraCpu->socket == cpu->socket) {
			log_info(infoLogger, "Removida la cpu con socket %d de ocupadas",
					cpu->socket);
			list_remove(cpusOcupadas, c);
			return;
		}
	}
	log_error(infoLogger,
			"Se quiso remover una cpu con socket %d de la lista de ocupadas pero no se pudo encontrar",
			cpu->socket);
}

//llamado desde la consola del kernel
void matarProceso(int32_t pid) {
	int cpuSocket = estaElProcesoEnEjecucion(pid);
	if (cpuSocket == 0) {
		//el proceso no se esta ejecutando
		t_pcb* pcb = buscarPcbPorPID(pid);
		if (pcb == NULL) {
			printf("El proceso con pid %d no se pudo encontrar \n", pid);
			return;
		}
		log_info(infoLogger, "Matando proceso con pid %d", pid);
		switch (pcb->estado) {
		case NUEVO:
			pasarPcbDeNuevoAFinalizados(&pcb, FINALIZADO_POR_COMANDO);
			break;

		case LISTO:
			pcb->EC = FINALIZADO_POR_COMANDO;
			pasarPcbDeListoAFinalizados(&pcb);
			pedirAMemoriaFinalizarProceso(pcb->PID);
			informarAconsolaTerminacionDeProceso(pcb->PID, pcb->EC);
			break;

		case BLOQUEADO:
			pasarPcbDeBloqueadoAfinalizados(&pcb, FINALIZADO_POR_COMANDO);
			pedirAMemoriaFinalizarProceso(pcb->PID);
			informarAconsolaTerminacionDeProceso(pcb->PID, pcb->EC);
			break;
		}
		informar_paginas_heap(pid);
	} else {
		//el proceso esta ejecutandose, cuando una cpu termina quantum lo mato, activo un flag
		entrada_destruir_proceso* entrada = malloc(
				sizeof(entrada_destruir_proceso));
		entrada->pid = pid;
		list_add(procesosADestruir, &pid);
	}

}

//cpu envio el mesaje ABORTAR
void pedidoAbortarProgramaDeCpu(int32_t cpuSocket) {
	CPU* cpu = buscarCPUBySocket(cpuSocket);
	if (cpu == NULL) {
		log_error(infoLogger,
				"FALLO ABORTANDO PROGRAMA, NO SE ENCONTRO CPU CON SOCKET %d",
				cpuSocket);
		return;
	}

	log_info(infoLogger, "La cpu socket %d pidio abortar el proceso %d",
			cpu->socket, cpu->PID_en_ejecucion);
	t_pcb* pcb = recibirPcb(cpuSocket, infoLogger);
	reemplazarPcbEnEjecutando(pcb);
	pasarPcbDeEjecutandoAFinalizados(&pcb);

	cpu->PID_en_ejecucion = 0;
	cpu->quantumAcumulado = 0;
	eliminarCpuDeOcupadas(cpu);
	list_add(cpusLibres, cpu);

	//le pido a la memoria que me libere las paginas del procesos
	pedirAMemoriaFinalizarProceso(pcb->PID);

	informar_paginas_heap(pcb->PID);

	//aviso a la consola que se finalizo el proceso
	informarAconsolaTerminacionDeProceso(pcb->PID, pcb->EC);
	pasarProximoProcDeNewAReady();

}

//la consola pidio que se termine el proceso o se desconecto
void pedidoAbortarProgramaDeConsola(int32_t socket, bool porDesconexion) {
	int32_t pid = buscarPidByProgramaSocket(socket);



	if (pid == -1) {
		log_error(infoLogger,
				"No se encontro ningun proceso corriendo en LA CONSOLA SOCKET %d",
				socket);
		return;
	}

	log_info(infoLogger, "La consola socket %d pidio cerrar su proceso pid %d",
			socket, pid);
	//lo saco de la lista de programas
	eliminarPidProgramaSocket(socket);
	t_pcb* pcb = buscarPcbPorPID(pid);
	if (pcb == NULL) {
		log_error(infoLogger,
				"Consola socket %d quiere cerrar un proceso o se desconecto pero el proceso %d no existe o fue finalizado",
				socket, pid);
		return;
	}
	if (porDesconexion) {
		pcb->EC = DESCONEXION_CONSOLA;
	} else {
		pcb->EC = FINALIZADO_POR_COMANDO;
	}
	if (pcb->estado == EJECUTANDO) {
		entrada_destruir_proceso* entrada = malloc(
				sizeof(entrada_destruir_proceso));
		entrada->pid = pid;
		list_add(procesosADestruir, entrada);
	} else {
		pasarPcbAFinalizados(&pcb);
		pedirAMemoriaFinalizarProceso(pid);
		informar_paginas_heap(pid);
	}

}

void informarAconsolaTerminacionDeProceso(int32_t pid, EXIT_CODE_TYPE exitCode) {
	entrada_programaSocket_pid* programa = buscarHiloProgramaByPID(pid);
	if (programa == NULL) {
		return;
	}
	PROTOCOLO_KERNEL_A_CONSOLA finalizarProceso = PROCESO_TERMINADO;
	librocket_enviarMensaje(programa->programaSocket, &finalizarProceso,
			sizeof(int32_t), infoLogger);
	librocket_enviarMensaje(programa->programaSocket, &(exitCode),
			sizeof(int32_t), infoLogger);
	FD_CLR(programa->programaSocket, &socketsConsolaSet);
	borrarPrograma(programa);
}

void checkSiDejoArchivosAbiertos(int32_t pid) {
	char* pidString = itoa(pid);
	t_list* pft = dictionary_get(dicPID_PFT, pidString);
	int c = 3;
	for(;c<list_size(pft);c++){
		entrada_pft* entrada = list_get(pft,c);
		entrada_gft* entradaGlobal = list_get(globalFileTable,entrada->global_fd);
		if(entradaGlobal->abiertos > 1){
			entradaGlobal->abiertos--;
		}else {
			list_remove(globalFileTable,entrada->global_fd);
		}
	}

}



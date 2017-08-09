#include "headers/conexionesKernelCpu.h"

void* nuevoCpuConectado(int socket) {

	printf("Se conecto una CPU con socket: %d.\n", socket);
	log_info(infoLogger, "Se conecto una CPU con socket: %d.\n", socket);

	agregarCpuALaLista(socket);

	return NULL;

}

void mensajeRecibidoDeCpu(int socket) {

	PROTOCOLO_CPU_A_KERNEL mensaje;
	PROTOCOLO_KERNEL_A_CPU respuesta;

	int result = librocket_recibirMensaje(socket, &mensaje,
			sizeof(PROTOCOLO_CPU_A_KERNEL), infoLogger);

	if (result == 0) {
		/*se cayo una cpu, removerla, limpiarla del set sockets y ademas terminar el proceso
		 * y aumentar el nivel de multiprogramacion
		 */
		printf("Se desconecto una CPU con el socket: %d.\n", socket);
		log_info(infoLogger, "Se desconecto una CPU con el socket: %d.\n",
				socket);

		terminarProcesoEjecutandoEnCpu(socket);
		FD_CLR(socket, &socketsCpuSet);
		removerCpuDeLaLista(socket);

		return;

	}

	if (mensaje == HANDSHAKE_CONECTAR_CPU_A_KERNEL) {

		char* texto = string_new();
		string_append(&texto, "kernel manda sus saludos");
		int32_t tamanioTexto = string_length(texto);
		tamanioTexto++;

		respuesta = HANDSHAKE_CONEXION_DE_CPU_A_KERNEL_ACEPTADA;
		librocket_enviarMensaje(socket, &respuesta,
				sizeof(PROTOCOLO_KERNEL_A_CPU), infoLogger);

		respuesta = ENVIO_TEXTO_A_CPU;
		librocket_enviarMensaje(socket, &respuesta,
				sizeof(PROTOCOLO_KERNEL_A_CPU), infoLogger);
		librocket_enviarMensaje(socket, &tamanioTexto, sizeof(int32_t),
				infoLogger);
		librocket_enviarMensaje(socket, texto, tamanioTexto, infoLogger);

		respuesta = ENVIO_TAMANIO_PAGINA;
		librocket_enviarMensaje(socket, &respuesta,
				sizeof(PROTOCOLO_KERNEL_A_CPU), infoLogger);
		librocket_enviarMensaje(socket, &kernel_config->pageSize,
				sizeof(int32_t), infoLogger);

		free(texto);
		if (list_size(colaListos) > 0) {
			planificar(NULL);
		}

	} else if (mensaje == OBTENER_VALOR_COMPARTIDA) {

		obtener_valor_compartida(socket, mensaje);

	} else if (mensaje == ASIGNAR_VALOR_COMPARTIDA) {

		asignar_valor_compartida(socket, mensaje);

	} else if (mensaje == PEDIDO_BLOQUE_MEMORIA) {

		pedido_bloque_memoria(socket);

	} else if (mensaje == LIBERAR_BLOQUE) {

		liberar_bloque(socket);

	} else if (mensaje == WAIT) {

		pedido_wait(socket);

	} else if (mensaje == SIGNAL) {

		pedido_signal(socket);

	} else if (mensaje == TERMINE_SENTENCIA) {

		planificar(socket);

	} else if (mensaje == PROGRAMA_TERMINADO) {
		t_pcb* pcb = recibirPcb(socket, infoLogger);
		reemplazarPcbEnEjecutando(pcb);
		informar_paginas_heap(pcb->PID);
		log_info(infoLogger,
				"La cpu con socket %d informo que termino la ejecucion del proceso con PID %d",
				socket, pcb->PID);

		pasarPcbDeEjecutandoAFinalizados(&pcb);
		CPU* cpu = buscarCPUBySocket(socket);
		cpu->PID_en_ejecucion = 0;
		cpu->quantumAcumulado = 0;
		list_add(cpusLibres, cpu);
		eliminarCpuDeOcupadas(cpu);
		//libero los recursos que usa el proceso en la memoria
		PROTOCOLO_KERNEL_Y_CPU_A_MEMORIA msjTerminar = FINALIZAR_PROGRAMA;
		librocket_enviarMensaje(memoriaSocket, &msjTerminar, sizeof(int32_t),
				infoLogger);
		librocket_enviarMensaje(memoriaSocket, &(pcb->PID), sizeof(int32_t),
				infoLogger);
		PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU respuestaMemoria;
		librocket_recibirMensaje(memoriaSocket, &respuestaMemoria,
				sizeof(int32_t), infoLogger);
		//informo a la consola que se termino el programa y exit code
		entrada_programaSocket_pid* programa = buscarHiloProgramaByPID(
				pcb->PID);
		if(programa!= NULL){
			PROTOCOLO_KERNEL_A_CONSOLA msjProgramaTerminado = PROCESO_TERMINADO;
					librocket_enviarMensaje(programa->programaSocket, &msjProgramaTerminado,
							sizeof(int32_t), infoLogger);
					librocket_enviarMensaje(programa->programaSocket, &pcb->EC,
							sizeof(EXIT_CODE_TYPE), infoLogger);
					borrarPrograma(programa);
		}

		if (list_size(colaNuevos) > 0) {
			pasarProximoProcDeNewAReady();
		}
		if (list_size(colaListos) > 0) {
			planificar(NULL);
		}

	} else if (mensaje == PRIMITIVA_ABRIR_ARCHIVO) {

		pedido_abrir_archivo(socket);

	} else if (mensaje == PRIMITIVA_BORRAR_ARCHIVO) {

		pedido_borrar_archivo(socket);

	} else if (mensaje == PRIMITIVA_CERRAR_ARCHIVO) {

		pedido_cerrar_archivo(socket);

	} else if (mensaje == PRIMITIVA_MOVER_CURSOR_ARCHIVO) {

		pedido_mover_cursor_archivo(socket);
		//Ver que pasa con el proceso cuando es ARCHIVO_INVALIDO

	} else if (mensaje == PRIMITIVA_ESCRIBIR_ARCHIVO) {

		pedido_escribir_archivo(socket);

	} else if (mensaje == PRIMITIVA_LEER_ARCHIVO) {

		pedido_leer_archivo(socket);

	} else if (mensaje == ABORTAR_PROGRAMA) {

		pedidoAbortarProgramaDeCpu(socket);

	} else if (mensaje == ME_DESCONECTO) {

		pedidoAbortarProgramaDeCpu(socket);

	} else {

		log_info(infoLogger,
				"una cpu envio un mensaje pero el kernel no puede entenderlo\n");

	}

}



void terminarProcesoEjecutandoEnCpu(int cpuSocket) {

	CPU* cpu = buscarCPUBySocket(cpuSocket);

	if (cpu == NULL) {
		log_info(infoLogger,
				"Se desconecto una cpu soket %d, pero no se encontro en ninguna lista",
				cpuSocket);
		return;
	}

	t_pcb* pcb = buscarPcbPorPID(cpu->PID_en_ejecucion);

	if (pcb == NULL) {
		log_info(infoLogger,
				"Se desconecto la cpu socket %d pero no estaba ejecutando ningun proceso",
				cpuSocket);
		return;
	}

	if(pcb->estado != EJECUTANDO){
		return;
	}

	log_info(infoLogger,
			"Cpu socket %d ejecutando proceso %d se desconecto, terminando proceso...",
			cpuSocket, pcb->PID);
	pcb->EC = DESCONEXION_CPU;
	int socketHiloPrograma = buscarProgramaSocketByPid(pcb->PID);

	pedirAMemoriaFinalizarProceso(pcb->PID);

	//le aviso a la consola que se termino el proceso y le mando el exit code
	PROTOCOLO_KERNEL_A_CONSOLA msj = PROCESO_TERMINADO;
	librocket_enviarMensaje(socketHiloPrograma, &msj,
			sizeof(PROTOCOLO_KERNEL_A_CONSOLA), infoLogger);
	librocket_enviarMensaje(socketHiloPrograma, &(pcb->EC),
			sizeof(EXIT_CODE_TYPE), infoLogger);
	eliminarPidProgramaSocket(socketHiloPrograma);
	FD_CLR(socketHiloPrograma, &socketsConsolaSet);
	pasarPcbDeEjecutandoAFinalizados(&pcb);
	pasarProximoProcDeNewAReady();

}

CPU* buscarCPUBySocket(int32_t socket) {
	log_info(infoLogger, "Buscando la CPU correspondiente al socket %d",
			socket);
	int size = list_size(cpusOcupadas);
	int c = 0;
	for (; c < size; c++) {
		CPU* cpu = list_get(cpusOcupadas, c);
		if (cpu->socket == socket) {
			log_info(infoLogger, "cpu encontrada en ocupadas");
			return cpu;
		}
	}

	c = 0;
	for (; c < list_size(cpusLibres); c++) {
		CPU* cpu = list_get(cpusLibres, c);
		if (cpu->socket == socket) {
			log_info(infoLogger, "cpu encontrada en libres");
			return cpu;
		}
	}
	log_error(infoLogger, "No se encontro la cpu con socket %d", socket);
	return NULL;
}

//agrega una nueva cpu a la lista de CPUS con el socket pasado por parametro
void agregarCpuALaLista(int32_t socket) {
	CPU* cpu = malloc(sizeof(CPU));
	cpu->quantumAcumulado = 0;
	cpu->socket = socket;
	list_add(cpusLibres, cpu);
	log_info(infoLogger, "Agregada nueva cpu con socket %d a la lista de CPUs",
			socket);
}

//busca el cpu con el socket correspondiente y lo remueve
void removerCpuDeLaLista(int32_t socket) {
	int* s;
	int i = 0;
	int cantidadCpus = list_size(cpusLibres);
	for (; i < cantidadCpus; i++) {
		s = list_get(cpusLibres, i);
		if (*s == socket) {
			CPU* cpu = list_remove(cpusLibres, i);
			log_info(infoLogger,
					"Removida una cpu de la lista de CPUs LIBRES, cpu socket: %d, pid en ejecucion: %d,"
							"quantum acumulado %d ", cpu->socket,
					cpu->PID_en_ejecucion, cpu->quantumAcumulado);
			free(cpu);
			return;
		}
	}

	cantidadCpus = list_size(cpusOcupadas);
	i = 0;
	for (; i < cantidadCpus; i++) {
		s = list_get(cpusOcupadas, i);
		if (*s == socket) {
			CPU* cpu = list_remove(cpusOcupadas, i);
			log_info(infoLogger,
					"Removida una cpu de la lista de CPUs OCUPADAS, cpu socket: %d, pid en ejecucion: %d,"
							"quantum acumulado %d ", cpu->socket,
					cpu->PID_en_ejecucion, cpu->quantumAcumulado);
			free(cpu);
			return;
		}
	}

	log_error(infoLogger,
			"No se pudo quitar el CPU con socket %d porque no se encontro",
			socket);

}

void obtener_valor_compartida(int32_t socketCPU,
		PROTOCOLO_CPU_A_KERNEL condicion) {

	PROTOCOLO_KERNEL_A_CPU respuesta;
	t_valor_variable valor;

	if (existeCompartida(socketCPU, &valor, condicion)) {
		respuesta = ENVIADO_VALOR_COMPARTIDA;
		librocket_enviarMensaje(socketCPU, &respuesta,
				sizeof(PROTOCOLO_KERNEL_A_CPU), infoLogger);
		librocket_enviarMensaje(socketCPU, &valor, sizeof(t_valor_variable),
				infoLogger);
	} else {
		respuesta = COMPARTIDA_INEXISTENTE;
		librocket_enviarMensaje(socketCPU, &respuesta,
				sizeof(PROTOCOLO_KERNEL_A_CPU), infoLogger);
	}

}

void asignar_valor_compartida(int32_t socketCPU,
		PROTOCOLO_CPU_A_KERNEL condicion) {

	PROTOCOLO_KERNEL_A_CPU respuesta;
	t_valor_variable valor;

	if (existeCompartida(socketCPU, &valor, condicion)) {
		respuesta = VALOR_COMPARTIDA_ASIGNADO;
		librocket_enviarMensaje(socketCPU, &respuesta,
				sizeof(PROTOCOLO_KERNEL_A_CPU), infoLogger);
		librocket_enviarMensaje(socketCPU, &valor, sizeof(t_valor_variable),
				infoLogger);
	} else {
		respuesta = COMPARTIDA_INEXISTENTE;
		librocket_enviarMensaje(socketCPU, &respuesta,
				sizeof(PROTOCOLO_KERNEL_A_CPU), infoLogger);
	}

}

void pedido_wait(int32_t socketCPU) {

	PROTOCOLO_KERNEL_A_CPU respuesta;
	t_semaforo* semaforo = existeSemaforo(socketCPU);
	if (semaforo == NULL) {
		respuesta = SEMAFORO_INEXISTENTE;
		librocket_enviarMensaje(socketCPU, &respuesta,
				sizeof(PROTOCOLO_KERNEL_A_CPU), infoLogger);
	} else {
		log_info(infoLogger,
				"La cpu socket %d hizo wait sobre el semaforo %s cuyo valor es %d",
				socketCPU, *semaforo->nombreSemaforo, semaforo->valor);
		respuesta = SEMAFORO_OPERADO_CORRECTAMENTE;
		librocket_enviarMensaje(socketCPU, &respuesta,
				sizeof(PROTOCOLO_KERNEL_A_CPU), infoLogger);

		semaforo->valor--;
		//printf("Valor del semaforo %s: %i.\n", *semaforo->nombreSemaforo, semaforo->valor);

		if (semaforo->valor < 0) {
			PROTOCOLO_KERNEL_A_CPU cambiar = CAMBIAR_PROCESO;
			librocket_enviarMensaje(socketCPU, &cambiar, sizeof(int32_t),
					infoLogger);
			t_pcb* pcb = recibirPcb(socketCPU, infoLogger);
			 if(estaElflagDeTerminacionDeProcesoActivo(pcb->PID)){
				//al proceso hay que matarlo, no pasarlo a bloqueados
				 			reemplazarPcbEnEjecutando(pcb);
				 			pasarPcbAFinalizados(&pcb);
				 			pedirAMemoriaFinalizarProceso(pcb->PID);
				 			informarAconsolaTerminacionDeProceso(pcb->PID,
				 					pcb->EC);
				 			informar_paginas_heap(pcb->PID);
				 			pasarProximoProcDeNewAReady();
				 			CPU* cpu = buscarCPUBySocket(socketCPU);
				 			eliminarCpuDeOcupadas(cpu);
				 			list_add(cpusLibres, cpu);
				 			semaforo->valor++;
			 }else {
				 ////queda BLOQUEADO
					if (pcb == NULL) {
								log_error(infoLogger,
										"Error al recibir el pcb de la cpu al bloquearse !");
							} else {
								log_info(infoLogger, "El proceso %d quedo bloqueado", pcb->PID);
								//string_append(&pcb->nombre_semaforo_que_lo_bloqueo,*semaforo->nombreSemaforo);
								entrada_info_proceso* infoProceso = obtenerInfoProceso(pcb);

								free(infoProceso->sem_bloqueado_por);
								infoProceso->sem_bloqueado_por = string_new();

								if(infoProceso != NULL){
									string_append(&(infoProceso->sem_bloqueado_por),*semaforo->nombreSemaforo);
								}
								CPU* cpu = buscarCPUBySocket(socketCPU);
								eliminarCpuDeOcupadas(cpu);
								list_add(cpusLibres,cpu);
								reemplazarPcbEnEjecutando(pcb);
								pasarPcbDeEjecutandoABloqueados(&pcb);
							}
			 }
			if (list_size(colaListos) > 0) {
				planificar(NULL);
			}

		} else {
			//no se bloquea
			PROTOCOLO_KERNEL_A_CPU segui = SEGUI_EJECUTANDO;
			librocket_enviarMensaje(socketCPU, &segui, sizeof(int32_t),
					infoLogger);
		}

	}

}

void pedido_signal(int32_t socketCPU) {
	PROTOCOLO_KERNEL_A_CPU respuesta;
	t_semaforo* semaforo = existeSemaforo(socketCPU);
	if (semaforo == NULL) {
		respuesta = SEMAFORO_INEXISTENTE;
		librocket_enviarMensaje(socketCPU, &respuesta,
				sizeof(PROTOCOLO_KERNEL_A_CPU), infoLogger);
	}else {
		log_info(infoLogger,
					"La cpu socket %d hizo signal sobre el semaforo %s cuyo valor es %d",
					socketCPU, *semaforo->nombreSemaforo, semaforo->valor);
			respuesta = SEMAFORO_OPERADO_CORRECTAMENTE;
			librocket_enviarMensaje(socketCPU, &respuesta,
					sizeof(PROTOCOLO_KERNEL_A_CPU), infoLogger);

			semaforo->valor++;
			//printf("Valor del semaforo %s: %i.\n", *semaforo->nombreSemaforo, semaforo->valor);

			//veo si alguno de los procesos bloqueados esta esperando este semaforo
			int c = 0;
			for(;c<list_size(colaBloqueados);c++){
				t_pcb* pcb = list_get(colaBloqueados,c);
				entrada_info_proceso* infoProceso = obtenerInfoProceso(pcb);
				if(string_equals_ignore_case(infoProceso->sem_bloqueado_por,*semaforo->nombreSemaforo)){
					pasarPcbDeBloqueadoAListo(&pcb);
					free(infoProceso->sem_bloqueado_por);
					infoProceso->sem_bloqueado_por = string_new();
					if(list_size(cpusLibres) > 0){
						planificar(NULL);
					}
					break;// solo un proceso puede desbloquearse
				}
			}


	}

}

bool existeCompartida(int32_t socketCPU, t_valor_variable* valor,
		PROTOCOLO_CPU_A_KERNEL condicion) {

	t_variable_global* variableBuscada;
	t_nombre_variable* nombreVariableCompartida;
	int32_t tamanioNombre = 0;
	bool existe = false;

	librocket_recibirMensaje(socketCPU, &tamanioNombre, sizeof(int32_t),
			infoLogger);

	nombreVariableCompartida = malloc(tamanioNombre);
	librocket_recibirMensaje(socketCPU, nombreVariableCompartida, tamanioNombre,
			infoLogger);
	char* nombreConAdmiracion = string_from_format("!%s",
			nombreVariableCompartida);

	int32_t contador;
	for (contador = 0; contador < list_size(variables_globales); contador++) {

		variableBuscada = list_get(variables_globales, contador);

		if (string_equals_ignore_case(variableBuscada->identificador,
				nombreConAdmiracion)) {

			existe = true;

			if (condicion == ASIGNAR_VALOR_COMPARTIDA) {
				librocket_recibirMensaje(socketCPU, &variableBuscada->valor,
						sizeof(t_valor_variable), infoLogger);
			}

			*valor = variableBuscada->valor;

			break;
		}
	}

	free(nombreVariableCompartida);
	free(nombreConAdmiracion);

	return existe;

}

t_semaforo* existeSemaforo(int32_t socketCPU) {

	t_semaforo* semaforoBuscado;
	int32_t tamanio = 0;

	librocket_recibirMensaje(socketCPU, &tamanio, sizeof(int32_t), infoLogger);

	t_nombre_variable* nombreSemaforo = malloc(tamanio);
	librocket_recibirMensaje(socketCPU, nombreSemaforo, tamanio, infoLogger);

	int32_t contador;
	for (contador = 0; contador < list_size(semaforos); contador++) {

		semaforoBuscado = list_get(semaforos, contador);

		if (string_equals_ignore_case(*semaforoBuscado->nombreSemaforo,
				nombreSemaforo)) {
			return semaforoBuscado;
		}
	}

	return NULL;

}

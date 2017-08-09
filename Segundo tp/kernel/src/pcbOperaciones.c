#include "headers/pcbOperaciones.h"

/*
 * Crear un nuevo PCB, con el pid correspondiente y su estado en NUEVO
 */
t_pcb* crearPcb() {

	t_pcb* pcb = malloc(sizeof(t_pcb));

	pcb->PID = (maxPID + 1);
	maxPID = pcb->PID;
	pcb->PC = 0;
	pcb->SP = 0;
	pcb->cantidad_paginas_totales = 0;
	pcb->estado = NUEVO;
	pcb->EC = -20;

	pcb->cant_entradas_indice_stack = 0;
	pcb->indice_stack = list_create();
	pcb->indice_codigo = list_create();
	//pcb->nombre_semaforo_que_lo_bloqueo = string_new();

	t_registro_stack* nuevoRegistro = malloc(sizeof(t_registro_stack));

	nuevoRegistro->posicion = pcb->cant_entradas_indice_stack;
	nuevoRegistro->argumentos = list_create();
	nuevoRegistro->variables = list_create();
	nuevoRegistro->pos_retorno = 0;

	t_direccion_logica direccionVacia;
	direccionVacia.offset = 0;
	direccionVacia.paginaId = 0;
	direccionVacia.pid = 0;
	direccionVacia.tamanio = 0;
	nuevoRegistro->pos_var_retorno = direccionVacia;

	list_add(pcb->indice_stack, nuevoRegistro);
	pcb->cant_entradas_indice_stack++;

	t_list* listaArchivos = list_create();
	//agrego 3 elementos iniciales representando STDIN, STDOUT Y STDERR
	entrada_pft entrada_inicial;
	list_add(listaArchivos, &entrada_inicial);
	list_add(listaArchivos, &entrada_inicial);
	list_add(listaArchivos, &entrada_inicial);
	dictionary_put(dicPID_PFT, itoa(pcb->PID), listaArchivos);

	//agrego una entrada en la tabla de info por proceso
	entrada_info_proceso* entrada = malloc(sizeof(entrada_info_proceso));
	entrada->pid = pcb->PID;
	entrada->rafagas_ejecutadas = 0;
	entrada->syscall_ejecutadas = 0;
	entrada->sem_bloqueado_por = string_new();
	entrada->cantidadAllocs =0;
	entrada->cantidadLiberaciones = 0;
	entrada->totalAllocado = 0;
	entrada->totalLiberado = 0;
	list_add(listaInfoProcesos, entrada);

	pcb->banderas.cambio_proceso = false;
	pcb->banderas.desconexion = false;
	pcb->banderas.ejecutando = false;
	pcb->banderas.llamada_a_funcion = false;
	pcb->banderas.llamada_sin_retorno = false;
	pcb->banderas.se_llamo_a_wait = false;
	pcb->banderas.terminacion_anormal = false;
	pcb->banderas.termino_programa = false;

	return pcb;
}

//no busca en los procesos finalizados
t_pcb* buscarPcbPorPID(int32_t pid) {

	int32_t sizeListos = list_size(colaListos);
	int32_t sizeEjecutando = list_size(colaEjecutando);
	int32_t sizeBloqueados = list_size(colaBloqueados);
	int32_t sizeNuevos = list_size(colaNuevos);

	int i = 0;
	for (; i < sizeListos; i++) {
		t_pcb* pcb = list_get(colaListos, i);
		if (pcb->PID == pid) {
			return pcb;
		}
	}

	i = 0;
	for (; i < sizeEjecutando; i++) {
		t_pcb* pcb = list_get(colaEjecutando, i);
		if (pcb->PID == pid) {
			return pcb;
		}
	}

	i = 0;
	for (; i < sizeBloqueados; i++) {
		t_pcb* pcb = list_get(colaBloqueados, i);
		if (pcb->PID == pid) {
			return pcb;
		}
	}
	i = 0;
	for (; i < sizeNuevos; i++) {
		t_pcb* pcb = list_get(colaNuevos, i);
		if (pcb->PID == pid) {
			return pcb;
		}
	}

	return NULL;
}

void limpiarSemaforosProceso(t_pcb* pcb){

entrada_info_proceso* entrada = 	obtenerInfoProceso(pcb);
if(entrada->sem_bloqueado_por == NULL || string_equals_ignore_case(entrada->sem_bloqueado_por,"")){
	//el proceso no estaba esperando ningun semaforo
	return;
}

int contador;
t_semaforo* semaforoBuscado;
for (contador = 0; contador < list_size(semaforos); contador++) {
	semaforoBuscado = list_get(semaforos, contador);
	if (string_equals_ignore_case(*semaforoBuscado->nombreSemaforo,
			entrada->sem_bloqueado_por)) {
		semaforoBuscado->valor++;
		log_info(infoLogger,"Signal del semaforo %s que estaba esperando el proceso %d, su nuevo valor es %d", *semaforoBuscado->nombreSemaforo,pcb->PID, semaforoBuscado->valor);
		free(entrada->sem_bloqueado_por);
		entrada->sem_bloqueado_por = string_new();
	}
}



}

void pasarPcbAFinalizados(t_pcb** pcb) {
	if(!planificacionActivada) {
			log_info(infoLogger,"Se entro en pasarPcbAFinalizados pero la planificacion esta detenida");
			return;
		}
	int c = 0;

	if ((*pcb)->estado == EJECUTANDO) {
		for (; c < list_size(colaEjecutando); c++) {
			t_pcb* p = list_get(colaEjecutando, c);
			if (p->PID == (*pcb)->PID) {
				list_remove(colaEjecutando, c);
				(*pcb)->estado = TERMINADO;
				list_add(colaFinalizados, *pcb);
				cantidadDeProcesosEnSistema--;
				limpiarSemaforosProceso(*pcb);
				checkSiDejoArchivosAbiertos((*pcb)->PID);
			}
		}

	} else if ((*pcb)->estado == LISTO) {
		for (; c < list_size(colaListos); c++) {
			t_pcb* p = list_get(colaListos, c);
			if (p->PID == (*pcb)->PID) {
				list_remove(colaListos, c);
				(*pcb)->estado = TERMINADO;
				list_add(colaFinalizados, *pcb);
				cantidadDeProcesosEnSistema--;
				limpiarSemaforosProceso(*pcb);
				checkSiDejoArchivosAbiertos((*pcb)->PID);
			}
		}
	} else if ((*pcb)->estado == BLOQUEADO) {
		for (; c < list_size(colaBloqueados); c++) {
			t_pcb* p = list_get(colaBloqueados, c);
			if (p->PID == (*pcb)->PID) {
				list_remove(colaBloqueados, c);
				(*pcb)->estado = TERMINADO;
				list_add(colaFinalizados, *pcb);
				cantidadDeProcesosEnSistema--;
				limpiarSemaforosProceso(*pcb);
				checkSiDejoArchivosAbiertos((*pcb)->PID);
			}
		}
	} else if ((*pcb)->estado == NUEVO) {
		for (; c < list_size(colaNuevos); c++) {
			t_pcb* p = list_get(colaNuevos, c);
			if (p->PID == (*pcb)->PID) {
				list_remove(colaNuevos, c);
				(*pcb)->estado = TERMINADO;
				list_add(colaFinalizados, *pcb);
			}
		}
	}
}

void pasarPcbDeNuevoAListos(t_pcb** pcb) {
	if(!planificacionActivada) {
				log_info(infoLogger,"Se entro en pasarPcbDeNuevoAListos pero la planificacion esta detenida");
				return;
			}
	(*pcb)->estado = LISTO;
	list_add(colaListos, *pcb);
	cantidadDeProcesosEnSistema++;
}

void pasarPcbDeNuevoAFinalizados(t_pcb** pcb, EXIT_CODE_TYPE code) {
	if(!planificacionActivada) {
				log_info(infoLogger,"Se entro en pasarPcbDeNuevoAFinalizados pero la planificacion esta detenida");
				return;
			}
	int tamanioNuevos = list_size(colaNuevos);
	int c = 0;
	for (; c < tamanioNuevos; c++) {
		t_pcb* p = list_get(colaNuevos, c);
		if (p->PID == (*pcb)->PID) {
			(*pcb)->estado = TERMINADO;
			(*pcb)->EC = code;
			list_remove(colaNuevos, c);
			list_add(colaFinalizados, *pcb);
			log_info(infoLogger,
					"Proceso con pid %d pasado de la cola de nuevos a finalizados con EC: %d",
					(*pcb)->PID, code);
			return;
		}
	}
	log_error(infoLogger,
			"No se encontro proceso con pid %d pasando de la cola de nuevos a finalizados con EC: %d",
			(*pcb)->PID, code);
}

void pasarPcbDeListosAEjecutando(t_pcb** pcb) {
	if(!planificacionActivada) {
				log_info(infoLogger,"Se entro en pasarPcbDeListosAejecutando pero la planificacion esta detenida");
				return;
			}
	int tamanioListos = list_size(colaListos);
	int c = 0;
	for (; c < tamanioListos; c++) {
		t_pcb * p = list_get(colaListos, c);
		if (p->PID == (*pcb)->PID) {
			(*pcb)->estado = EJECUTANDO;
			list_remove(colaListos, c);
			list_add(colaEjecutando, *pcb);
			return;
		}
	}
}

void pasarPcbDeEjecutandoAListos(t_pcb** pcb) {
	if(!planificacionActivada) {
				log_info(infoLogger,"Se entro en pasarPcbDeEjecutandoAListos pero la planificacion esta detenida");
				return;
			}
	int tamanio = list_size(colaEjecutando);
	int c = 0;
	for (; c < tamanio; c++) {
		t_pcb * p = list_get(colaEjecutando, c);
		if (p->PID == (*pcb)->PID) {
			(*pcb)->estado = LISTO;
			list_remove(colaEjecutando, c);
			list_add(colaListos, *pcb);
			return;
		}
	}
}

void pasarPcbDeBloqueadoAListo(t_pcb** pcb) {
	if(!planificacionActivada) {
					log_info(infoLogger,"Se entro en pasarPcbDeBloqueadoAListo pero la planificacion esta detenida");
					return;
				}
	int c = 0;
	for (; c < list_size(colaBloqueados); c++) {
		t_pcb* p = list_get(colaBloqueados, c);
		if (p->PID == (*pcb)->PID) {
			(*pcb)->estado = LISTO;
			list_remove(colaBloqueados, c);
			list_add(colaListos, *pcb);
			log_info(infoLogger,"Se desbloqueo al proceso %d",(*pcb)->PID);
			return;
		}
	}
}

void pasarPcbDeEjecutandoABloqueados(t_pcb** pcb) {
	if(!planificacionActivada) {
					log_info(infoLogger,"Se entro en pasarPcbDeEjecutandoABloqueados pero la planificacion esta detenida");
					return;
				}
	int tamanio = list_size(colaEjecutando);
	int c = 0;
	for (; c < tamanio; c++) {
		t_pcb * p = list_get(colaEjecutando, c);
		if (p->PID == (*pcb)->PID) {
			(*pcb)->estado = BLOQUEADO;
			list_remove(colaEjecutando, c);
			list_add(colaBloqueados, *pcb);
			return;
		}
	}
}

void pasarPcbDeEjecutandoAFinalizados(t_pcb** pcb) {
	if(!planificacionActivada) {
						log_info(infoLogger,"Se entro en pasarPcbDeEjecutandoAFinalizados pero la planificacion esta detenida");
						return;
					}
	int tamanio = list_size(colaEjecutando);
	int c = 0;
	for (; c < tamanio; c++) {
		t_pcb * p = list_get(colaEjecutando, c);
		if (p->PID == (*pcb)->PID) {
			(*pcb)->estado = TERMINADO;
			cantidadDeProcesosEnSistema--;
			list_remove(colaEjecutando, c);
			list_add(colaFinalizados, *pcb);
			limpiarSemaforosProceso(*pcb);
			return;
		}
	}
}

void pasarPcbDeListoAFinalizados(t_pcb** pcb) {
	if(!planificacionActivada) {
							log_info(infoLogger,"Se entro en pasarPcbDeListoAFinalizados pero la planificacion esta detenida");
							return;
						}
	int tamanioListos = list_size(colaListos);

	int c = 0;

	for (; c < tamanioListos; c++) {
		t_pcb * p = list_get(colaListos, c);

		if (p->PID == (*pcb)->PID) {
			(*pcb)->estado = TERMINADO;
			list_remove(colaListos, c);
			list_add(colaFinalizados, *pcb);
			cantidadDeProcesosEnSistema--;
			checkSiDejoArchivosAbiertos((*pcb)->PID);
			return;
		}

	}
}

void pasarPcbDeBloqueadoAfinalizados(t_pcb** pcb, EXIT_CODE_TYPE exitCode) {
	if(!planificacionActivada) {
								log_info(infoLogger,"Se entro en pasarPcbDeBloqueadoAfinalizados pero la planificacion esta detenida");
								return;
							}
	int cantidadBloqueados = list_size(colaBloqueados);
	int c = 0;
	for (; c < cantidadBloqueados; c++) {
		t_pcb* p = list_get(colaBloqueados, c);
		if (p->PID == (*pcb)->PID) {
			(*pcb)->estado = TERMINADO;
			(*pcb)->EC = exitCode;
			list_remove(colaBloqueados, c);
			list_add(colaFinalizados, *pcb);
			log_info(infoLogger,
					"Eliminado de la cola de bloqueados y agregado a finalizados el proceso con pid %d con exit code %d",
					(*pcb)->PID, exitCode);
			cantidadDeProcesosEnSistema--;
			limpiarSemaforosProceso(*pcb);
			checkSiDejoArchivosAbiertos((*pcb)->PID);
			return;
		}
	}
	log_error(infoLogger,
			"Se quiso pasar de bloqueados a finalizados un proceso con pid %d pero no se encontro, exitCode: %d ",
			(*pcb)->PID, exitCode);
}

/*
 * busca en la lista que contiene los sockets de los hilos programa y el pid asociado
 * devuelve -1 si no lo encuentra
 */
int32_t buscarPidByProgramaSocket(int32_t programaSocket) {
	int size = list_size(listaProgramaSocketPid);

	int c = 0;
	for (; c < size; c++) {
		entrada_programaSocket_pid* entrada = list_get(listaProgramaSocketPid,
				c);
		if (entrada->programaSocket == programaSocket) {
			return entrada->pid;
		}
	}
	return -1;
}

void eliminarPidProgramaSocket(int32_t programaSocket) {
	int size = list_size(listaProgramaSocketPid);
	int c = 0;
	for (; c < size; c++) {
		entrada_programaSocket_pid* entrada = list_get(listaProgramaSocketPid,
				c);
		if (entrada->programaSocket == programaSocket) {
			list_remove(listaProgramaSocketPid, c);
			return;
		}
	}
}

int32_t buscarProgramaSocketByPid(int32_t pid) {
	int size = list_size(listaProgramaSocketPid);
	int c = 0;
	for (; c < size; c++) {
		entrada_programaSocket_pid* entrada = list_get(listaProgramaSocketPid,
				c);
		if (entrada->pid == pid) {
			return entrada->programaSocket;
		}
	}
	return -1;
}

/*
 * Recibe un pcb, busca aquel pcb con el mismo PID en la cola de ejecutando, y lo reemplaza
 */
void reemplazarPcbEnEjecutando(t_pcb* pcb) {
	int size = list_size(colaEjecutando);
	int i = 0;
	for (; i < size; i++) {
		t_pcb* p = list_get(colaEjecutando, i);
		if (p->PID == pcb->PID) {
			list_remove(colaEjecutando, i);
			list_add_in_index(colaEjecutando, i, pcb);
		}
	}

}

/*
 *Busca en las CPUS en ejecucion si alguna tiene en ejecucion el pid parametro
 *En caso que se encuentre se devuelve el socket de la cpu
 *En caso contrario se devuelve 0
 */
int estaElProcesoEnEjecucion(int32_t pid) {
	int cpusOcupadasSize = list_size(cpusOcupadas);
	int i = 0;
	for (; i < cpusOcupadasSize; i++) {
		CPU* cpu = list_get(cpusOcupadas, i);
		if (cpu->PID_en_ejecucion == pid) {
			return cpu->socket;
		}
	}
	return 0; // no se encontro el proceso en ejecucion
}

entrada_info_proceso* obtenerInfoProceso(t_pcb* pcb){
	int i = 0;
	for(;i<list_size(listaInfoProcesos);i++){
		entrada_info_proceso* entrada = list_get(listaInfoProcesos,i);
		if(entrada->pid == pcb->PID){
			return entrada;
		}
	}
	log_error(infoLogger,"Se busco la entrada de info proceso %d pero no se encontro",pcb->PID);
	return NULL;

}

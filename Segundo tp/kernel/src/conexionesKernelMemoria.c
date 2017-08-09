#include "headers/conexionesKernelMemoria.h"

/*
 * retorna -2 si no hay suficiente memoria, failure si fallo por otros motivos,
 * success si se asignaron las paginas en memoria
 * modifica el exit code
 */
int pedirAMemoriaInicializacionDeProceso(t_pcb** punteroAptroPCB,
		char* codigoPrograma) {
	//para evitar tener que hablar con memoria en pruebas
	if (testSinMemoria)
		return SUCCESS;

	t_pcb* pcb = *punteroAptroPCB;

	//guardo en los logs info
	log_info(infoLogger,
			"Le pido a la memoria inicializar un proceso con el pid %d ",
			pcb->PID);

	//tengo que  pedir a la memoria que lo inicialice y me de las paginas y guardar el codigo en las paginas
	//pido inicialmente las paginas necesarias para guardar el codigo del programa y el stack maximo del mismo
	int32_t cantidadDePaginasAPedir =
			obtenerCantidadDePaginasParaIniciarProceso(codigoPrograma);

	//TODO
	PROTOCOLO_KERNEL_Y_CPU_A_MEMORIA inicializar = INICIALIZAR_PROGRAMA;
	librocket_enviarMensaje(memoriaSocket, &inicializar,
			sizeof(PROTOCOLO_KERNEL_Y_CPU_A_MEMORIA), infoLogger);
	librocket_enviarMensaje(memoriaSocket, &(pcb->PID), sizeof(int32_t),
			infoLogger);
	librocket_enviarMensaje(memoriaSocket, &cantidadDePaginasAPedir,
			sizeof(int32_t), infoLogger); //le mando la cantidad de paginas

	PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU respuesta;
	int rtaRecv = librocket_recibirMensaje(memoriaSocket, &respuesta,
			sizeof(PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU), infoLogger);

	if (rtaRecv == 0) {
		log_error(infoLogger, "---------SE CAYO LA MEMORIA!!------------");
		exit(1);
	}

	if (respuesta == PROCESO_INICIADO_EN_MEMORIA) {
		log_info(infoLogger, "proceso iniciado en memoria");
		pcb->cantidad_paginas_totales = cantidadDePaginasAPedir;

		pcb->SP = cantidadDePaginasAPedir - kernel_config->stackSize;
		pcb->fin_stack.offset = 0;
		pcb->fin_stack.paginaId = pcb->SP;
		pcb->fin_stack.pid = pcb->PID;
		pcb->fin_stack.tamanio = 4;

		return SUCCESS;
	} else if (respuesta == MEMORIA_INSUFICIENTE) {
		pcb->EC = NO_SE_PUDO_ASIGNAR_RECURSO;
		log_error(infoLogger,
				"No se pudo inicializar el proceso, memoria insuficiente");
		return -2;
	} else {
		pcb->EC = NO_DEFINIDO;
		log_error(infoLogger,
				"No se pudo inicializar el proceso, error no definido");
		return FAILURE;

	}
}

int obtenerCantidadDePaginasParaIniciarProceso(char* codigoPrograma) {
	log_info(infoLogger,
			"Calculando cantidad de paginas iniciales necesarias para un proceso...");
	int32_t tamanioCodigo = string_length(codigoPrograma);
	int n = redondearDivisionPaArriba(tamanioCodigo, kernel_config->pageSize)
			+ kernel_config->stackSize;
	log_info(infoLogger, "Paginas necesarias: %d", n);
	return n;
}

//devuelve SUCCESS o FAILURE
int redondearDivisionPaArriba(int dividendo, int divisor) {
	if (divisor == 0) {
		perror("SE QUISO DIVIDIR POR CERO");
		exit(1);
	}
	unsigned int division = dividendo / divisor;
	if (dividendo % divisor > 0) {
		division += 1;
	}
	return division;
}

//pide que se le de n nuevas paginas al proceso con id=pid

PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU pedirNPaginas(int pid, int cantidad) {

	agregarSyscallAPcb(pid);

	/*if (testSinMemoria)
	 return SUCCESS;

	 */

	log_info(infoLogger, "Haciendo un pedido de pagina para el PID %d....",
			pid);

	PROTOCOLO_KERNEL_Y_CPU_A_MEMORIA mensaje = PEDIDO_ASIGNAR_PAGINAS;

	librocket_enviarMensaje(memoriaSocket, &mensaje,
			sizeof(PROTOCOLO_KERNEL_Y_CPU_A_MEMORIA), infoLogger);
	librocket_enviarMensaje(memoriaSocket, &pid, sizeof(int32_t), infoLogger);
	librocket_enviarMensaje(memoriaSocket, &cantidad, sizeof(int32_t),
			infoLogger);

	PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU respuesta;
	librocket_recibirMensaje(memoriaSocket, &respuesta,
			sizeof(PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU), infoLogger);
	if (respuesta == PAGINAS_ASIGNADAS) {
		log_info(infoLogger, "Exitoso pedido de pagina nueva al proceso %d",
				pid);
		return SUCCESS;
	} else {
		log_error(infoLogger, "Fallo asignar una nueva página al proceso %d",
				pid);
		return FAILURE;
	}

}

/*
 * Envia el codigo del proceso con el PID a la memoria
 */
void enviarCodigoDePrograma(int32_t pid, char* codigo) {
	if (testSinMemoria)
		return;

	log_info(infoLogger,
			"iniciando envio de codigo del proceso  con PID %d a la memoria",
			pid);
	PROTOCOLO_KERNEL_Y_CPU_A_MEMORIA envioCodigoPrograma = ESCRIBIR_A_MEMORIA;
	int32_t tamanioCodigo = string_length(codigo);
	if (kernel_config->pageSize == 0) {
		printf("----AVISO---TAMAÑO DE PAGINA 0, CERRANDO");
		log_error(infoLogger, "----AVISO---TAMAÑO DE PAGINA 0, CERRANDO");
		free(codigo);
		exit(1);
	}
	int32_t paginasOcupadasPorCodigo = redondearDivisionPaArriba(tamanioCodigo,
			kernel_config->pageSize);

	int i = 0;
	char* codigoAEnviar = string_new();
	string_append(&codigoAEnviar, codigo);

	int32_t bytesTotalesAEnviar = tamanioCodigo;
	int bytesEnviados = 0;
	for (; i < paginasOcupadasPorCodigo; i++) {
		int offset = 0;
		t_direccion_logica* direccion = malloc(sizeof(t_direccion_logica));
		librocket_enviarMensaje(memoriaSocket, &envioCodigoPrograma,
				sizeof(PROTOCOLO_KERNEL_Y_CPU_A_MEMORIA), infoLogger);
		direccion->pid = pid;
		direccion->offset = offset;
		direccion->paginaId = i;

		char* codigoAEnviar = string_new();
		if (bytesTotalesAEnviar > kernel_config->pageSize) { // se escribe mas de una pagina
			int bytesAEscribir = kernel_config->pageSize;
			direccion->tamanio = bytesAEscribir;
			librocket_enviarMensaje(memoriaSocket, direccion,
					sizeof(t_direccion_logica), infoLogger);
			char* codigoCortado = string_substring_from(codigo, bytesEnviados);
			codigoAEnviar = string_substring_until(codigoCortado,
					bytesAEscribir);
			librocket_enviarMensaje(memoriaSocket, codigoAEnviar,
					direccion->tamanio, infoLogger);
			PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU resultadoEscritura;
			librocket_recibirMensaje(memoriaSocket, &resultadoEscritura,
					sizeof(PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU), infoLogger);
			if (!escrituraExitosa(direccion, codigo, resultadoEscritura)) {
				break;
				//TODO ver que pasa si no se pudo escribir el codigo, habria que tirar abajo el proceso
			}

			bytesEnviados += bytesAEscribir;
			bytesTotalesAEnviar -= bytesAEscribir;
			free(codigoAEnviar);

		} else {
			//mando  de una ya que entra en una pagina
			direccion->tamanio = bytesTotalesAEnviar;
			char* codigoCortado = string_substring_from(codigo, bytesEnviados);
			codigoAEnviar = string_substring_until(codigoCortado,
					bytesTotalesAEnviar);
			librocket_enviarMensaje(memoriaSocket, direccion,
					sizeof(t_direccion_logica), infoLogger);
			//mandar codigo
			librocket_enviarMensaje(memoriaSocket, codigoAEnviar,
					direccion->tamanio, infoLogger);
			PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU resultadoEscritura;
			librocket_recibirMensaje(memoriaSocket, &resultadoEscritura,
					sizeof(PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU), infoLogger);
			if (!escrituraExitosa(direccion, codigo, resultadoEscritura)) {
				break;
				//TODO ver que pasa si no se pudo escribir el codigo, habria que tirar abajo el proceso
			}

		}
	}
	log_info(infoLogger, "Codigo del programa enviado! (PID %d)", pid);
}

bool escrituraExitosa(t_direccion_logica* direccion, char* codigo,
		PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU resultadoEscritura) {
	if (resultadoEscritura != ESCRITURA_SATISFACTORIA) {
		log_info(infoLogger,
				"Error al querer escribir el codigo del proceso %d en la pagina %d con offset %d",
				direccion->pid, direccion->paginaId, direccion->offset);
		return false;
	} else
		return true;
}

void pedirAMemoriaFinalizarProceso(int32_t pid) {
	PROTOCOLO_KERNEL_Y_CPU_A_MEMORIA liberarMemoria = FINALIZAR_PROGRAMA;
	librocket_enviarMensaje(memoriaSocket, &liberarMemoria, sizeof(int32_t),
			infoLogger);

	librocket_enviarMensaje(memoriaSocket, &pid, sizeof(int32_t), infoLogger);

	PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU respuestaMemoria;
	librocket_recibirMensaje(memoriaSocket, &respuestaMemoria, sizeof(int32_t),
			infoLogger);
	if (respuestaMemoria == ELIMINACION_DE_PROCESO_SATISFACTORIO) {
		log_info(infoLogger, "Proceso %d borrado de la memoria exitosamente",
				pid);
	} else {
		log_info(infoLogger,
				"Hubo un error al eliminar el proceso %d de la memoria", pid);
	}
}

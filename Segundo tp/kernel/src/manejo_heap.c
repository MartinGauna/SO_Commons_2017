#include "headers/kernel.h"
#include "headers/capaMemoria.h"
#include "headers/conexionesKernelCpu.h"
#include "headers/manejo_heap.h"

int b_e;

void test_sin_memoria();
//int main(int argc, char* argv[]) {
int main_bis() {

	infoLogger = log_create(LOG_PATH, "kernel", false, LOG_LEVEL_INFO);

	test_sin_memoria();

	return EXIT_SUCCESS;
}

heap_metadata* generar_heap_metadata(int32_t bytesPedidos, t_bool bul) {

	// Se genera un bloque de metada y lo devuelve

	heap_metadata* metadata = malloc(METADATA_BLOQUE_SIZE);

	metadata->isFree = bul;
	metadata->size = bytesPedidos;

	return metadata;

}

int compactar(nodo_tabla_heap* pagina) {

	log_info(infoLogger, "COMPACTANDO...");

	log_info(infoLogger, "Compactando pagina %d", pagina->numeroPagina);

	//nodo_tabla_heap* estructuraPagina = list_get(tablaHeap, pagina);
	//t_list * bloques =  pagina->lista_de_bloques;
	int32_t size_bloques = list_size(pagina->lista_de_bloques);

	if (size_bloques > 1) {

		int i = 0;
		int j = 1;

		t_bloque* bloque1 = list_get(pagina->lista_de_bloques, i);
		t_bloque* bloque2 = list_get(pagina->lista_de_bloques, j);

		while (j < size_bloques) {

			if (bloque1->isUsed == false && bloque2->isUsed == false) {

				PROTOCOLO_KERNEL_Y_CPU_A_MEMORIA mensajeMemoria = ESCRIBIR_A_MEMORIA;

				t_direccion_logica puntero_struct_especial;
				puntero_struct_especial.pid = pagina->PID;
				puntero_struct_especial.paginaId = pagina->numeroPagina;
				puntero_struct_especial.tamanio = METADATA_BLOQUE_SIZE;
				puntero_struct_especial.offset = bloque1->offset- METADATA_BLOQUE_SIZE;

				heap_metadata* heap_metadata_especial = generar_heap_metadata(bloque1->size+bloque2->size+METADATA_BLOQUE_SIZE, '0');

				librocket_enviarMensaje(memoriaSocket, &mensajeMemoria,
						sizeof(PROTOCOLO_KERNEL_Y_CPU_A_MEMORIA), infoLogger);

				librocket_enviarMensaje(memoriaSocket, &puntero_struct_especial,
						sizeof(t_direccion_logica), infoLogger);

				librocket_enviarMensaje(memoriaSocket, heap_metadata_especial,METADATA_BLOQUE_SIZE, infoLogger);

				PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU respuestaMemoriaEspecial;

				librocket_recibirMensaje(memoriaSocket, &respuestaMemoriaEspecial,
						sizeof(PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU), infoLogger);

				if(respuestaMemoriaEspecial ==ERROR_EN_ESCRITURA){

					log_info(infoLogger, "Falló la escritura en memoria por lo tanto falló la compactación de la pagina %d del proceso %d",pagina->numeroPagina, pagina->PID);
					log_info(infoLogger, "Abortando compactación...");
					log_info(infoLogger, "COMPACTACIÓN ABORTADA");
					j = size_bloques;
					return 0;

				}else if(respuestaMemoriaEspecial ==ESCRITURA_SATISFACTORIA){
					//loguear q fue exitoso y seguir

					bloque1->size += bloque2->size + METADATA_BLOQUE_SIZE;

					log_info(infoLogger,"Se compactaron los bloques %d y %d, de la pagina %d",i, j, pagina->numeroPagina);
					log_info(infoLogger, "Quedaron compactados %d bytes",bloque1->size);

					list_remove(pagina->lista_de_bloques, j);
					return 1 + compactar(pagina);
					j = size_bloques;
				}

			} else {

				i++;
				j++;
				bloque1 = list_get(pagina->lista_de_bloques, i);
				bloque2 = list_get(pagina->lista_de_bloques, j);

			}
		}

		return 0;

	}

	int32_t ultimo_size = list_size(pagina->lista_de_bloques);

	t_bloque * bloque_especial = list_get(pagina->lista_de_bloques,
			ultimo_size - 1);

	bloque_especial->nro_bloque = -1;

	if (ultimo_size == size_bloques) {
		log_info(infoLogger, "No se compactó ningun bloque de la pagina %d",
				pagina->numeroPagina);
	}

	chekear_paginas_libres(pagina);

	return 0;
}

void contabilizarAlloc(int32_t pid, int32_t bytesPedidos) {
	int i = 0;
	for (; i < list_size(listaInfoProcesos); i++) {
		entrada_info_proceso* entrada = list_get(listaInfoProcesos, i);
		if (entrada->pid == pid) {
			entrada->cantidadAllocs++;
			entrada->totalAllocado += bytesPedidos;
			return;
		}
	}
}

void contabilizarFree(int32_t pid, int32_t bytesLiberados) {
	int i = 0;
	for (; i < list_size(listaInfoProcesos); i++) {
		entrada_info_proceso* entrada = list_get(listaInfoProcesos, i);
		if (entrada->pid == pid) {
			entrada->cantidadLiberaciones++;
			entrada->totalLiberado += bytesLiberados;
			return;
		}
	}
}

void asignar_paginas_heap(int32_t bytesPedidos, int32_t pid, int32_t socketCPU) {

	b_e = -2;

	log_info(infoLogger, "PEDIDO DE PAGINA HEAP");
	contabilizarAlloc(pid, bytesPedidos);

	int pagina = check_existe_pagina_con_espacio(bytesPedidos, pid);

	t_bloque* bloque_asignado;

	heap_metadata * heap_metadata_actual;

	t_direccion_logica puntero_struct;

	t_puntero puntero;

	if (pagina >= 0) {

		bloque_asignado = buscar_primer_bloque_libre(pagina, bytesPedidos);

		//PARTE AGREGADA PARA PARTIR UN BLOQUE INTERNO QUE NO ES EL ESPECIAL

		PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU rsp_memoria_particionada=ESCRITURA_SATISFACTORIA;

		if(bloque_asignado->size > bytesPedidos+METADATA_BLOQUE_SIZE){

			//El > es el minimo que puede tener de espacio disponible

			int indice=buscar_indice_bloque(pagina, bloque_asignado->offset, pid);

			if(indice  >= 0){

				t_bloque * bloque_particionado= malloc(sizeof(t_bloque));
				bloque_particionado->size= bloque_asignado->size-bytesPedidos-METADATA_BLOQUE_SIZE;
				bloque_particionado->isUsed=false;
				bloque_particionado->nro_bloque=0;
				bloque_particionado->offset=bloque_asignado->offset+bytesPedidos+METADATA_BLOQUE_SIZE;
				bloque_particionado->fin=bloque_particionado->offset+bloque_particionado->size;

				bloque_asignado->size=bytesPedidos;
				bloque_asignado->fin=bloque_asignado->offset+bytesPedidos;

				nodo_tabla_heap * estructura_pagina = buscar_pagina(pagina, pid);
				list_add_in_index(estructura_pagina->lista_de_bloques, indice+1, bloque_particionado);

				t_direccion_logica puntero_struct_particionado;
				puntero_struct_particionado.pid = pid;
				puntero_struct_particionado.paginaId = pagina;
				puntero_struct_particionado.tamanio = METADATA_BLOQUE_SIZE;
				puntero_struct_particionado.offset = bloque_particionado->offset- METADATA_BLOQUE_SIZE;

				heap_metadata* heap_metadata_particionada = generar_heap_metadata(bloque_particionado->size, '0');

				PROTOCOLO_KERNEL_Y_CPU_A_MEMORIA msj_particionado = ESCRIBIR_A_MEMORIA;

				librocket_enviarMensaje(memoriaSocket, &msj_particionado,
						sizeof(PROTOCOLO_KERNEL_Y_CPU_A_MEMORIA), infoLogger);

				librocket_enviarMensaje(memoriaSocket, &puntero_struct_particionado,
						sizeof(t_direccion_logica), infoLogger);

				librocket_enviarMensaje(memoriaSocket, heap_metadata_particionada, METADATA_BLOQUE_SIZE,
						infoLogger);

				librocket_recibirMensaje(memoriaSocket, &rsp_memoria_particionada,
						sizeof(PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU), infoLogger);


			}else {
				log_info(infoLogger, "No se encontró el indice del bloque para agregar el bloque particionado");
			}

		}

		//Generamos los punteros para mandar

		puntero_struct.pid = pid;
		puntero_struct.paginaId = pagina;
		puntero_struct.tamanio = bytesPedidos;
		puntero_struct.offset = bloque_asignado->offset;

		puntero = posicionMemoria_a_puntero(puntero_struct,
				kernel_config->pageSize);

		puntero_struct.offset = bloque_asignado->offset - METADATA_BLOQUE_SIZE;
		puntero_struct.tamanio = METADATA_BLOQUE_SIZE;

		heap_metadata_actual = generar_heap_metadata(bytesPedidos, '1');

		//genero el bloque especial particionado

		nodo_tabla_heap * pagina_struct = buscar_pagina(pagina, pid);

		int posicion_be = list_size(pagina_struct->lista_de_bloques);

		t_bloque * bloque_especial = list_get(pagina_struct->lista_de_bloques,
				posicion_be - 1);

		t_direccion_logica puntero_struct_especial;
		puntero_struct_especial.pid = pid;
		puntero_struct_especial.paginaId = pagina;
		puntero_struct_especial.tamanio = METADATA_BLOQUE_SIZE;
		puntero_struct_especial.offset = bloque_especial->offset- METADATA_BLOQUE_SIZE;

		heap_metadata* heap_metadata_especial = generar_heap_metadata(bloque_especial->size, '0');

		// Mensajeria
		// Mandamos al proceso

		PROTOCOLO_KERNEL_A_CPU mensajeACPU;
		PROTOCOLO_KERNEL_Y_CPU_A_MEMORIA mensajeAMemoria = ESCRIBIR_A_MEMORIA;

		librocket_enviarMensaje(memoriaSocket, &mensajeAMemoria,
				sizeof(PROTOCOLO_KERNEL_Y_CPU_A_MEMORIA), infoLogger);

		librocket_enviarMensaje(memoriaSocket, &puntero_struct,
				sizeof(t_direccion_logica), infoLogger);

		librocket_enviarMensaje(memoriaSocket, heap_metadata_actual, METADATA_BLOQUE_SIZE,
				infoLogger);

		PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU respuestaMemoria;

		librocket_recibirMensaje(memoriaSocket, &respuestaMemoria,
				sizeof(PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU), infoLogger);

		librocket_enviarMensaje(memoriaSocket, &mensajeAMemoria,
				sizeof(PROTOCOLO_KERNEL_Y_CPU_A_MEMORIA), infoLogger);

		librocket_enviarMensaje(memoriaSocket, &puntero_struct_especial,
				sizeof(t_direccion_logica), infoLogger);

		librocket_enviarMensaje(memoriaSocket, heap_metadata_especial,
		METADATA_BLOQUE_SIZE, infoLogger);

		PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU respuestaMemoriaEspecial;

		librocket_recibirMensaje(memoriaSocket, &respuestaMemoriaEspecial,
				sizeof(PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU), infoLogger);

		if (respuestaMemoria == ERROR_EN_ESCRITURA	|| respuestaMemoriaEspecial == ERROR_EN_ESCRITURA || rsp_memoria_particionada==ERROR_EN_ESCRITURA) {
			mensajeACPU = PEDIDO_MEMORIA_RECHAZADO;
			librocket_enviarMensaje(socketCPU, &mensajeACPU,
					sizeof(PROTOCOLO_KERNEL_A_CPU), infoLogger);

		} else if (respuestaMemoria == ESCRITURA_SATISFACTORIA && respuestaMemoriaEspecial == ESCRITURA_SATISFACTORIA && rsp_memoria_particionada==ESCRITURA_SATISFACTORIA) {

			mensajeACPU = DIRECCION_ENVIADA;
			librocket_enviarMensaje(socketCPU, &mensajeACPU,
					sizeof(PROTOCOLO_KERNEL_A_CPU), infoLogger);
			librocket_enviarMensaje(socketCPU, &puntero, sizeof(t_puntero),
					infoLogger);

		}

	} else {

		//No existe pagina con su pid que tenga espacio, ni paginas libres,

		//Debe crear nueva pagina

		PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU response = pedirNPaginas(pid, 1);

		if (response == SUCCESS) {

			pagina = calcular_numero_nuevapagina(pid);

			log_info(infoLogger,
					"Se creo una nueva pagina nro %d para el proceso %d",
					pagina, pid);

			nodo_tabla_heap* nueva_pagina = crear_nueva_pagina_heap(
					bytesPedidos, pid, pagina);

			int indice_bloqueEspeciaL = indice_bloque_especial(
					nueva_pagina->lista_de_bloques);
			t_bloque* bloque_especial = list_get(nueva_pagina->lista_de_bloques,
					indice_bloqueEspeciaL);

			bloque_asignado = crear_nuevo_bloque(bytesPedidos, bloque_especial,
					nueva_pagina->lista_de_bloques);
			recalcular_offset_final(bloque_especial, bloque_asignado);

			list_remove(nueva_pagina->lista_de_bloques, indice_bloqueEspeciaL);
			list_add(nueva_pagina->lista_de_bloques, bloque_asignado);
			list_add(nueva_pagina->lista_de_bloques, bloque_especial);

			//Generamos los punteros para mandar

			puntero_struct.pid = pid;
			puntero_struct.paginaId = pagina;
			puntero_struct.tamanio = bytesPedidos;
			puntero_struct.offset = bloque_asignado->offset;

			puntero = posicionMemoria_a_puntero(puntero_struct,
					kernel_config->pageSize);

			puntero_struct.offset = bloque_asignado->offset
					- METADATA_BLOQUE_SIZE;
			puntero_struct.tamanio = METADATA_BLOQUE_SIZE;

			heap_metadata_actual = generar_heap_metadata(bytesPedidos, '1');

			// Mensajeria
			// Mandamos al proceso

			//genero el bloque especial particionado

			t_direccion_logica puntero_struct_especial;
			puntero_struct_especial.pid = pid;
			puntero_struct_especial.paginaId = pagina;
			puntero_struct_especial.tamanio = METADATA_BLOQUE_SIZE;
			puntero_struct_especial.offset = bloque_especial->offset
					- METADATA_BLOQUE_SIZE;

			heap_metadata* heap_metadata_especial = generar_heap_metadata(
					bloque_especial->size, '0');

			// Mensajeria
			// Mandamos al proceso

			PROTOCOLO_KERNEL_A_CPU mensajeACPU;
			PROTOCOLO_KERNEL_Y_CPU_A_MEMORIA mensajeAMemoria =
					ESCRIBIR_A_MEMORIA;

			librocket_enviarMensaje(memoriaSocket, &mensajeAMemoria,
					sizeof(PROTOCOLO_KERNEL_Y_CPU_A_MEMORIA), infoLogger);

			librocket_enviarMensaje(memoriaSocket, &puntero_struct,
					sizeof(t_direccion_logica), infoLogger);

			librocket_enviarMensaje(memoriaSocket, heap_metadata_actual,
			METADATA_BLOQUE_SIZE, infoLogger);

			PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU respuestaMemoria;

			librocket_recibirMensaje(memoriaSocket, &respuestaMemoria,
					sizeof(PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU), infoLogger);

			librocket_enviarMensaje(memoriaSocket, &mensajeAMemoria,
					sizeof(PROTOCOLO_KERNEL_Y_CPU_A_MEMORIA), infoLogger);

			librocket_enviarMensaje(memoriaSocket, &puntero_struct_especial,
					sizeof(t_direccion_logica), infoLogger);

			librocket_enviarMensaje(memoriaSocket, heap_metadata_especial,
			METADATA_BLOQUE_SIZE, infoLogger);

			PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU respuestaMemoriaEspecial;

			librocket_recibirMensaje(memoriaSocket, &respuestaMemoriaEspecial,
					sizeof(PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU), infoLogger);

			if (respuestaMemoria == ERROR_EN_ESCRITURA
					&& respuestaMemoriaEspecial == ERROR_EN_ESCRITURA) {
				mensajeACPU = PEDIDO_MEMORIA_RECHAZADO;
				librocket_enviarMensaje(socketCPU, &mensajeACPU,
						sizeof(PROTOCOLO_KERNEL_A_CPU), infoLogger);

			} else if (respuestaMemoria == ESCRITURA_SATISFACTORIA
					&& respuestaMemoriaEspecial == ESCRITURA_SATISFACTORIA) {

				mensajeACPU = DIRECCION_ENVIADA;
				librocket_enviarMensaje(socketCPU, &mensajeACPU,
						sizeof(PROTOCOLO_KERNEL_A_CPU), infoLogger);
				librocket_enviarMensaje(socketCPU, &puntero, sizeof(t_puntero),
						infoLogger);

			}

		} else {

			CPU* cpu = buscarCPUBySocket(socketCPU);

			t_pcb* pcb = buscarPcbPorPID(cpu->PID_en_ejecucion);

			pcb->EC = NO_SE_PUEDEN_DAR_MAS_PAGINAS;
			PROTOCOLO_KERNEL_A_CPU rechazado = PEDIDO_MEMORIA_RECHAZADO;
			librocket_enviarMensaje(socketCPU, &rechazado,
					sizeof(PROTOCOLO_KERNEL_A_CPU), infoLogger);
			int32_t programaSocket = buscarProgramaSocketByPid(pcb->PID);
			PROTOCOLO_KERNEL_A_CONSOLA msjPRocesoRechazado = PROCESO_TERMINADO;
			librocket_enviarMensaje(programaSocket, &msjPRocesoRechazado,
					sizeof(PROTOCOLO_KERNEL_A_CONSOLA), infoLogger);
			librocket_enviarMensaje(programaSocket, &(pcb->EC), sizeof(int32_t),
					infoLogger);

			log_info(infoLogger, "No se le pudo dar una pagina al proceso %d",
					pid);
		}

	}

}

int indice_bloque_especial(t_list * lista_de_bloques) {

	int i;
	t_bloque * bloque_actual;

	for (i = 0; i < list_size(lista_de_bloques); i++) {

		bloque_actual = list_get(lista_de_bloques, i);

		if (bloque_actual->nro_bloque < 0)
			return i;
	}

	//NUNCA DEBERIA PASAR!
	return 0;
}

t_bloque* buscar_primer_bloque_libre(int pagina, int bytesPedidos) {
//3

	t_bloque* bloque_libre;

	int i;
	int j;

	int size = list_size(tablaHeap);

	for (i = 0; i < size; i++) {

		nodo_tabla_heap* nodo_pagina = list_get(tablaHeap, i);

		if (nodo_pagina->numeroPagina == pagina) {

			int size_bloques = list_size(nodo_pagina->lista_de_bloques);

			for (j = 0; j < size_bloques; j++) {

				bloque_libre = list_get(nodo_pagina->lista_de_bloques, j);

				if (bloque_libre->isUsed == false
						&& bloque_libre->size >= bytesPedidos
						&& bloque_libre->nro_bloque >= 0) {
					b_e = 0;

					ocupar_bloque(bloque_libre, bytesPedidos);
					i = size;
					j = size_bloques;

				} else if (bloque_libre->isUsed == false
						&& bloque_libre->size
								>= (bytesPedidos + METADATA_BLOQUE_SIZE)
						&& bloque_libre->nro_bloque < 0) {

					//SE parte el bloque especial

					b_e = -1;
					t_bloque* bloque_nuevo = crear_nuevo_bloque(bytesPedidos,
							bloque_libre, nodo_pagina->lista_de_bloques);
					recalcular_offset_final(bloque_libre, bloque_nuevo);

					t_bloque* bloque_especial = list_get(
							nodo_pagina->lista_de_bloques, j);

					list_remove(nodo_pagina->lista_de_bloques, j);
					list_add(nodo_pagina->lista_de_bloques, bloque_nuevo);
					list_add(nodo_pagina->lista_de_bloques, bloque_especial);

					bloque_libre = bloque_nuevo;

					i = size;
					j = size_bloques;
				}

			}

		}
	}

	return bloque_libre;

}

void ocupar_bloque(t_bloque* bloque, int bytesPedidos) {
	bloque->isUsed = true;
	bloque->fin = bytesPedidos;
}

t_bloque* crear_nuevo_bloque(int bytesPedidos, t_bloque* bloque_especial,
		t_list * lista_bloques) {

	t_bloque * bloque_nuevo = malloc(sizeof(t_bloque));

	bloque_nuevo->offset = bloque_especial->offset;
	bloque_nuevo->isUsed = true;
	bloque_nuevo->nro_bloque = buscar_ultimo_bloque_bis(lista_bloques);
	bloque_nuevo->size = bytesPedidos;
	bloque_nuevo->fin = bloque_nuevo->offset + bytesPedidos;

	return bloque_nuevo;
}

void recalcular_offset_final(t_bloque* bloque_especial, t_bloque* bloque_nuevo) {

	bloque_especial->offset = bloque_especial->offset + bloque_nuevo->size
			+ (METADATA_BLOQUE_SIZE);

	bloque_especial->size = bloque_especial->size - bloque_nuevo->size
			- (METADATA_BLOQUE_SIZE);

	//No se deberia considerar el metadatabloquesize, porque el espacio en el q se inicia
	//ya considera esos 5kb

}

int buscar_ultimo_bloque_bis(t_list* lista_bloques) {

	int nro_max_bloque = 0;

	t_bloque * bloque_retorno = malloc(sizeof(t_bloque));
	bloque_retorno->nro_bloque = 0;

	int size = list_size(lista_bloques);

	int i;
	for (i = 0; i < size; i++) {

		t_bloque* ultimo_bloque = list_get(lista_bloques, i);

		if (ultimo_bloque->nro_bloque > nro_max_bloque) {
			nro_max_bloque = ultimo_bloque->nro_bloque;
			bloque_retorno = ultimo_bloque;
		}
	}

	return bloque_retorno->nro_bloque;
}

int check_existe_pagina_con_espacio(int bytesPedidos, int pid) {

	int i = 0;

	int cantidadPaginasAsignadas = list_size(tablaHeap);

	for (; i < cantidadPaginasAsignadas; i++) {

		nodo_tabla_heap* entradaTabla = list_get(tablaHeap, i);

		if (entradaTabla->PID == pid) {

			entradaTabla->PID = pid;

			int j = 0;

			int size_bloques = list_size(entradaTabla->lista_de_bloques);

			for (; j < size_bloques; j++) {

				t_bloque* bloque = list_get(entradaTabla->lista_de_bloques, j);

				if (bloque->size
						>= bytesPedidos&& bloque->nro_bloque>=0 && bloque->isUsed == false) {

					log_info(infoLogger,
							"Se le dieron al proceso %d , %d bytes de la pagina %d",
							pid, bytesPedidos, bloque->size,
							entradaTabla->numeroPagina);
					j = size_bloques;
					i = cantidadPaginasAsignadas;

					return entradaTabla->numeroPagina;

				} else if (bloque->size
						>= (bytesPedidos + METADATA_BLOQUE_SIZE)&& bloque->nro_bloque<0 && bloque->isUsed == false) {
					log_info(infoLogger,
							"Se le dieron al proceso %d , %d bytes, de un bloque nuevo de la pagina %d",
							pid, bytesPedidos, entradaTabla->numeroPagina);

					j = size_bloques;
					i = cantidadPaginasAsignadas;
					return entradaTabla->numeroPagina;

				}

			}

		}

	}

	log_info(infoLogger,
			"El proceso %d pidio %d bytes pero no tiene ninguna pagina asignada o no tiene paginas con espacio para alocar lo que pidio",
			pid, bytesPedidos);

	return -1; // no tiene espacio en la spaginas asignadas o no tiene paginas asigadas
}

nodo_tabla_heap* crear_nueva_pagina_heap(int bytesPedidos, int pid,
		int nro_pagina) {

	nodo_tabla_heap* nueva_pagina = malloc(sizeof(nodo_tabla_heap));

	nueva_pagina->PID = pid;
	nueva_pagina->espacio_bloque_maximo = kernel_config->pageSize
			- METADATA_BLOQUE_SIZE;
	nueva_pagina->numeroPagina = nro_pagina;
	nueva_pagina->lista_de_bloques = list_create();

	t_bloque* bloque_especial = malloc(sizeof(t_bloque));
	bloque_especial->isUsed = false;
	bloque_especial->nro_bloque = -1;
	bloque_especial->offset = 5;
	bloque_especial->size = kernel_config->pageSize - METADATA_BLOQUE_SIZE;
	bloque_especial->fin = kernel_config->pageSize;

	list_add(nueva_pagina->lista_de_bloques, bloque_especial);

	list_add(tablaHeap, nueva_pagina);

	return nueva_pagina;
}

int calcular_numero_nuevapagina(int pid) {

	t_pcb* pcb = buscarPcbPorPID(pid);

	int max = pcb->cantidad_paginas_totales;

	int size_tabla = list_size(tablaHeap);

	int i = 0;

	for (; i < size_tabla; i++) {

		nodo_tabla_heap * pagina = list_get(tablaHeap, i);

		if (pagina->numeroPagina >= max && pagina->PID == pid) {

			max = pagina->numeroPagina;

			max++;

		}
	}

	return max;
}

void liberar_bloque_heap(t_puntero puntero, int32_t pid, int socketCPU) {

	t_direccion_logica puntero_struct = puntero_a_posicionMemoria(puntero,
			kernel_config->pageSize, pid);

	nodo_tabla_heap * pagina;

	int size_tabla = list_size(tablaHeap);
	int flag = -1;
	int i = 0;
	int nro_pag;

	int nro_bloque;

	int j = 0;

	int tamanio;

	for (; i < size_tabla; i++) {

		pagina = list_get(tablaHeap, i);

		if (pagina->PID == pid
				&& pagina->numeroPagina == puntero_struct.paginaId) {

			int size_bloques = list_size(pagina->lista_de_bloques);

			for (; j < size_bloques; j++) {

				t_bloque * bloque_a_borrar = list_get(pagina->lista_de_bloques,
						j);
				contabilizarFree(pid, bloque_a_borrar->size);

				if (bloque_a_borrar->offset == puntero_struct.offset) {
					flag=0;
					//log_info(infoLogger,"El proceso %d libero un bloque de %d de heap", pid, bloque_a_borrar->size);

					bloque_a_borrar->isUsed = false;

					tamanio = bloque_a_borrar->size;
					nro_pag = i;
					nro_bloque = j;
					j = size_bloques;
					i = size_tabla;
				}
			}

		}

	}

	if(flag==-1){

		PROTOCOLO_KERNEL_A_CPU mensajeACPU=LIBERACION_INVALIDA;
		librocket_enviarMensaje(socketCPU, &mensajeACPU,sizeof(PROTOCOLO_KERNEL_A_CPU), infoLogger);
		return;
	}

	puntero_struct.offset = puntero_struct.offset - METADATA_BLOQUE_SIZE;
	puntero_struct.tamanio = METADATA_BLOQUE_SIZE;

	heap_metadata* heap_falso = generar_heap_metadata(tamanio, '0');

	PROTOCOLO_KERNEL_A_CPU mensajeACPU;

	PROTOCOLO_KERNEL_Y_CPU_A_MEMORIA mensajeAMemoria = ESCRIBIR_A_MEMORIA;

	librocket_enviarMensaje(memoriaSocket, &mensajeAMemoria,
			sizeof(PROTOCOLO_KERNEL_Y_CPU_A_MEMORIA), infoLogger);

	librocket_enviarMensaje(memoriaSocket, &puntero_struct,
			sizeof(t_direccion_logica), infoLogger);

	librocket_enviarMensaje(memoriaSocket, heap_falso, METADATA_BLOQUE_SIZE,
			infoLogger);

	PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU respuestaMemoria;

	librocket_recibirMensaje(memoriaSocket, &respuestaMemoria,
			sizeof(PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU), infoLogger);

	agregarSyscallAPcb(pid);

	if (respuestaMemoria == ERROR_EN_ESCRITURA) {
		mensajeACPU = LIBERACION_INVALIDA;
		librocket_enviarMensaje(socketCPU, &mensajeACPU,
				sizeof(PROTOCOLO_KERNEL_A_CPU), infoLogger);

	} else if (respuestaMemoria == ESCRITURA_SATISFACTORIA) {

		mensajeACPU = LIBERACION_EXITOSA;
		librocket_enviarMensaje(socketCPU, &mensajeACPU,
				sizeof(PROTOCOLO_KERNEL_A_CPU), infoLogger);

		nodo_tabla_heap * pagina_borrar = list_get(tablaHeap, nro_pag);

		t_bloque * bloque_borrar = list_get(pagina_borrar->lista_de_bloques,
				nro_bloque);

		bloque_borrar->isUsed = false;

		log_info(infoLogger, "El proceso %d libero un bloque de %d de heap",
				pid, bloque_borrar->size);

		compactar(pagina_borrar);
	}

}

void chekear_paginas_libres(nodo_tabla_heap* estructuraPagina) {

	//nodo_tabla_heap* estructuraPagina = list_get(tablaHeap, pagina);

	int size = list_size(estructuraPagina->lista_de_bloques);

	if (size == 1) {

		PROTOCOLO_KERNEL_Y_CPU_A_MEMORIA mensaje_memoria = LIBERAR_PAGINA;

		librocket_enviarMensaje(memoriaSocket, &mensaje_memoria,
				sizeof(PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU), infoLogger);

		librocket_enviarMensaje(memoriaSocket, &estructuraPagina->PID,
				sizeof(int32_t), infoLogger);

		librocket_enviarMensaje(memoriaSocket, &estructuraPagina->numeroPagina,
				sizeof(int32_t), infoLogger);

		PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU respuesta;

		librocket_recibirMensaje(memoriaSocket, &respuesta,
				sizeof(PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU), infoLogger);

		if (respuesta == PAGINA_LIBERADA) {

			log_info(infoLogger,
					"Se liberó la pagina %d utilizada por el proceso %d \n ",
					estructuraPagina->numeroPagina, estructuraPagina->PID);

			borrar_pagina_en_kernel(estructuraPagina);

		} else {

			log_info(infoLogger,
					"No se puso borrar la pagina %d del proceso %d\n ",
					estructuraPagina->numeroPagina, estructuraPagina->PID);

		}
	}

}

void informar_paginas_heap(int pid) {

	//printf("----INFORMACION SOBRE PAGINAS DE HEAP DEL PROCESO %d----",pid);
	log_info(infoLogger,
			"----INFORMACION SOBRE PAGINAS DE HEAP DEL PROCESO %d----");

	int size_tabla = list_size(tablaHeap);

	int i = 0;

	int flag = -1;

	for (; i < size_tabla; i++) {

		nodo_tabla_heap * pagina = list_get(tablaHeap, i);

		if (pagina->PID == pid) {
			flag = 0;

			//printf("El proceso  %d tiene la pagina %d sin liberar\n",pid, pagina->numeroPagina);
			log_info(infoLogger,
					"El proceso  %d tiene la pagina %d sin liberar\n", pid,
					pagina->numeroPagina);

		}

	}

	if (flag == -1) {

		//printf("El proceso  %d libero todas las paginas de heap\n",pid);
		log_info(infoLogger, "El proceso  %d libero todas las paginas de heap",
				pid);

	}

	liberar_paginas(pid);
}

void liberar_paginas(int pid) {

	log_info(infoLogger, "Liberando paginas del proceso %d", pid);

	int pid_antiguo;

	int nro_antiguo;

	int size_tabla = list_size(tablaHeap);

	int i = 0;

	for (; i < size_tabla; i++) {

		nodo_tabla_heap * pagina = list_get(tablaHeap, i);

		if (pagina->PID == pid) {

			pid_antiguo = pagina->PID;

			nro_antiguo = pagina->numeroPagina;

			list_clean(pagina->lista_de_bloques);

			pagina->PID = -1;

			pagina->numeroPagina = -1;

			t_bloque * bloque_especial = malloc(sizeof(t_bloque));

			bloque_especial->isUsed = false;
			bloque_especial->nro_bloque = -1;
			bloque_especial->offset = 5;
			bloque_especial->size = kernel_config->pageSize
					- METADATA_BLOQUE_SIZE;
			bloque_especial->fin = kernel_config->pageSize;

			list_add(pagina->lista_de_bloques, bloque_especial);

			log_info(infoLogger,
					"Se libero la pagina %d perteneciente al proceso %d",
					nro_antiguo, pid_antiguo);

		}

	}

}

void borrar_pagina_en_kernel(nodo_tabla_heap* pagina) {

	int size = list_size(tablaHeap);

	int i = 0;

	for (; i < size; i++) {
		nodo_tabla_heap* pagina_a_borrar = list_get(tablaHeap, i);

		if (pagina_a_borrar->PID == pagina->PID
				&& pagina_a_borrar->numeroPagina == pagina->numeroPagina) {

			list_remove(tablaHeap, i);

			i = size;
		}
	}

}

nodo_tabla_heap * buscar_pagina(int32_t pagina, int32_t pid) {

	int size = list_size(tablaHeap);
	int i;

	nodo_tabla_heap* nodo_pagina;
	for (i = 0; i < size; i++) {

		nodo_pagina = list_get(tablaHeap, i);

		if (nodo_pagina->numeroPagina == pagina && nodo_pagina->PID==pid) {
			return nodo_pagina;
		}
	}
	return nodo_pagina;
}

int buscar_indice_bloque(int32_t pagina, int32_t offset, int32_t pid){

	nodo_tabla_heap* nodo_pagina=buscar_pagina(pagina,pid);

	int size = list_size(nodo_pagina->lista_de_bloques);

	int i =0;

	for(; i  < size; i++){

		t_bloque * bloque =list_get(nodo_pagina->lista_de_bloques, i);

		if(bloque->offset== offset){
			return i;
		}
	}

	return -1;

}

#include "headers/capaMemoria.h"


/*

int checkSiExistePaginaConEspacio(int bytesPedidos, int pid) {

	int i = 0;

	int cantidadPaginasAsignadas = list_size(tablaHeap);

	for (; i < cantidadPaginasAsignadas; i++) {

		nodo_tabla_heap* entradaTabla = list_get(tablaHeap, i);

		if (entradaTabla->PID == pid) {

			if (entradaTabla->espacioDisponible
					>= (bytesPedidos + METADATA_BLOQUE_SIZE)) {

				int j = 0;

				int size_bloques = list_size(entradaTabla->lista_de_bloques);

				for (; j < size_bloques; j++) {

					t_bloque* bloque = list_get(entradaTabla->lista_de_bloques,
							j);

					if (bloque->size >= (bytesPedidos + METADATA_BLOQUE_SIZE)) {

						log_info(infoLogger,
								"se le dieron al proceso %d , %d bytes, tenia libres %d ",
								pid, bytesPedidos,
								entradaTabla->espacioDisponible);
						return entradaTabla->numeroPagina;
					}

				}
			}

			//if (entradaTabla->espacioDisponible >= (bytesPedidos + METADATA_BLOQUE_SIZE)) {

			//char* mensaje = malloc(100);

			//sprintf(mensaje,"se le dieron al proceso %d , %d bytes, tenia libres %d ",pid, bytesPedidos, entradaTabla->espacioDisponible);
			//log_info(infoLogger, mensaje);
			//free(mensaje);

			return entradaTabla->numeroPagina;

			//	} else {
			//char* mensaje = malloc(100);
			//sprintf(mensaje,"el proceso %d pidio %d bytes y se necesita una nueva pagina, tenia libres %d ",pid, bytesPedidos, entradaTabla->espacioDisponible);

			//log_info(infoLogger, mensaje);
			//free(mensaje);
			//		return -1;
			//	}
		}
	}

	log_info(infoLogger,
			"el proceso %d pidio %d bytes pero no tiene ninguna pagina asignada o no tiene paginas con espacio para alocar lo que pidio",
			pid, bytesPedidos);

	return -1; // no tiene paginas asignadas
}

*/

/*
int asignarNuevaPagina(int pid, int nro_pages) {

	int i = 0;
	int max_num = 0;

	nodo_tabla_heap nueva_page;

	nueva_page.PID = pid;
	nueva_page.espacioDisponible = kernel_config->stackSize;

	int cantpages = list_size(tablaHeap);

	for (; i < cantpages; i++) {

		nodo_tabla_heap* entradaTabla = list_get(tablaHeap, i);

		if (entradaTabla->numeroPagina > max_num) {
			max_num = entradaTabla->numeroPagina;
		}
	}

	nueva_page.numeroPagina = max_num + 1;

	return nueva_page.numeroPagina;

}

*/


/*

void actualizarNodoHeap(int pagina, int pid, int bytesPedidos) {

	//TODO MODIFICAR LA METADATA DE LA PAGINA SI LA PAGINA ESTA LIBRE

	//Actualiza la tabla de paginas cuando se ocupo un espacio pedido por algun proceso

	int cantpages = list_size(tablaHeap);

	int i;

	for (i = 0; i < cantpages; i++) {

		nodo_tabla_heap* entrada = list_get(tablaHeap, i);

		if (entrada->numeroPagina == pagina) {

			t_bloque* bloque_nuevo = malloc(sizeof(t_bloque));

			bloque_nuevo->isUsed = true;
			//bloque_nuevo.nro_bloque=3;
			bloque_nuevo->offset = calcular_offset(pagina, bytesPedidos);
			bloque_nuevo->size = bytesPedidos;

			list_add(entrada->lista_de_bloques, bloque_nuevo);

			entrada->espacioDisponible = entrada->espacioDisponible
					- (bytesPedidos + METADATA_BLOQUE_SIZE);

		}

	}

}

nodo_tabla_heap* nuevaPagina(int pid) {

	//Crea una nueva pagina con el nro máximo+1 de la pagina ultima
	// Y crea un struct del bloque para agregarlo a la lista

	int max_num = 0;
	nodo_tabla_heap nueva_page;
	int cantpages = list_size(tablaHeap);

	nueva_page.PID = pid;
	nueva_page.espacioDisponible = kernel_config->pageSize;

	int j;
	for (j = 0; j < cantpages; j++) {

		nodo_tabla_heap* entradaTabla = list_get(tablaHeap, j);

		if (entradaTabla->numeroPagina > max_num) {
			max_num = entradaTabla->numeroPagina;
		}
	}

	nueva_page.numeroPagina = max_num + 1;

	list_add(tablaHeap, &nueva_page);

	heap_metadata pagina_memoria;

	pagina_memoria.size = kernel_config->pageSize - METADATA_BLOQUE_SIZE;
	pagina_memoria.isFree = true;

	return &nueva_page;

}

void actualizar_estructuras_heap(t_direccion_logica puntero_a_borrar) {

	int nro_pag = puntero_a_borrar.paginaId;
	int pid = puntero_a_borrar.pid;
	int offset = puntero_a_borrar.offset;
	int size = puntero_a_borrar.tamanio;

	int size_paginas = list_size(tablaHeap);

	int i = 0;

	for (; i < size_paginas; i++) {

		nodo_tabla_heap * pagina = list_get(tablaHeap, i);

		if (pagina->PID == pid && pagina->numeroPagina == nro_pag) {

			pagina->espacioDisponible = pagina->espacioDisponible + size;

			int size_bloques = list_size(pagina->lista_de_bloques);

			int j = 0;

			for (; j < size_bloques; j++) {

				t_bloque * bloque_a_actualizar = list_get(
						pagina->lista_de_bloques, j);

				if (bloque_a_actualizar->offset == offset
						&& bloque_a_actualizar->size == size) {

					bloque_a_actualizar->isUsed = false;
				}

			}

		}

	}

	//lista_de_bloques
	// heap->isFree=0;

	// Acá se actualiza lo que conoce kernel sobre las paginas y bloques, pero falta
	// ubicar la pagina que corresponde al bloque que se va a liberar.

	//nodo_tabla_heap nodo;

	//nodo.espacioDisponible= nodo.espacioDisponible + heap.size;

}

void compactar() {

	//TODO Juntar los bloques consecutivos vacios.

	char * binary = string_new();

	int32_t size_paginas = list_size(&tablaHeap);

	int i;

	for (i = 0; i < size_paginas; i++) {

		nodo_tabla_heap* pagina = (nodo_tabla_heap*) malloc(
				sizeof(nodo_tabla_heap));

		pagina = list_get(&tablaHeap, i);

		int32_t size_bloques = list_size(&pagina->lista_de_bloques);

		int32_t flag_init = 0;
		int32_t flag_end = -1;
		int32_t flag_before = -0;

		printf("%d\n", size_bloques);

		int j = 0;


		 for(j;j< size_bloques;j++ ){

		 puts("bloques");

		 t_bloque* bloque =(t_bloque*) malloc (sizeof (t_bloque));;

		 bloque= list_get(&pagina->lista_de_bloques, j);

		 puts('2');

		 if(bloque->isUsed == false){

		 //if()

		 }else{

		 }

		 }


		int flag = 0;
		int sum = 0;
		//int j=0;
		int flag_int = 0;

		while (flag < 2) {

			t_bloque* bloque = (t_bloque*) malloc(sizeof(t_bloque));

			bloque = list_get(&pagina->lista_de_bloques, j);

			if (bloque->isUsed == false) {
				flag = bloque->nro_bloque;
				flag_int = j;
				sum = sum + bloque->size;
				flag++;
			} else {

				flag = bloque->nro_bloque;
				sum = 0;
				flag = 0;
			}

			j++;

		}

		t_bloque* bloque_eliminar = (t_bloque*) malloc(sizeof(t_bloque));
		t_bloque* bloque_compactado = (t_bloque*) malloc(sizeof(t_bloque));

		bloque_eliminar = list_remove(pagina->lista_de_bloques, j);

		bloque_compactado = list_get(pagina->lista_de_bloques, j - 1);

		bloque_compactado->size = sum;

	}

	//printf(binary);

}

*/

//Settea en false la estructura de metadata de la pagina si todos los bloques estan en false

//Mejor nombre Actualizarmetadata_bloqyes_pagina_en_memoria()


/*
bool limpiar_pagina_en_memoria(nodo_tabla_heap* pagina) {

	if (pagina->espacioDisponible
			== kernel_config->pageSize - METADATA_BLOQUE_SIZE) {

		int32_t response;

		heap_metadata new_pagina_false;

		librocket_enviarMensaje(memoriaSocket, &pagina->numeroPagina,
				sizeof(int32_t), infoLogger);

		librocket_enviarMensaje(memoriaSocket, &pagina->espacioDisponible,
				sizeof(int32_t), infoLogger);

		librocket_recibirMensaje(memoriaSocket, &new_pagina_false,
				sizeof(heap_metadata), infoLogger);

		new_pagina_false.isFree = true;
		new_pagina_false.size = kernel_config->pageSize - METADATA_BLOQUE_SIZE;

		librocket_enviarMensaje(memoriaSocket, &new_pagina_false,
				sizeof(heap_metadata), infoLogger);

		librocket_recibirMensaje(memoriaSocket, &response, sizeof(int32_t),
				infoLogger);

		if (response) {

			char * text_log = string_new();
			string_append(&text_log,
					"No se pudo liberar la memoria de la pagina: ");
			string_append(&text_log, pagina->PID);
			log_error(infoLogger, &text_log);
			return false;

		}

		return true;

	} else {

		char * text_log = string_new();
		string_append(&text_log, "No fue necesario liberar la pagina: ");
		string_append(&text_log, pagina->PID);
		log_error(infoLogger, &text_log);
		return true;
	}

}

*/

//Calcula el offset sumando todos los tamaños


/*
int calcular_offset(int pagina, int bytes) {
//TODO Revisar anda mal.

	int i;
	int j;

	int offset = -1;

	int size = list_size(tablaHeap);

	for (i = 0; i < size; i++) {

		nodo_tabla_heap* nodo_tabla = list_get(tablaHeap, i);

		if (nodo_tabla->numeroPagina == pagina) {

			int size_bloques = list_size(nodo_tabla->lista_de_bloques);

			for (j = 0; j < size_bloques; j++) {

				t_bloque* bloque_libre = list_get(nodo_tabla->lista_de_bloques,
						j);

				if (bloque_libre->isUsed == false
						&& bloque_libre->size < bytes) {

					//	offset = sumar_bloques_restantes(i,j)+ METADATA_BLOQUE_SIZE;

					//TODO Que pasa si lo que me pide es menor al bloque libre que tengo en el que entra.
					return bloque_libre->offset;

				}

			}

			if (offset == -1) {

				t_bloque* bloque_nuevo;

				t_bloque* ultimo_bloque = buscar_ultimo_bloque(
						nodo_tabla->lista_de_bloques);

				bloque_nuevo->isUsed = true;
				bloque_nuevo->size = bytes;
				bloque_nuevo->nro_bloque = ultimo_bloque->nro_bloque + 1;
				bloque_nuevo->offset = ultimo_bloque->offset
						+ ultimo_bloque->size + METADATA_BLOQUE_SIZE;

				//TODO ver si corresponder agregar aca el bloque nuevo

				list_add(nodo_tabla->lista_de_bloques, bloque_nuevo);

				return bloque_nuevo->offset;
			}
		}

	}

	return -1;
}

*/

//Suma los tamaños de bloques de la pagina dada hasta el nro de bloque limite


/*
int sumar_bloques_restantes(int nro_pagina, int limite_bloques) {

	nodo_tabla_heap* pagina = list_get(tablaHeap, nro_pagina);

	int suma = 0;

	int i;

	for (i = 0; i < limite_bloques; i++) {

		t_bloque* bloque = list_get(pagina->lista_de_bloques, i);

		if (bloque->nro_bloque < limite_bloques) {

			suma = suma + bloque->size;
		}

	}

	suma = suma + METADATA_BLOQUE_SIZE * i;

	return suma;
}
*/

//Busca el ultimo bloque de acuerdo al nro.

/*

t_bloque* buscar_ultimo_bloque(t_list* lista_bloques) {

	int blok = 0;
	int i;

	t_bloque * bloque_retorno;

	int size = list_size(lista_bloques);

	if (size == 1) {

		return (t_bloque*) list_get(lista_bloques, 0);

	} else {

		for (i = 0; i < size; i++) {

			t_bloque* ultimo_bloque = list_get(lista_bloques, i);
			if (ultimo_bloque->nro_bloque > blok) {
				blok = ultimo_bloque->nro_bloque;
				bloque_retorno = ultimo_bloque;
			}
		}

	}

	return bloque_retorno;
}
*/


void pedido_bloque_memoria(int32_t socketCPU) {

	int32_t bytesPedidos;
	int32_t pid;

	librocket_recibirMensaje(socketCPU, &pid, sizeof(int32_t), infoLogger);

	librocket_recibirMensaje(socketCPU, &bytesPedidos, sizeof(int32_t),
			infoLogger);

	if (bytesPedidos + (METADATA_BLOQUE_SIZE * 2) > kernel_config->pageSize) {

		// se deniega la solicitud por ser mas grande que el tamanio de una pagina
		//se debe terminar el proceso en ejecucion, setear su EC y mover a la cola de finalizados
		CPU* cpu = buscarCPUBySocket(socketCPU);

		t_pcb* pcb = buscarPcbPorPID(cpu->PID_en_ejecucion);

		//TODO aca deberia recibir el pcb actualizado ?? preguntar

		pcb->EC = MALLOC_MAYOR_A_PAGESIZE;
		pasarPcbDeEjecutandoAFinalizados(&pcb);
		PROTOCOLO_KERNEL_A_CPU rechazado = PEDIDO_MEMORIA_RECHAZADO;
		librocket_enviarMensaje(socketCPU, &rechazado,
				sizeof(PROTOCOLO_KERNEL_A_CPU), infoLogger);

		int32_t programaSocket = buscarProgramaSocketByPid(pcb->PID);
		PROTOCOLO_KERNEL_A_CONSOLA msjPRocesoRechazado = PROCESO_TERMINADO;
		librocket_enviarMensaje(programaSocket, &msjPRocesoRechazado,
				sizeof(PROTOCOLO_KERNEL_A_CONSOLA), infoLogger);
		librocket_enviarMensaje(programaSocket, &(pcb->EC), sizeof(int32_t),
				infoLogger);

	} else {

		asignar_paginas_heap(bytesPedidos,pid,socketCPU);

	}

}

void liberar_bloque(int32_t socketCPU) {


	int32_t pid;

	t_puntero puntero_recibido;

	librocket_recibirMensaje(socketCPU, &pid, sizeof(int32_t), infoLogger);

	librocket_recibirMensaje(socketCPU, &puntero_recibido,sizeof(t_puntero), infoLogger);

	liberar_bloque_heap(puntero_recibido, pid, socketCPU);


}








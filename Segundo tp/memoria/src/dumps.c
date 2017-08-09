#include "headers/dumps.h"
#include <stdio.h>
#include <math.h>

void estadisticas(t_estructurasMemoria* estructurasMemoria){
	int porcentajeColisiones =
			(estructurasMemoria->colisiones) * 100 / (estructurasMemoria->escrituras + estructurasMemoria->lecturas);
	int porcentajeCache =
			(estructurasMemoria->aciertosDeCache) * 100 / (estructurasMemoria->aciertosDeCache + estructurasMemoria->lecturas);

	printf("Estadisticas de memoria \n");
	printf("------------------------ \n");
	printf("Accesos totales a memoria: %d \n", estructurasMemoria->escrituras + estructurasMemoria->lecturas);
	printf("Lecturas totales a memoria: %d \n", estructurasMemoria->lecturas);
	printf("Escrituras totales a memoria: %d \n", estructurasMemoria->escrituras);
	printf("Accesos de lectura totales por cache: %d \n", estructurasMemoria->aciertosDeCache);
	printf("Porcentaje de aciertos de cache: %d \n", porcentajeCache);
	printf("Porcentaje de colisiones: %d \n", porcentajeColisiones);
}

void dumpTamanioMemoria(t_estructurasMemoria* estructurasMemoria, FILE* standardOut){
	if (standardOut){
		fprintf(standardOut, "DUMP DE TAMAÑO DE MEMORIA \n");
		fprintf(standardOut, "------------------------ \n");
	}else{
		printf("DUMP DE TAMAÑO DE MEMORIA \n");
		printf("------------------------ \n");
	}
	int i=0; int contVacios=0; int contReservados=0;
	for (;i<estructurasMemoria->memoria_config->marcos;i++){
		registroMarcoPagina* reg = (registroMarcoPagina*) list_get(estructurasMemoria->tablaPaginas, i);
		if (reg->pid == FRAME_VACIO) contVacios++;
		if (reg->pid == FRAME_RESERVADO) contReservados++;
	}
	int ocupados = estructurasMemoria->memoria_config->marcos - contReservados - contVacios;
	if (standardOut){
		fprintf(standardOut, "#FRAME_SIZE: %d \n", estructurasMemoria->memoria_config->marcoSize);
		fprintf(standardOut, "#CANTIDAD DE FRAMES        #OCUPADOS        LIBRES        RESERVADOS \n");
		fprintf(standardOut, "---------------------------------------------------------------------- \n");
		fprintf(standardOut, "%d          %s %d %s %d %s %d\n", estructurasMemoria->memoria_config->marcos, padding(estructurasMemoria->memoria_config->marcos),
				ocupados, padding(ocupados), contVacios, padding(contVacios), contReservados);
	}else{
		printf("#FRAME_SIZE: %d \n", estructurasMemoria->memoria_config->marcoSize);
		printf( "#CANTIDAD DE FRAMES        #OCUPADOS        LIBRES        RESERVADOS \n");
		printf( "---------------------------------------------------------------------- \n");
		printf( "%d          %s %d %s %d %s %d\n", estructurasMemoria->memoria_config->marcos, padding(estructurasMemoria->memoria_config->marcos),
				ocupados, padding(ocupados), contVacios, padding(contVacios), contReservados);
	}

}

void dumpContenido(t_estructurasMemoria* estructurasMemoria, int32_t pid, FILE* standardOut){

	if (standardOut){
		fprintf(standardOut, "DUMP DE CONTENIDO DE MEMORIA \n");
		fprintf(standardOut, "------------------------ \n");
	}else{
		printf("DUMP DE CONTENIDO DE MEMORIA \n");
		printf("------------------------ \n");
	}

	int i;
	int y;
	for (i=0;i<estructurasMemoria->memoria_config->marcos;i++){
		registroMarcoPagina* registro = (registroMarcoPagina*) list_get(estructurasMemoria->tablaPaginas, i);

		if ((pid==0 || registro->pid==pid) && registro->pid!=FRAME_VACIO){
			if (standardOut){
				fprintf(standardOut, "#FRAME        #PROCESS_ID        PAGINA_ID \n");
				fprintf(standardOut, "---------------------------------------------------------------------- \n");
				fprintf(standardOut, "%d %s %d %s %d \n", i, padding(i), registro->pid, padding(registro->pid), registro->paginaId);
				fprintf(standardOut, "---------------------------------------------------------------------- \n");
				registroTipoPagina* tipo = list_get(estructurasMemoria->tipoDePaginas, i);
				if (tipo->tipo == CODIGO){
					fprintf(standardOut, "#CONTENIDO (codigo)\n");
				}else if (tipo->tipo == HEAP){
					fprintf(standardOut, "#CONTENIDO (heap)\n");
				} else if(tipo->tipo == STACK) {
					fprintf(standardOut, "#CONTENIDO (stack)\n");
				}
				char* punteroAMemoria = estructurasMemoria->memoria + i * estructurasMemoria->memoria_config->marcoSize;
				if (tipo->tipo == CODIGO){
					for (y=0;y<estructurasMemoria->memoria_config->marcoSize;y++){
						fprintf(standardOut, "%c", *(punteroAMemoria + y));
					}
				}
				if (tipo->tipo == STACK){
					int* punteroMemoriaInt = (int*) punteroAMemoria;
					for (y=0;y*4<estructurasMemoria->memoria_config->marcoSize;y++){
						fprintf(standardOut, "|%d", *(punteroMemoriaInt + y));
					}
				}
				if (tipo->tipo == HEAP) {
					int lectorHeap = 0;
					int bloqueN = 0;
					while(lectorHeap < estructurasMemoria->memoria_config->marcoSize){
						heap_metadata_dump* bloque = (heap_metadata_dump*) (punteroAMemoria + lectorHeap);
						fprintf(standardOut, "Bloque %d: Tamanio: %d - libre: %c\n", bloqueN, bloque->size, bloque->isFree);
						int y;
						int* punteroInt = (int*)punteroAMemoria;
						for (y=0;y*4<bloque->size;y++){
							fprintf(standardOut, "|%d", *(punteroInt + y));
						}
						lectorHeap += bloque->size + 5;
						bloqueN++;
						fprintf(standardOut, "\n");
					}
				}
				printf("\n");

			}else{
				printf("#FRAME        #PROCESS_ID        PAGINA_ID \n");
				printf("---------------------------------------------------------------------- \n");
				printf("%d %s %d %s %d \n", i, padding(i), registro->pid, padding(registro->pid), registro->paginaId);
				printf("---------------------------------------------------------------------- \n");
				registroTipoPagina* tipo = list_get(estructurasMemoria->tipoDePaginas, i);
				if (tipo->tipo == CODIGO){
					printf("#CONTENIDO (codigo)\n");
				}else if (tipo->tipo == HEAP){
					printf("#CONTENIDO (heap)\n");
				} else if(tipo->tipo == STACK) {
					printf("#CONTENIDO (stack)\n");
				}
				char* punteroAMemoria = estructurasMemoria->memoria + i * estructurasMemoria->memoria_config->marcoSize;
				if (tipo->tipo == CODIGO){
					for (y=0;y<estructurasMemoria->memoria_config->marcoSize;y++){
						printf("%c", *(punteroAMemoria + y));
					}
				}
				if (tipo->tipo == STACK){
					int* punteroMemoriaInt = (int*) punteroAMemoria;
					for (y=0;y*4<estructurasMemoria->memoria_config->marcoSize;y++){
						printf("|%d", *(punteroMemoriaInt + y));
					}
				}
				//Desactivo por el momento porque las estructuras de heap no se graban bien
				if (tipo->tipo == HEAP) {
					int lectorHeap = 0;
					int bloqueN = 0;
					while(lectorHeap < estructurasMemoria->memoria_config->marcoSize){
						heap_metadata_dump* bloque = (heap_metadata_dump*) (punteroAMemoria + lectorHeap);
						printf("Bloque %d: Tamanio: %d - libre: %c\n", bloqueN, bloque->size, bloque->isFree);
						int y;
						int* punteroInt = (int*)punteroAMemoria+5;
						for (y=0;y*4<bloque->size;y++){
							printf("|%d", *(punteroInt + y));
						}
						lectorHeap += bloque->size+5;
						bloqueN++;
						printf("\n");
					}
				}
				printf("\n");
			}
		}
	}
}

void dumpTablaPaginas(t_estructurasMemoria* estructurasMemoria, int32_t pid, FILE* standardOut){

	if (standardOut){
		fprintf(standardOut, "DUMP DE TABLA DE PAGINAS \n");
		fprintf(standardOut, "------------------------ \n");
		fprintf(standardOut, "#FRAME        #PROCESS_ID        PAGINA_ID \n");
		fprintf(standardOut, "---------------------------------------------------------------------- \n");
	}else{
		printf("DUMP DE TABLA DE PAGINAS \n");
		printf("------------------------ \n");
		printf("#FRAME        #PROCESS_ID        PAGINA_ID \n");
		printf("---------------------------------------------------------------------- \n");
	}
	int i;
	for (i=0;i<estructurasMemoria->memoria_config->marcos;i++){
		registroMarcoPagina* registro = (registroMarcoPagina*) list_get(estructurasMemoria->tablaPaginas, i);

		if ((pid==0 || registro->pid==pid) && registro->pid!=FRAME_VACIO){
			if (standardOut){
				fprintf(standardOut, "%d %s %d %s %d \n", i, padding(i), registro->pid, padding(registro->pid), registro->paginaId);
			}else{
				printf("%d %s %d %s %d \n", i, padding(i), registro->pid, padding(registro->pid), registro->paginaId);
			}
		}
	}
}

void dumpPaginasPorProceso(t_estructurasMemoria* estructurasMemoria, int32_t pid, FILE* standardOut){
	t_list* listaPaginasPorProceso = list_create();

	int i;

	for (i=0;i<estructurasMemoria->memoria_config->marcos;i++){
		registroMarcoPagina* registroPagina = (registroMarcoPagina*) list_get(estructurasMemoria->tablaPaginas, i);
		if (registroPagina->pid == FRAME_VACIO || registroPagina->pid == FRAME_RESERVADO){
			continue;
		}
		registroCantidadPaginasPorProceso* busqueda = buscarPidEnRegistro(listaPaginasPorProceso, registroPagina->pid);
		if (busqueda){
			busqueda->cantidadPaginas++;
		}else{
			registroCantidadPaginasPorProceso* nuevoRegistro = malloc(sizeof(registroCantidadPaginasPorProceso));
			nuevoRegistro->cantidadPaginas = 1;
			nuevoRegistro->pid=registroPagina->pid;
			list_add(listaPaginasPorProceso, nuevoRegistro);
		}

	}
	if (pid==0){
		if (standardOut){
			fprintf(standardOut, "\n");
			fprintf(standardOut, "DUMP DE PAGINAS POR PROCESO \n");
			fprintf(standardOut, "------------------------ \n");
			fprintf(standardOut, "#PROCESS_ID       #CANTIDAD_DE_PAGINAS \n");
			fprintf(standardOut, "---------------------------------------------------------------------- \n");
		}else{
			printf("\n");
			printf("DUMP DE PAGINAS POR PROCESO \n");
			printf( "------------------------ \n");
			printf("#PROCESS_ID       #CANTIDAD_DE_PAGINAS \n");
			printf("---------------------------------------------------------------------- \n");
		}
	}

	for (i=0;i<listaPaginasPorProceso->elements_count;i++){
		registroCantidadPaginasPorProceso* registro = (registroCantidadPaginasPorProceso*) list_get(listaPaginasPorProceso, i);
		if (standardOut){
			if (pid==0)
				fprintf(standardOut, "%d %s %d \n", registro->pid, padding(registro->pid), registro->cantidadPaginas);
			else{
				if (registro->pid==pid){
					fprintf(standardOut, "TAMAÑO DEL PROCESO \n PID=%d \n %d bytes \n", registro->pid,
							registro->cantidadPaginas * estructurasMemoria->memoria_config->marcoSize);
				}
			}
		}else{
			if (pid==0)
				printf("%d %s %d \n", registro->pid, padding(registro->pid), registro->cantidadPaginas);
			else{
				if (registro->pid==pid){
					printf("TAMAÑO DEL PROCESO \n PID=%d \n %d bytes \n", registro->pid,
							registro->cantidadPaginas * estructurasMemoria->memoria_config->marcoSize);
				}
			}
		}
	}
	list_destroy_and_destroy_elements(listaPaginasPorProceso, free);
}

registroCantidadPaginasPorProceso* buscarPidEnRegistro(t_list* lista, int32_t pid){
	int i;
	for (i=0;i<lista->elements_count;i++){
		registroCantidadPaginasPorProceso* registro = (registroCantidadPaginasPorProceso*) list_get(lista, i);
		if (registro->pid == pid)
			return registro;
	}
	return NULL;
}

void dumpCache(t_estructurasMemoria* estructurasMemoria, FILE* standardOut){

	if (standardOut){
		fprintf(standardOut, "DUMP DE CACHE \n");
		fprintf(standardOut, "------------------------ \n");
	}else{
		printf("DUMP DE CACHE \n");
		printf( "------------------------ \n");
	}

	int32_t indice;
	for (indice=0;indice<estructurasMemoria->cache->elements_count;indice++){
		registroCacheMemoria* registro = (registroCacheMemoria*) list_get(estructurasMemoria->cache, indice);
		if (registro->pid == FRAME_VACIO)
			continue;
		if (standardOut){
			fprintf(standardOut, "#FRAME             #PROCESS_ID       #PAGINA_ID\n");
			fprintf(standardOut, "---------------------------------------------------------------------- \n");
			fprintf(standardOut, "%d %s %d %s %d \n", indice, padding(indice), registro->pid, padding(registro->pid), registro->paginaId);
			fprintf(standardOut, "---------------------------------------------------------------------- \n");
			fprintf(standardOut, "#CONTENIDO\n");
			int indice;
			for (indice=0;indice<estructurasMemoria->memoria_config->marcoSize;indice++){
				fprintf(standardOut, "%c", *(((char*)registro->pagina) + indice));
			}
			fprintf(standardOut, "\n");
		}else{
			printf("#FRAME             #PROCESS_ID       #PAGINA_ID\n");
			printf("---------------------------------------------------------------------- \n");
			printf("%d %s %d %s %d \n", indice, padding(indice), registro->pid, padding(registro->pid), registro->paginaId);
			printf("---------------------------------------------------------------------- \n");
			printf("#CONTENIDO\n");
			int indice;
			for (indice=0;indice<estructurasMemoria->memoria_config->marcoSize;indice++){
				printf("%c", *(((char*)registro->pagina) + indice));
			}
			printf("\n");
		}

	}
}

void cambiarRetardo(t_estructurasMemoria* estructurasMemoria, int32_t nuevoRetardo){
	estructurasMemoria->memoria_config->retardoMemoria = nuevoRetardo;
}

char* padding(int32_t numero){

	char* padding1 = "               ";
	char* padding2 = "              ";
	char* padding3 = "             ";
	char* padding4 = "            ";
	char* padding5 = "           ";
	char* padding6 = "          ";
	char* padding7 = "         ";
	char* padding8 = "        ";
	char* padding9 = "       ";
	char* padding10 = "      ";

	int digitos = 0;
	while (numero != 0){
		numero = numero / 10;
		digitos++;
	}

	switch(digitos) {

	   case 1  :
		  return padding1;
	      break;
	   case 2  :
		  return padding2;
	      break;
	   case 3  :
		  return padding3;
	      break;
	   case 4  :
		  return padding4;
	      break;
	   case 5  :
		  return padding5;
	      break;
	   case 6  :
		  return padding6;
	      break;
	   case 7  :
		  return padding7;
	      break;
	   case 8  :
		  return padding8;
	      break;
	   case 9  :
		  return padding9;
	      break;
	   case 10  :
		  return padding10;
	      break;
	   default:
		   return padding1;
	}
}


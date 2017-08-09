#include "headers/estructurasAdministrativas.h"
#include <structs/structs.h>

int32_t funcionHash(t_estructurasMemoria* estructurasMemoria, int32_t pid, int32_t pagina){
	//Por decir algo ! pueden ser mas o menos
	int cantidadProcesosPromedio = 18;
	int cantidadDePaginas = estructurasMemoria->memoria_config->marcos;
	//4% de la memoria dedicada a las estructuras administrativas
	int paginasReservadas = estructurasMemoria->memoria_config->marcos / 100 * 4;
	int coeficientePaginasPorProceso = cantidadDePaginas / cantidadProcesosPromedio;
	return paginasReservadas + pid * coeficientePaginasPorProceso + pagina;

}

int32_t obtenerPaginasAsignadasPorProceso(t_estructurasMemoria* estructurasMemoria, int32_t pid){
	int indice;
	int cant=0;
	for (indice=0;indice<estructurasMemoria->memoria_config->marcos;indice++){
		if (((registroMarcoPagina*)list_get(estructurasMemoria->tablaPaginas, indice))->pid == pid)
			cant++;
	}
	return cant;
}

PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU escribirEnTablaDeFrame(t_estructurasMemoria* estructurasMemoria, int32_t pid, int32_t pagina, int vaciar, TIPO_DE_PAGINA tipo){

	int32_t indiceInicial = funcionHash(estructurasMemoria, pid, pagina);
	if (indiceInicial >= estructurasMemoria->memoria_config->marcos){
		indiceInicial=estructurasMemoria->memoria_config->marcos-1;
	}
	if (indiceInicial < 0){
		indiceInicial=0;
	}
	int32_t indice = indiceInicial;
	do{
		registroMarcoPagina* registro = (registroMarcoPagina*)list_get(estructurasMemoria->tablaPaginas, indice);
		if (!vaciar){
			if (registro->pid == FRAME_VACIO){
				registro->paginaId = pagina;
				registro->pid = pid;
				registroTipoPagina* estruct = malloc(sizeof(registroTipoPagina));
				estruct->tipo = tipo;
				list_remove_and_destroy_element(estructurasMemoria->tipoDePaginas, indice, free);
				list_add_in_index(estructurasMemoria->tipoDePaginas, indice, estruct);
				return PAGINAS_ASIGNADAS;
			}
		}else{
			if (registro->pid==pid && registro->paginaId==pagina){
				registro->pid = FRAME_VACIO;
				list_remove_and_destroy_element(estructurasMemoria->tipoDePaginas, indice, free);
				registroTipoPagina* estruct = malloc(sizeof(registroTipoPagina));
				estruct->tipo = VACIO;
				list_add_in_index(estructurasMemoria->tipoDePaginas, indice, estruct);
				return PAGINAS_ASIGNADAS;
			}
		}

		indice++;
		if (indice == estructurasMemoria->memoria_config->marcos)
			indice=0;
	} while (indice!=indiceInicial);
	return MEMORIA_INSUFICIENTE;
}

PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU eliminarPaginasDeProceso(t_estructurasMemoria* estructurasMemoria, int32_t pid){
	int indice;
	for (indice=0;indice<estructurasMemoria->memoria_config->marcos;indice++){
		registroMarcoPagina* registro = (registroMarcoPagina*)list_get(estructurasMemoria->tablaPaginas, indice);
		if (registro->pid == pid){
			registro->pid = FRAME_VACIO;
			list_remove_and_destroy_element(estructurasMemoria->tipoDePaginas, indice, free);
			registroTipoPagina* estruct = malloc(sizeof(registroTipoPagina));
			estruct->tipo = VACIO;
			list_add_in_index(estructurasMemoria->tipoDePaginas, indice, estruct);
		}
	}
	return ELIMINACION_DE_PROCESO_SATISFACTORIO;
}

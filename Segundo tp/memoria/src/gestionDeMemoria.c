#include "headers/gestionDeMemoria.h"
#include "headers/estructurasAdministrativas.h"
#include <unistd.h>

PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU leerDatoInterna(t_estructurasMemoria* estructurasMemoria, t_direccion_logica* puntero, void** punteroADato, TIPO_DE_DATOS tipo){

	pthread_mutex_lock(&estructurasMemoria->mutex_cache);
	registroCacheMemoria* paginaCacheada = leerCache(estructurasMemoria, puntero->pid, puntero->paginaId);
	if (paginaCacheada != NULL){
		estructurasMemoria->aciertosDeCache++;
		memcpy(*punteroADato, paginaCacheada->pagina + puntero->offset, puntero->tamanio);
		pthread_mutex_unlock(&estructurasMemoria->mutex_cache);
		return LECTURA_SATISFACTORIA;
	}else{
		void* pagina = leerPaginaDeMemoria(estructurasMemoria, puntero);
		if (pagina!=NULL){
			estructurasMemoria->lecturas++;
			registroCacheMemoria* registro= escribirCache(estructurasMemoria, puntero->pid, puntero->paginaId, pagina);
			char* lectura = (char*) (registro->pagina + puntero->offset);
			memcpy(*punteroADato, lectura, puntero->tamanio);
			pthread_mutex_unlock(&estructurasMemoria->mutex_cache);
			return LECTURA_SATISFACTORIA;
		}else{
			pthread_mutex_unlock(&estructurasMemoria->mutex_cache);
			log_info(estructurasMemoria->logger, "[MEMORIA-DEBUG] Error de lectura en pid: %d pagina: %d offset: %d",
					puntero->pid, puntero->paginaId, puntero->offset);
			return ERROR_EN_LECTURA;
		}
	}
}

PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU escribirDatoInterna(t_estructurasMemoria* estructurasMemoria, t_direccion_logica* puntero, void* punteroAEscribir, TIPO_DE_DATOS tipo){

	int marcoSize = estructurasMemoria->memoria_config->marcoSize;
	int32_t frame = buscarIndiceEnPaginaDeFrames(estructurasMemoria, puntero);
	if (frame!=-1){
		estructurasMemoria->escrituras++;
		usleep(estructurasMemoria->memoria_config->retardoMemoria * 1000);
		pthread_mutex_lock(&estructurasMemoria->mutex_cache);
		registroCacheMemoria* paginaCacheada = leerCache(estructurasMemoria, puntero->pid, puntero->paginaId);
		if (tipo == INT){
			int* punteroAMemoria = estructurasMemoria->memoria + frame * marcoSize + puntero->offset;
			int* punteroAEscribirInt = (int*) punteroAEscribir;
			*punteroAMemoria = *punteroAEscribirInt;
			if (paginaCacheada != NULL){
				int* cacheAEscribir = paginaCacheada->pagina + puntero->offset;
				*cacheAEscribir = *punteroAEscribirInt;
			}
		} else if (tipo == METADATA) {
			//No se le da un tratamiento especial a su escritura
		} else{
			void* punteroAPagina = estructurasMemoria->memoria + frame * marcoSize;
			memcpy(punteroAPagina + puntero->offset, punteroAEscribir, puntero->tamanio);
			if (paginaCacheada != NULL){
				void* paginaCacheadaAEscribir = paginaCacheada->pagina;
				memcpy(paginaCacheadaAEscribir + puntero->offset, punteroAEscribir, puntero->tamanio);
			}
		}

		pthread_mutex_unlock(&estructurasMemoria->mutex_cache);
		return ESCRITURA_SATISFACTORIA;
	}else{
		log_info(estructurasMemoria->logger, "[MEMORIA-DEBUG] Error de lectura en pid: %d pagina: %d offset: %d",
				puntero->pid, puntero->paginaId, puntero->offset);
		return ERROR_EN_ESCRITURA;
	}
}

registroCacheMemoria* leerCache(t_estructurasMemoria* estructurasMemoria, int32_t pid, int32_t pagina){
	int32_t indice;
	for (indice=0;indice<estructurasMemoria->cache->elements_count;indice++){
		registroCacheMemoria* registro = (registroCacheMemoria*) list_get(estructurasMemoria->cache, indice);
		if (registro->paginaId == pagina && registro->pid == pid){
			//ACIERTO DE CACHE
			registroCacheMemoria* elegido = (registroCacheMemoria*) list_remove(estructurasMemoria->cache, indice);
			list_add_in_index(estructurasMemoria->cache, 0, elegido);
			return elegido;
		}
	}
	return NULL;
}

void flushCache(t_estructurasMemoria* estructurasMemoria){
	log_info(estructurasMemoria->logger, "[MEMORIA-INFO] Pedido de flush de cache");
	pthread_mutex_lock(&estructurasMemoria->mutex_cache);
	while (estructurasMemoria->cache->elements_count > 0)
		list_remove_and_destroy_element(estructurasMemoria->cache, 0, destructorCache);
	registroAuxCache* framesVacios = list_get(estructurasMemoria->contadorAuxCache, 0);
	framesVacios->cantidadPaginasCacheadas = estructurasMemoria->memoria_config->entradasCache;
	pthread_mutex_unlock(&estructurasMemoria->mutex_cache);
}

void limpiarCachePorPid(t_estructurasMemoria* estructurasMemoria, int pid){
	log_info(estructurasMemoria->logger, "[MEMORIA-INFO] Eliminando entradas de la cache para el pid %d", pid);
	pthread_mutex_lock(&estructurasMemoria->mutex_cache);
	int i;
	int total = estructurasMemoria->cache->elements_count;
	for (i = 0; i<total;i++){
		registroCacheMemoria* registro = (registroCacheMemoria*)list_get(estructurasMemoria->cache, i);
		if (registro->pid == pid){
			list_remove_and_destroy_element(estructurasMemoria->cache, i, destructorCache);
			i--;total--;
		}
	}
	eliminarRegistroAux(estructurasMemoria, pid);
	pthread_mutex_unlock(&estructurasMemoria->mutex_cache);
}

registroCacheMemoria* escribirCache(t_estructurasMemoria* estructurasMemoria, int32_t pid, int32_t paginaId, void* pagina){

	eliminarRegistroCacheLRU(estructurasMemoria, pid);
	registroCacheMemoria* registro = malloc(sizeof(registroCacheMemoria));
	registro->paginaId=paginaId;
	registro->pid=pid;
	registro->pagina=malloc(estructurasMemoria->memoria_config->marcoSize);
	memcpy(registro->pagina, pagina, estructurasMemoria->memoria_config->marcoSize);
	list_add_in_index(estructurasMemoria->cache, 0, registro);
	return registro;
}
void destructorCache(void* elemento){
	registroCacheMemoria* registro = (registroCacheMemoria*) elemento;
	free(registro->pagina);
	free(registro);
}

void destructorAuxCache(void* elemento){
	registroAuxCache* registro = (registroAuxCache*) elemento;
	free(registro);
}

void* leerPaginaDeMemoria(t_estructurasMemoria* estructurasMemoria, t_direccion_logica* puntero){

	int32_t indice = buscarIndiceEnPaginaDeFrames(estructurasMemoria, puntero);
	if (indice!=-1){
		usleep(estructurasMemoria->memoria_config->retardoMemoria * 1000);
		return estructurasMemoria->memoria + indice * estructurasMemoria->memoria_config->marcoSize;
	}else{
		return NULL;
	}

}

int32_t buscarIndiceEnPaginaDeFrames(t_estructurasMemoria* estructurasMemoria, t_direccion_logica* puntero){
	int frameAprox = funcionHash(estructurasMemoria, puntero->pid, puntero->paginaId);
	if (frameAprox>=estructurasMemoria->memoria_config->marcos){
		frameAprox = estructurasMemoria->memoria_config->marcos-1;
	}
	int i;
	for (i=0;i<estructurasMemoria->memoria_config->marcos; i++){
		if (i+frameAprox==estructurasMemoria->memoria_config->marcos)
			frameAprox-=estructurasMemoria->memoria_config->marcos;
		registroMarcoPagina* registro = list_get(estructurasMemoria->tablaPaginas, i+frameAprox);
		if (registro->paginaId==puntero->paginaId && registro->pid==puntero->pid){
			if (i!=0)
				estructurasMemoria->colisiones++;
			return (i+frameAprox);
		}
	}
	return -1;
}

void eliminarRegistroCacheLRU(t_estructurasMemoria* estructurasMemoria, int32_t pid){

	registroAuxCache* framesVacios = list_get(estructurasMemoria->contadorAuxCache, 0);
	if (framesVacios->cantidadPaginasCacheadas){
		registroAuxCache* registroAux = obtenerRegistroAux(estructurasMemoria, pid);
		if (registroAux){
			if (registroAux->cantidadPaginasCacheadas == estructurasMemoria->memoria_config->cacheXProc){
				desalojarLocalmente(estructurasMemoria, pid);
			}else{
				registroAux->cantidadPaginasCacheadas++;
			}
		}else{
			crearNuevoRegistroCacheAux(estructurasMemoria, pid);
		}
		framesVacios->ultimaPaginaCacheada++;
		framesVacios->cantidadPaginasCacheadas--;
	} else {
		registroAuxCache* registroAux = obtenerRegistroAux(estructurasMemoria, pid);
		if (registroAux){
			if (registroAux->cantidadPaginasCacheadas == estructurasMemoria->memoria_config->cacheXProc){
				desalojarLocalmente(estructurasMemoria, pid);
			}else{
				registroAux->cantidadPaginasCacheadas++;
				desalojarGlobalmente(estructurasMemoria);
			}
		}else{
			crearNuevoRegistroCacheAux(estructurasMemoria, pid);
			desalojarGlobalmente(estructurasMemoria);
		}
	}
}

void desalojarGlobalmente(t_estructurasMemoria* estructurasMemoria){
	registroCacheMemoria* aEliminar=(registroCacheMemoria*)list_get(estructurasMemoria->cache, list_size(estructurasMemoria->cache)-1);
	int32_t y;
	for (y=1;y<list_size(estructurasMemoria->contadorAuxCache);y++){
		registroAuxCache* registroAux = (registroAuxCache*)list_get(estructurasMemoria->contadorAuxCache, y);
		if (registroAux->pid == aEliminar->pid){
			registroAux->cantidadPaginasCacheadas--;
		}
	}
	list_remove_and_destroy_element(estructurasMemoria->cache, list_size(estructurasMemoria->cache)-1, destructorCache);
}

void desalojarLocalmente(t_estructurasMemoria* estructurasMemoria, int32_t pid){
	int32_t indice=0;
	int32_t contador=0;
	for (indice=0;indice<estructurasMemoria->memoria_config->entradasCache;indice++){
		registroCacheMemoria* registroCache = (registroCacheMemoria*) list_get(estructurasMemoria->cache, indice);
		if (registroCache->pid == pid){
			contador++;
		}
		if (contador == estructurasMemoria->memoria_config->cacheXProc){
			break;
		}
	}
	list_remove_and_destroy_element(estructurasMemoria->cache, indice, destructorCache);
}

void crearNuevoRegistroCacheAux(t_estructurasMemoria* estructurasMemoria, int32_t pid){
	registroAuxCache* nuevoRegistro= malloc(sizeof(registroAuxCache));
	nuevoRegistro->pid=pid;
	nuevoRegistro->cantidadPaginasCacheadas=1;
	list_add(estructurasMemoria->contadorAuxCache, nuevoRegistro);
}

registroAuxCache* obtenerRegistroAux(t_estructurasMemoria* estructurasMemoria, int32_t pid){
	int32_t i;
	for (i=1;i<list_size(estructurasMemoria->contadorAuxCache);i++){
		registroAuxCache* registro = (registroAuxCache*)list_get(estructurasMemoria->contadorAuxCache, i);
		if (registro->pid == pid){
			return registro;
		}
	}
	return NULL;
}

void eliminarRegistroAux(t_estructurasMemoria* estructurasMemoria, int32_t pid){
	int32_t i;
	for (i=1;i<list_size(estructurasMemoria->contadorAuxCache);i++){
		registroAuxCache* registro = (registroAuxCache*)list_get(estructurasMemoria->contadorAuxCache, i);
		if (registro->pid == pid){
			list_remove_and_destroy_element(estructurasMemoria->contadorAuxCache, i, destructorAuxCache);
			break;
		}
	}
}

PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU asignarPaginasDeHeap(t_estructurasMemoria* estructurasMemoria, int32_t pid, int32_t paginasHeap){
	log_info(estructurasMemoria->logger, "[MEMORIA-INFO] Pedido de asignacion de paginas de HEAP de proceso pid=%d paginas=%d", pid, paginasHeap);
	int32_t paginasAsignadasHastaAhora = obtenerPaginasAsignadasPorProceso(estructurasMemoria, pid);
	int i,y;
	for (i=0;i<paginasHeap;i++){
		PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU respuesta=escribirEnTablaDeFrame(estructurasMemoria, pid, paginasAsignadasHastaAhora+i, 0, HEAP);
		if (respuesta == MEMORIA_INSUFICIENTE){
			for(y=paginasAsignadasHastaAhora;y<paginasAsignadasHastaAhora+i;y++){
				escribirEnTablaDeFrame(estructurasMemoria, pid, y, 1, VACIO);
			}
			log_info(estructurasMemoria->logger, "[MEMORIA-INFO] Paginas de HEAP insuficientes. pid=%d paginas=%d",
					pid, paginasHeap);
			return MEMORIA_INSUFICIENTE;
		}
	}
	return PAGINAS_ASIGNADAS;
}

PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU crearPaginasIniciales(t_estructurasMemoria* estructurasMemoria, int32_t pid, int32_t paginasFijas){
	log_info(estructurasMemoria->logger, "[MEMORIA-INFO] Pedido de inicializacion de proceso pid=%d paginas=%d", pid, paginasFijas);
	int32_t i;
	TIPO_DE_PAGINA tipoAsignado = CODIGO;
	for (i=0;i<paginasFijas;i++){
		if (i == (paginasFijas - estructurasMemoria->memoria_config->stackSize))
			tipoAsignado = STACK;
		PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU respuesta = escribirEnTablaDeFrame(estructurasMemoria, pid, i, 0, tipoAsignado);
		if (respuesta  == MEMORIA_INSUFICIENTE){
			eliminarPaginasDeProceso(estructurasMemoria, pid);
			log_info(estructurasMemoria->logger, "[MEMORIA-INFO] Paginas iniciales insifucientes. pid=%d paginas=%d",
					pid, paginasFijas);
			return MEMORIA_INSUFICIENTE;
		}
	}
	return PAGINAS_ASIGNADAS;
}

PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU leerDato(t_estructurasMemoria* estructurasMemoria, void** punteroADevolver, t_direccion_logica* puntero, TIPO_DE_DATOS tipo){
	if (puntero->offset < 0 || puntero->offset > estructurasMemoria->memoria_config->marcoSize)
		return ERROR_EN_LECTURA;
	log_info(estructurasMemoria->logger, "[MEMORIA-INFO] Pedido de lectura de pid=%d pagina=%d offset=%d tamanio=%d", puntero->pid, puntero->paginaId, puntero->offset, puntero->tamanio);
	return leerDatoInterna(estructurasMemoria, puntero, punteroADevolver, tipo);
}

PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU escribirDato(t_estructurasMemoria* estructurasMemoria, void* punteroAEscribir, t_direccion_logica* puntero, TIPO_DE_DATOS tipo){
	if (puntero->offset < 0 || puntero->offset > estructurasMemoria->memoria_config->marcoSize)
		return ERROR_EN_LECTURA;
	log_info(estructurasMemoria->logger, "[MEMORIA-INFO] Pedido de escritura de pid=%d pagina=%d offset=%d tamanio=%d", puntero->pid, puntero->paginaId, puntero->offset, puntero->tamanio);
	return escribirDatoInterna(estructurasMemoria, puntero, punteroAEscribir, tipo);
}

PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU finalizarProgramaEnMemoria(t_estructurasMemoria* estructurasMemoria, int32_t pid){
	log_info(estructurasMemoria->logger, "[MEMORIA-INFO] Pedido de finalizacion de proceso pid=%d", pid);
	limpiarCachePorPid(estructurasMemoria, pid);
	return eliminarPaginasDeProceso(estructurasMemoria, pid);
}

PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU liberarPagina(t_estructurasMemoria* estructurasMemoria, int32_t pid, int32_t paginaId){
	log_info(estructurasMemoria->logger, "[MEMORIA-INFO] Pedido de liberacion de pagina de proceso pid=%d pagina=%d", pid, paginaId);
	PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU resultado = escribirEnTablaDeFrame(estructurasMemoria, pid, paginaId, 1, VACIO);
	if (resultado == PAGINAS_ASIGNADAS){
		return PAGINA_LIBERADA;
	}else{
		return PAGINA_NO_LIBERADA;
	}
}

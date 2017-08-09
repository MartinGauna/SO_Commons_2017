#include <connections/networkHandler.h>
#include <commons/collections/list.h>
#include <structs/structs.h>
#include <protocolos/protocols.h>

#ifndef HEADERS_GESTIONDEMEMORIA_H_
#define HEADERS_GESTIONDEMEMORIA_H_

PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU crearPaginasIniciales(t_estructurasMemoria* estructurasMemoria, int32_t pid, int32_t paginasFijas);
PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU leerDato(t_estructurasMemoria* estructurasMemoria, void** punteroADevolver, t_direccion_logica* puntero, TIPO_DE_DATOS tipo);
PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU escribirDato(t_estructurasMemoria* estructurasMemoria, void* punteroAEscribir, t_direccion_logica* puntero, TIPO_DE_DATOS tipo);
PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU asignarPaginasDeHeap(t_estructurasMemoria* estructurasMemoria, int32_t pid, int32_t paginasHeap);
PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU finalizarProgramaEnMemoria(t_estructurasMemoria* estructurasMemoria, int32_t pid);
PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU liberarPagina(t_estructurasMemoria* estructurasMemoria, int32_t pid, int32_t paginaId);
registroCacheMemoria* escribirCache(t_estructurasMemoria* estructurasMemoria, int32_t pid, int32_t paginaId, void* pagina);
registroCacheMemoria* leerCache(t_estructurasMemoria* estructurasMemoria, int32_t pid, int32_t pagina);
void eliminarRegistroCacheLRU(t_estructurasMemoria* estructurasMemoria, int32_t pid);
void* leerPaginaDeMemoria(t_estructurasMemoria* estructurasMemoria, t_direccion_logica* puntero);
void flushCache(t_estructurasMemoria* estructurasMemoria);
void destructorCache(void* elemento);
void destructorAuxCache(void* elemento);
void eliminarRegistroAux(t_estructurasMemoria* estructurasMemoria, int32_t pid);
int32_t buscarIndiceEnPaginaDeFrames(t_estructurasMemoria* estructurasMemoria, t_direccion_logica* puntero);
void desalojarGlobalmente(t_estructurasMemoria* estructurasMemoria);
void desalojarLocalmente(t_estructurasMemoria* estructurasMemoria, int32_t pid);
void crearNuevoRegistroCacheAux(t_estructurasMemoria* estructurasMemoria, int32_t pid);
registroAuxCache* obtenerRegistroAux(t_estructurasMemoria* estructurasMemoria, int32_t pid);



#endif /* HEADERS_GESTIONDEMEMORIA_H_ */

#include <stdio.h>
#include <structs/structs.h>
#include <math.h>

#ifndef HEADERS_DUMPS_H_
#define HEADERS_DUMPS_H_

typedef char t_bool;

typedef struct {
	int32_t size;
	t_bool isFree;
} heap_metadata_dump;

void dumpTablaPaginas(t_estructurasMemoria* estructurasMemoria, int32_t pid, FILE* standardOut);
void dumpPaginasPorProceso(t_estructurasMemoria* estructurasMemoria, int32_t pid, FILE* standardOut);
registroCantidadPaginasPorProceso* buscarPidEnRegistro(t_list* lista, int32_t pid);
void dumpCache(t_estructurasMemoria* estructurasMemoria, FILE* standardOut);
void cambiarRetardo(t_estructurasMemoria* estructurasMemoria, int32_t nuevoRetardo);
char* padding(int32_t numero);
void dumpContenido(t_estructurasMemoria* estructurasMemoria, int32_t pid, FILE* standardOut);
void dumpTamanioMemoria(t_estructurasMemoria* estructurasMemoria, FILE* standardOut);
void estadisticas(t_estructurasMemoria* estructurasMemoria);

#endif /* HEADERS_DUMPS_H_ */

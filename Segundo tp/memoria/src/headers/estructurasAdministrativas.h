#include <connections/networkHandler.h>
#include <protocolos/protocols.h>
#include <structs/structs.h>

#ifndef HEADERS_ESTRUCTURASADMINISTRATIVAS_H_
#define HEADERS_ESTRUCTURASADMINISTRATIVAS_H_

int32_t funcionHash(t_estructurasMemoria* estructurasMemoria, int32_t pid, int32_t pagina);
PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU escribirEnTablaDeFrame(t_estructurasMemoria* estructurasMemoria, int32_t pid, int32_t pagina, int vaciar, TIPO_DE_PAGINA tipo);
int32_t obtenerPaginasAsignadasPorProceso(t_estructurasMemoria* estructurasMemoria, int32_t pid);
PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU eliminarPaginasDeProceso(t_estructurasMemoria* estructurasMemoria, int32_t pid);
void destructorRegistroMarcoPagina(void* registro);

#endif /* HEADERS_ESTRUCTURASADMINISTRATIVAS_H_ */

#include <structs/structs.h>
#include <protocolos/protocols.h>

#ifndef TESTS_HEADERS_TESTDUMPS_H_
#define TESTS_HEADERS_TESTDUMPS_H_

void testDumpTablaPaginas(t_estructurasMemoria* estructurasMemoria);
void testDumpTablaCantidadPaginasPorProceso(t_estructurasMemoria* estructurasMemoria);
void testDumpMemoria(t_estructurasMemoria* estructurasMemoria);
void crearMemoriaDummy(t_estructurasMemoria* estructurasMemoria);
void testDumpCache(t_estructurasMemoria* estructurasMemoria);
void testConsola(t_estructurasMemoria* estructurasMemoria);

#endif /* TESTS_HEADERS_TESTDUMPS_H_ */

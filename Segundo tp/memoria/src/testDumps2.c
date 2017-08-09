#include "headers/testDumps.h"
#include <structs/structs.h>
#include <protocolos/protocols.h>
#include <commons/collections/list.h>
#include "headers/dumps.h"

void testDumpCache(t_estructurasMemoria* estructurasMemoria){
	crearMemoriaDummy(estructurasMemoria);
	char* punteroAEnviar;
	t_direccion_logica* direccionLogica = malloc(sizeof(t_direccion_logica));
	direccionLogica->pid=2;
	direccionLogica->paginaId=7;
	direccionLogica->offset = 5;
	direccionLogica->tamanio = 1;

	leerDato(estructurasMemoria, &punteroAEnviar, direccionLogica);

	direccionLogica->pid=2;
	direccionLogica->paginaId=2;
	direccionLogica->offset = 5;
	direccionLogica->tamanio = 1;

	leerDato(estructurasMemoria, &punteroAEnviar, direccionLogica);

	direccionLogica->pid=3;
	direccionLogica->paginaId=5;
	direccionLogica->offset = 5;
	direccionLogica->tamanio = 1;

	leerDato(estructurasMemoria, &punteroAEnviar, direccionLogica);

	dumpCache(estructurasMemoria, NULL);

}

void crearMemoriaDummy(t_estructurasMemoria* estructurasMemoria){

	int i;
	char contenido='A';int32_t pidAux=1, paginaIdAux=0;
	for (i=0;i<estructurasMemoria->memoria_config->marcos;i++){
		registroMarcoPagina* registro = list_get(estructurasMemoria->tablaPaginas, i);
		if (registro->pid != FRAME_RESERVADO){
			registro->pid=pidAux;
			registro->paginaId=paginaIdAux;
			int y;
			for (y=0;y<estructurasMemoria->memoria_config->marcoSize;y++){
				memcpy(estructurasMemoria->memoria + estructurasMemoria->memoria_config->marcoSize * i + y, (const void*)&contenido, 1);
			}
			contenido++;paginaIdAux++;
			if (paginaIdAux==10){
				paginaIdAux=1;
				pidAux++;
			}
			if (contenido-1=='Z')
				contenido='A';
		}
	}
}


void testDumpMemoria(t_estructurasMemoria* estructurasMemoria){

	crearMemoriaDummy(estructurasMemoria);
	dumpContenido(estructurasMemoria, 0, NULL);

}
void testDumpTablaCantidadPaginasPorProceso(t_estructurasMemoria* estructurasMemoria){

	crearMemoriaDummy(estructurasMemoria);

	dumpPaginasPorProceso(estructurasMemoria, 0, NULL);

}

void testDumpTablaPaginas(t_estructurasMemoria* estructurasMemoria){

	crearMemoriaDummy(estructurasMemoria);

	dumpTablaPaginas(estructurasMemoria, 0, NULL);

}

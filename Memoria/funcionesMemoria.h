
#ifndef TP_2017_1C_LOSPANCHOS_MEMORIA_H
#define TP_2017_1C_LOSPANCHOS_MEMORIA_H

#include "../Commons/configuracion.h"
#include "../Commons/socket.h"
#include <commons/log.h>
#include <commons/config.h>

typedef struct {
    configMemoria configMemoria;
	void* memoriaPrincipal;
	t_log* logger;
} structMemoria;

void consolaMem_imprimir_encabezado();
void consolaMem_imprimir_menu();
void liberar_memoria(t_log* logger,configMemoria* config);
void* inicializarMemoria(structMemoria* structMemoria);

#endif


//
// Created by Martin Gauna on 4/2/17.
//

#ifndef TP_2017_1C_LOSPANCHOS_KERNEL_H
#define TP_2017_1C_LOSPANCHOS_KERNEL_H

#include "../Commons/configuracion.h"
#include "../Commons/socket.h"
#include <commons/log.h>
#include <commons/config.h>


void liberar_memoria(t_log* logger, configKernel* config);
void closeConections(int socketCPU, int socketFS, int socketMemoria, int socketConsola);
void printConfig(configKernel* conf);

#endif //TP_2017_1C_LOSPANCHOS_KERNEL_H

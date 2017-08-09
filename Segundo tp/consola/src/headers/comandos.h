#ifndef HEADERS_COMANDOS_H_
#define HEADERS_COMANDOS_H_

// Inclusiones
#include "consola.h"

// Funciones
void crearDiccionarioComandos();
void iniciarPrograma(char*);
void finalizarPrograma(char*);
void desconectarConsola();
void limpiarMensajes();
void help();
bool contienePid(void*);

#endif

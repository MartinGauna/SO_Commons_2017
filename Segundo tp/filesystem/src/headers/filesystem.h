#ifndef HEADERS_FILESYSTEM_H_
#define HEADERS_FILESYSTEM_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/log.h>
#include <protocolos/protocols.h>
#include <connections/networkHandler.h>
#include <commons/collections/list.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>
#include <structs/structs.h>
#include <unistd.h>
#include "logicaFileSystem.h"

void cargarConfig(char**);

void liberarMemoria();
void * servidorFileSystem(int socket);
void iniciarFilesystem(int argc, char* argv[]);
void escucharMensajesDelKernel(int socket);
void cargarMetadata();
void abrirFicheroBitMap();
void cargarPathsFileSystem();
void mapBitMap();
void validarArchivo(int32_t);
void crearArchivo(int32_t);
void borrarArchivo(int32_t);
void obtenerDatos(int32_t);
void guardarDatos(int32_t);
void crearFileSystem();

#endif

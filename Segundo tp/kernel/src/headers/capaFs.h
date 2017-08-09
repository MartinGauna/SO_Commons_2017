#ifndef HEADERS_CAPAFS_H_
#define HEADERS_CAPAFS_H_

#include "kernel.h"
#include "structs/structs.h"

t_descriptor_archivo abrirArchivo(char*, char*, int32_t);
int borrarArchivo(t_descriptor_archivo, int32_t, char**);
int cerrarArchivo(t_descriptor_archivo, int32_t);
PROTOCOLO_KERNEL_A_FS  moverCursorArchivo(t_descriptor_archivo, t_valor_variable, int32_t);
entrada_pft* obtenerEntradaPFT(t_descriptor_archivo descriptor, int32_t pid);
entrada_gft* obtenerEntradaGFT(entrada_pft* entrada_pft);
void pedido_abrir_archivo(int32_t);
void pedido_borrar_archivo(int32_t);
void pedido_cerrar_archivo(int32_t);
void pedido_mover_cursor_archivo(int32_t);
void pedido_escribir_archivo(int32_t);
void pedido_leer_archivo(int32_t);
bool existeArchivo(char* nombre);
bool crearArchivo(char* nombre);

t_descriptor_archivo obtenerFDGlobal(char*);

#endif

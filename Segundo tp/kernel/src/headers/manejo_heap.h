#ifndef HEADERS_MANEJO_HEAP_H_
#define HEADERS_MANEJO_HEAP_H_

#include "kernel.h"
#include "capaMemoria.h"
#include "conexionesKernelCpu.h"
#include "structs/structs.h"

t_bloque* crear_nuevo_bloque(int bytesPedidos, t_bloque* bloque_especial,t_list * lista_bloques);
t_bloque* buscar_primer_bloque_libre(int pagina, int bytesPedidos);
int buscar_ultimo_bloque_bis(t_list* lista_bloques);
nodo_tabla_heap* crear_nueva_pagina_heap(int bytesPedidos, int pid,int nro_pagina);
void liberar_bloque_heap(t_puntero puntero, int32_t pid, int socketCPU);
void asignar_paginas_heap(int32_t bytesPedidos, int32_t pid, int32_t socketFd );
void recalcular_offset_final(t_bloque* bloque_especial, t_bloque* bloque_nuevo);
void ocupar_bloque(t_bloque* bloque, int bytesPedidos);
int indice_bloque_especial(t_list * lista_de_bloques);
void chekear_paginas_libres(nodo_tabla_heap* pagina);
int calcular_numero_nuevapagina(int pid);
int check_existe_pagina_con_espacio(int bytesPedidos, int pid);
void liberar_paginas(int pid);
void borrar_pagina_en_kernel(nodo_tabla_heap* pagina);
heap_metadata* generar_heap_metadata(int32_t bytesPedidos, t_bool bul);
PROTOCOLO_MEMORIA_A_KERNEL_Y_CPU pedirNPaginas(int pid, int cantidad);
nodo_tabla_heap * buscar_pagina(int32_t pagina, int32_t pid);
int buscar_indice_bloque(int32_t pagina, int32_t offset, int32_t pid);

#endif

/*
 * PCB.h
 *
 *  Created on: 9/4/2017
 *      Author: utnso
 */

#ifndef PCB_H_
#define PCB_H_

typedef struct {
		int	PID;
		int PC;
		int contador_paginas;
		struct_icodigo * indice_codigo;
		struct_ietiqueta * indice_etiqueta;
		void * tabla_archivos;
		void * SP;
		int EC;
} struct_pcb;

typedef struct {} struct_ietiqueta;
typedef struct {} struct_icodigo;

static void liberar_pcb(struct_pcb *);

#endif /* PCB_H_ */

/*
 * PCB.c
 *
 *  Created on: 9/4/2017
 *      Author: utnso
 */
#include "PCB.h"

static struct_pcb * crear_pcb(int next_pid) {
	struct_pcb * pcb = malloc(size_of(struct_pcb));
	pcb->PID = next_pid;
	return pcb;
}

static void liberar_pcb(struct_pcb * self){
	free(self->PC);
	free(self->indice_codigo);
	free(self->indice_etiqueta);
	free(self->tabla_archivos);
	free(self->SP);
	free(self);
}

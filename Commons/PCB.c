/*
 * PCB.c
 *
 *  Created on: 21/5/2017
 *      Author: Martin Gauna
 */
#include "PCB.h"

t_pcb * crear_pcb(int pid) {
	t_pcb* pcb = (t_pcb *)malloc(sizeof(t_pcb));
	pcb->pid = pid;
	pcb->pc = 0;
	return pcb;
}

void free_pcb(t_pcb * self){
	if(self->ansisop){
		free(self->ansisop);
	}
	free(self);
}

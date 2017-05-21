/*
 * PCB.h
 *
 *  Created on: 21/5/2017
 *      Author: Martin Gauna
 */

#ifndef PCB_H_
#define PCB_H_

#include <stdlib.h>

typedef struct {
		int	pid;
		int pc;
		char* ansisop;
} t_pcb;

t_pcb * crear_pcb(int pid);
void free_pcb(t_pcb  *pcb);

#endif /* PCB_H_ */

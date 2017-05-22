/*
 * PCB.h
 *
 *  Created on: 21/5/2017
 *      Author: Martin Gauna
 */

#ifndef PCB_H_
#define PCB_H_

#include <stdlib.h>

enum exit_code {
	SIN_DEFINIR = -20,
	NO_MORE_PAGES = -9,
	MORE_MEM_THAN_PAGE,
	FINALIZAR_PROG_EXIT,
	DESC_CONSOLA,
	EXECP_MEM,
	NO_WRITE_PRIVILEGE,
	NO_READ_PRIVILEGE,
	NO_OPEN_PRIVILEGE,
	NO_RESOURCES,
	SUCCESSFUL
};

typedef struct {
		int	PID;
		int PC; //Program Counter
		void* FT; //File Table, Referencia a la tabla de archivos
		void* SP; //Stack Pointer
		int EC; //Exit Code
		char* codigo; // codigo AnsisOP
} t_pcb;

t_pcb * crear_pcb(int pid);
void free_pcb(t_pcb  *pcb);

#endif /* PCB_H_ */

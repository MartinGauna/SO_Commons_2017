/*
 * PCB.h
 *
 *  Created on: 21/5/2017
 *      Author: Martin Gauna
 */

#ifndef PCB_H_
#define PCB_H_

#include <stdlib.h>
#include <parser/metadata_program.h>

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
	SUCCESSFUL,
	NUEVO
};

typedef struct {

	int comienzo;
	int offset;

} t_registroIndiceCodigo;

typedef struct {

	int pagina;
	int offset;
	int size;

} t_posicionMemoria;

typedef struct {

	int argumentos;		//lista de argumentos
	int variables;		//lista de variables
	int direccionRetorno;
	t_posicionMemoria posicionMemoria;

} t_indiceStack;

typedef struct {
		int	pid;
		int programCounter; //Program Counter
		t_registroIndiceCodigo* indiceCodigo;
		t_indiceStack* indiceStack;
		int paginasDeCodigo;
		void* fileTable; //File Table, Referencia a la tabla de archivos
		void* stackPointer; //Stack Pointer
		int exitCode; //Exit Code
		int socketConsola;
		int consolaID;
		int socketCPU;
} t_pcb;

t_pcb * crear_pcb(int pid);
void free_pcb(t_pcb  *pcb);

#endif /* PCB_H_ */

/*
 * configKernel.h
 *
 *  Created on: 8/4/2017
 *      Author: utnso
 */

#ifndef CONFIGKERNEL_H_
#define CONFIGKERNEL_H_

typedef struct {
	int PUERTO_PROG;
	int PUERTO_CPU;
	char* IP_MEMORIA;
	int PUERTO_MEMORIA;
	char* IP_FS;
	int PUERTO_FS;
	int QUANTUM;
	int QUANTUM_SLEEP;
	char* ALGORITMO;
	int GRADO_MULTIPROG;
	char** SEM_IDS;//verificar que este bien
	int* SEM_INIT;//verificar que este bien
	char** SHARED_VARS;//qverificar q este bien
	int STACK_SIZE;
} t_configKernel;

t_configKernel leerArchConfigKernel(char* unPath);
void setChar(char** charSet, char* parametroBuscado);
void setInt(int* intSet, char* parametroBuscado);

#endif /* CONFIGKERNEL_H_ */

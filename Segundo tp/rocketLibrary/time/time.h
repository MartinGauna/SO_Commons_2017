#ifndef TIME_TIME_H_
#define TIME_TIME_H_

#include <commons/temporal.h>
#include <commons/string.h>
#include <string.h>
#include <stdlib.h>

typedef enum {

	SUMA = 0, RESTA = 1

} t_operador;

void* calcular(char*, char*);
char* sumar(char*, char*);

#endif

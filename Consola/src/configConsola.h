/*
 * configConsola.h
 *
 *  Created on: 6/4/2017
 *      Author: utnso
 */

#ifndef CONFIGCONSOLA_H_
#define CONFIGCONSOLA_H_

typedef struct {
	char* IP_KERNEL;
	int PUERTO_KERNELL;
} t_regConfig;

//Lee el archivo de configuracion de la consola y crea el struct correspondiente
t_regConfig leeArchConfigConsola();

//Setea e imprime el valor del puerto leido desde el archivo de configuracion
void setPUERTO(int* valorSet, char* valorBuscado);

//Setea e imprime el valor del IP leido desde el archivo de configuracion
void setIP(char** valorSet, char* valorBuscado);

#endif /* CONFIGCONSOLA_H_ */

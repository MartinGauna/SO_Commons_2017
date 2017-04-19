/*
 * configFileSystem.h
 *
 *  Created on: 8/4/2017
 *      Author: utnso
 */

typedef struct {
	int PUERTO; //tuve que ponerle puerto2 porque ya habia una variable llamada puerto en el main
	char* PUNTO_MONTAJE;
} t_configFileSystem;

t_configFileSystem* leerConfigFS();

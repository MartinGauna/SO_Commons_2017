/*
 * configFileSystem.h
 *
 *  Created on: 8/4/2017
 *      Author: utnso
 */

typedef struct {
	int PUERTO;
	char* PUNTO_MONTAJE;
} t_configFileSystem;

t_configFileSystem leerArchConfigFileSystem(char* path);

void setChar(char** charSet, char* parametroBuscado);

void setInt(int* intSet, char* parametroBuscado);

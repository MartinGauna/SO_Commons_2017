/*
 * archivo_configuracion.c
 *
 *  Created on: 3/4/2017
 *      Author: utnso
 */

#include "configuracion.h"

void* cargarConfiguracion(char* path,int configParamAmount,processType configType, t_log* logger){
	t_config* configFile;
	configConsole* confConsola;
	configCPU* confCPU;
	configFileSystem* confFileSystem;
	configKernel* confKernel;
	configMemoria* confMemoria;

	configFile = config_create(path);
	if (!configFile || configFile->properties->elements_amount == 0) {
		log_error(logger, "No se encontro el archivo de configuracion.\n");
		exit(EXIT_FAILURE);
	}

	if (config_keys_amount(configFile) != configParamAmount) {
		log_error(logger, "No se encuentran inicializados todos los parametros de configuracion requeridos.");
		exit(EXIT_FAILURE);
	}

	switch(configType){
	case CONSOLA:
		confConsola = (configConsole*) malloc(sizeof(configConsole));
		confConsola->puerto = leerPuerto(configFile, "PUERTO_KERNEL", logger);
		confConsola->ip = leerIP(configFile, "IP_KERNEL", logger);
		return confConsola;
	case CPU:
		confCPU = (configCPU*)malloc(sizeof(configCPU));
		confCPU->puertoKernel = leerPuerto(configFile, "PUERTO_KERNEL", logger);
		confCPU->ipKernel = leerIP(configFile, "IP_KERNEL", logger);
		confCPU->puertoMemoria = leerPuerto(configFile, "PUERTO_MEMORIA", logger);
		confCPU->ipMemoria = leerIP(configFile, "IP_MEMORIA", logger);
		return confCPU;
	case FILESYSTEM:
		confFileSystem = (configFileSystem*)malloc(sizeof(configFileSystem));
		confFileSystem->puerto = leerPuerto(configFile, "PUERTO", logger);
		confFileSystem->puntoMontaje = leerString(configFile, "PUNTO_MONTAJE", logger);
		return confFileSystem;
	case KERNEL:
		confKernel = (configKernel*)malloc(sizeof(configKernel));
		confKernel->puerto_prog = leerPuerto(configFile, "PUERTO_PROG", logger);
		confKernel->puertoCPU = leerPuerto(configFile, "PUERTO_CPU", logger);
		confKernel->ipMemoria = leerIP(configFile, "IP_MEMORIA", logger);
		confKernel->puertoMemoria = leerPuerto(configFile, "PUERTO_MEMORIA", logger);
		confKernel->ipFS = leerIP(configFile, "IP_FS", logger);
		confKernel->puertoFS = leerPuerto(configFile, "PUERTO_FS", logger);
		confKernel->quantum = leerInt(configFile, "QUANTUM", logger);
		confKernel->quantumSleep = leerInt(configFile, "QUANTUM_SLEEP", logger);
		confKernel-> algoritmo = leerString(configFile, "ALGORITMO", logger);
		confKernel->gradoMultiprog = leerInt(configFile, "GRADO_MULTIPROG", logger);
		confKernel->semIds = leerString(configFile, "SEM_IDS", logger);
		confKernel->semInits = leerString(configFile, "SEM_INIT", logger);
		confKernel->sharedVars = leerString(configFile, "SHARED_VARS", logger);
		confKernel->stackSize = leerInt(configFile, "STACK_SIZE", logger);
		return confKernel;
	case MEMORIA:
		confMemoria = (configMemoria*)malloc(sizeof(configMemoria));
		confMemoria->puerto = leerPuerto(configFile, "PUERTO", logger);
		confMemoria->marcos = leerInt(configFile, "MARCOS", logger);
		confMemoria->marcoSize = leerInt(configFile, "MARCO_SIZE", logger);
		confMemoria->entradasCache =  leerInt(configFile, "ENTRADAS_CACHE", logger);
		confMemoria->cacheXProc =  leerInt(configFile, "CACHE_X_PROC", logger);
		confMemoria->retardoMemoria =  leerInt(configFile, "RETARDO_MEMORIA", logger);
		configParamAmount = 6;
		return confMemoria;
	default:
			return NULL;
	}

}

char* leerString (void* configFile, char* parametro, t_log* logger){
	char* string = "";
	if (config_has_property(configFile, parametro)) {
		string = config_get_string_value(configFile, parametro);
		//TODO: Valida que no sea vacio
	} else {
		log_error(logger, "No se encuentra el parametro en el archivo de config.");//TODO generar el string correcto para el error
		exit(EXIT_FAILURE);
	}
	return string;
}

int leerInt (void* configFile, char* parametro, t_log* logger){
	int valor = 0;
	if (config_has_property(configFile, parametro)) {
		valor = config_get_int_value(configFile, parametro);
	} else {
		log_error(logger, "No se encuentra el parametro en el archivo de configuracion.");//TODO GENERAR UN BUEN TEXTO PARA EL LOG
		exit(EXIT_FAILURE);
	}
	return valor;
}

int leerPuerto (void* configFile, char* parametro, t_log* logger){
	int puerto = 0;
	puerto = leerInt(configFile, parametro, logger);
	validar_puerto(puerto, logger);
	return puerto;
}

char* leerIP (void* configFile, char* parametro, t_log* logger){
	char* ip = "";
	ip = leerString (configFile, parametro,logger);
	validar_ip(ip,logger);
	return ip;
}

void validar_puerto(int puerto, t_log* logger){
	if (puerto < PUERTO_MAX) {
		log_error(logger, "El numero de puerto indicado se encuentra reservado para el sistema.");
		exit(EXIT_FAILURE);
	}
	return;
}

void validar_ip(char* ip, t_log* logger) {
	regex_t regex;
	int regres;
	char msgbuf[100];

	regres = regcomp(&regex, "^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$",
					REG_EXTENDED);		//REVISAR LA EXPRESION REGULAR
	if (regres) {
		log_error(logger,"Error al generar la regex para validar IP.");
		exit(1);
	}

	regres = regexec(&regex, ip, 0, NULL, 0);

	if (regres == REG_NOMATCH) {
		log_error(logger,"IP invalida.");
		exit(EXIT_FAILURE);
	} else {
//		log_info(logger, "IP del Kernel valida.");
	    regerror(regres, &regex, msgbuf, sizeof(msgbuf));
//	    fprintf(stderr, "Regex match failed: %s\n", msgbuf);
	}
}






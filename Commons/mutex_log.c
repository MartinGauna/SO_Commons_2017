/*
 * mutex_log.c
 *
 *  Created on: 20/5/2017
 *      Author: utnso
 */


#include "mutex_log.h"

void log_info_mutex(t_log* logger, char* mensaje) {

	pthread_mutex_lock(&mutex);
	log_info(logger,mensaje);
	pthread_mutex_unlock(&mutex);

}

void log_warning_mutex(t_log* logger, char* mensaje) {

	pthread_mutex_lock(&mutex);
	log_warning(logger,mensaje);
	pthread_mutex_unlock(&mutex);

}

void log_error_mutex(t_log* logger, char* mensaje) {

	pthread_mutex_lock(&mutex);
	log_error(logger,mensaje);
	pthread_mutex_unlock(&mutex);

}

void log_debug_mutex(t_log* logger, char* mensaje) {

	pthread_mutex_lock(&mutex);
	log_debug(logger,mensaje);
	pthread_mutex_unlock(&mutex);

}

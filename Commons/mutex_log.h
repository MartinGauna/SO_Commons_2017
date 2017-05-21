/*
 * mutex_log.h
 *
 *  Created on: 20/5/2017
 *      Author: utnso
 */

#ifndef MUTEX_LOG_H_
#define MUTEX_LOG_H_

#include <commons/log.h>
#include <pthread.h>

extern pthread_mutex_t mutex;

void log_info_mutex(t_log* logger, char* mensaje);
void log_warning_mutex(t_log* logger, char* mensaje);
void log_error_mutex(t_log* logger, char* mensaje);
void log_debug_mutex(t_log* logger, char* mensaje);

#endif /* MUTEX_LOG_H_ */

/*
 * conexionesKernelFs.c
 *
 *  Created on: 30/5/2017
 *      Author: utnso
 */

#include "headers/conexionesKernelFs.h"


void abrirArchivoExterna(char* archivo, char* permisos,int32_t pid){

	PROTOCOLO_KERNEL_A_FS validarArchivo = VALIDAR_ARCHIVO;
	int32_t tamanioPath = string_length(archivo);


	librocket_enviarMensaje(fsSocket,&validarArchivo,sizeof(int32_t),infoLogger);
	librocket_enviarMensaje(fsSocket,&tamanioPath,sizeof(int32_t),infoLogger);
	librocket_enviarMensaje(fsSocket,archivo,sizeof(int32_t),infoLogger);

	//TODO obtener respuesta de creacion del archivo
	if(SUCCESS){
		//el archivo existe
		abrirArchivo(archivo,permisos,pid);

	}else {
		log_info(infoLogger,"El archivo con path %s no existe",archivo);

	}

}

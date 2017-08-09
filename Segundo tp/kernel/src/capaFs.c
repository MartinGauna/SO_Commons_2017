#include "headers/capaFs.h"

t_descriptor_archivo abrirArchivo(char* archivo, char* permisos, int32_t pid) {
	log_info(infoLogger,
			"Intentando abrir archivo %s para el PID %d con permisos %s",
			archivo, pid, permisos);
	agregarSyscallAPcb(pid);
	char* pidString = itoa(pid);
	t_list* pft = dictionary_get(dicPID_PFT, pidString);

	entrada_pft* entrada_pft = malloc(sizeof(entrada_pft));
	entrada_pft->flags = permisos;
	entrada_pft->global_fd = obtenerFDGlobal(archivo);
	if (entrada_pft->global_fd == -1) {
		if (existeArchivo(archivo)) {
			log_info(infoLogger,
					"El archivo no se encuentra abierto pero existe en el Filesystem");
		} else {
			log_info(infoLogger, "El archivo %s no existe", archivo);
			if (string_contains(entrada_pft->flags, "c")) {
				//crear archivo
				bool noSePudoAbrir;
				noSePudoAbrir = crearArchivo(archivo);
				if (!noSePudoAbrir) {
					return -2;
				}
			} else {
				//terminar programa
				log_info(infoLogger,
						"El proceso %d quiso abrir el archivo %s pero sin permisos de creacion y el mismo no existe, permisos: %s",
						pid, archivo, permisos);
				return NULL;
			}
		}
		//agregar a la tabla global
		entrada_gft* nuevaEntradaGlobal = malloc(sizeof(entrada_gft));
		nuevaEntradaGlobal->abiertos = 1;
		nuevaEntradaGlobal->nombre = string_new();
		string_append(&(nuevaEntradaGlobal->nombre), archivo);
		int globalFD = list_add(globalFileTable, nuevaEntradaGlobal);
		entrada_pft->global_fd = globalFD;
		entrada_pft->cursor = 0;
		//agrego entrada a la tabla del proceso
		t_descriptor_archivo fd = list_size(pft); // el descriptor de archivo es la nueva entrada en la PFT
		list_add(pft, entrada_pft);
		return fd;
	} else {
		//aumentar el contador en la tabla global
		entrada_gft* entradaGlobal = list_get(globalFileTable,
				entrada_pft->global_fd);
		log_info(infoLogger, "El archivo ya fue abierto previamente %d vece/s",
				entradaGlobal->abiertos);
		entradaGlobal->abiertos++;
		//agrego entrada a la tabla del proceso
		t_descriptor_archivo fd = list_size(pft); // el descriptor de archivo es la nueva entrada en la PFT
		entrada_pft->cursor = 0;
		list_add(pft, entrada_pft);
		return fd;
	}
}

int borrarArchivo(t_descriptor_archivo descriptor, int32_t pid,
		char** nombreArchivo) {
	log_info(infoLogger,
			"Intentando borrar el archivo %d para el PID %d con permisos",
			descriptor, pid);
	agregarSyscallAPcb(pid);
	char* pidString = itoa(pid);
	t_list* pft = dictionary_get(dicPID_PFT, pidString);
	if (pft == NULL) {
		log_error(infoLogger,
				"No se encontro la tabla de archivos del proceso para el descriptor %d y el pid %d",
				descriptor, pid);
		return -1;
	} else {
		entrada_pft* entrada_pft = list_get(pft, descriptor);
		if (entrada_pft == NULL) {
			log_error(infoLogger,
					"No se encontro el archivo con descriptor %d para el pid %d",
					descriptor, pid);
			return -1;
		} else {
			entrada_gft* entrada_gft = list_get(globalFileTable,
					entrada_pft->global_fd);
			if (entrada_gft->abiertos == 1) {
				string_append(nombreArchivo, entrada_gft->nombre);
				return 0;
			} else {
				log_error(infoLogger,
						"No pudo eliminarse el archivo %d para el pid %d. Se encuentra abierto por otros procesos",
						descriptor, pid);
				return -1;
			}
		}
	}
}

int cerrarArchivo(t_descriptor_archivo descriptor, int32_t pid) {
	log_info(infoLogger, "Intentando cerrar el archivo %d del proceso %d",
			descriptor, pid);
	agregarSyscallAPcb(pid);
	char* pidString = itoa(pid);
	t_list* pft = dictionary_get(dicPID_PFT, pidString);

	entrada_pft* entrada_pft = list_get(pft, descriptor);
	if (entrada_pft == NULL) {
		log_error(infoLogger,
				"No se encontro el archivo con descriptor %d para el pid %d",
				descriptor, pid);
		return -1;
	} else {
		list_remove(pft, descriptor);
		//busco la entrada en la global file table y descuento el contador o la elimino

		entrada_gft* entrada_gft = list_get(globalFileTable,
				entrada_pft->global_fd);
		if (entrada_gft->abiertos > 1) {
			entrada_gft->abiertos--;
		} else {
			//borro la entrada de la GLOBAL FILE TABLE
			list_remove(globalFileTable, entrada_pft->global_fd);
		}
		return 0;
	}
}

PROTOCOLO_KERNEL_A_FS moverCursorArchivo(t_descriptor_archivo descriptor,
		t_valor_variable posicion, int32_t pid) {

	entrada_pft* entrada_pft = obtenerEntradaPFT(descriptor, pid);
	agregarSyscallAPcb(pid);
	if (entrada_pft == NULL) {
		return ARCHIVO_INVALIDO;
	}

	entrada_gft* entrada_gft = obtenerEntradaGFT(entrada_pft);

	if (entrada_gft == NULL) {
		return ARCHIVO_INVALIDO;
	}

	entrada_pft->cursor = posicion;

	return MOVER_CURSOR_EXITOSO;

}

t_descriptor_archivo obtenerFDGlobal(char* file) {

	//Poner nombres mas descriptivos.

	int i;
	for (i = 0; i < list_size(globalFileTable); i++) {
		entrada_gft* entrada = list_get(globalFileTable, i);
		if (string_equals_ignore_case(file, entrada->nombre)) {
			return i;
		}
	}
	return -1;
}

entrada_gft* obtenerEntradaGFT(entrada_pft* entrada_pft) {

	//Poner nombres mas descriptivos.

	entrada_gft* entrada_gft = list_get(globalFileTable,
			entrada_pft->global_fd);
	return entrada_gft;
}

entrada_pft* obtenerEntradaPFT(t_descriptor_archivo descriptor, int32_t pid) {

	//Poner nombres mas descriptivos.

	char* pidString = itoa(pid);
	t_list* pft = dictionary_get(dicPID_PFT, pidString);
	if (pft == NULL) {
		return NULL;
	} else {
		entrada_pft* entrada_pft = list_get(pft, descriptor);
		return entrada_pft;
	}

}

void pedido_abrir_archivo(int32_t socketCPU) {

	int32_t pid = 0;
	char* direccion;
	t_banderas permisos;
	int32_t tamanioDireccion;
	char* permisosString = string_new();
	librocket_recibirMensaje(socketCPU, &pid, sizeof(int32_t), infoLogger);
	librocket_recibirMensaje(socketCPU, &tamanioDireccion, sizeof(int32_t),
			infoLogger);
	direccion = malloc(tamanioDireccion);
	librocket_recibirMensaje(socketCPU, direccion, tamanioDireccion,
			infoLogger);
	librocket_recibirMensaje(socketCPU, &permisos, sizeof(t_banderas),
			infoLogger);

	if (permisos.creacion)
		string_append_with_format(&permisosString, "%s,", "c");

	if (permisos.escritura)
		string_append_with_format(&permisosString, "%s,", "w");

	if (permisos.lectura)
		string_append_with_format(&permisosString, "%s,", "r");

	t_descriptor_archivo fd = abrirArchivo(direccion, permisosString, pid);
	PROTOCOLO_KERNEL_A_CPU respuestaApertura;
	if (fd == NULL) {
		respuestaApertura = ARCHIVO_INVALIDO;
		librocket_enviarMensaje(socketCPU, &respuestaApertura,
				sizeof(PROTOCOLO_KERNEL_A_CPU), infoLogger);
	} else if (fd == -2) {
		respuestaApertura = ESPACIO_INSUFICIENTE_EN_FS;
		librocket_enviarMensaje(socketCPU, &respuestaApertura,
				sizeof(PROTOCOLO_KERNEL_A_CPU), infoLogger);
	} else {
		respuestaApertura = APERTURA_EXITOSA;
		librocket_enviarMensaje(socketCPU, &respuestaApertura,
				sizeof(PROTOCOLO_KERNEL_A_CPU), infoLogger);
		librocket_enviarMensaje(socketCPU, &fd, sizeof(t_descriptor_archivo),
				infoLogger);
	}

}

void pedido_borrar_archivo(int32_t socketCPU) {

	int32_t pid = 0;
	t_descriptor_archivo descriptor;

	librocket_recibirMensaje(socketCPU, &pid, sizeof(int32_t), infoLogger);
	librocket_recibirMensaje(socketCPU, &descriptor,
			sizeof(t_descriptor_archivo), infoLogger);

	char* nombreArchivo = string_new();
	int resultado = borrarArchivo(descriptor, pid, &nombreArchivo);
	//OK
	if (resultado == 0) {

		//Mando orden de borrar al FS
		int32_t tamanioNombreArchivo = string_length(nombreArchivo);
		tamanioNombreArchivo++;

		PROTOCOLO_KERNEL_A_FS borrarArchivo = BORRAR_ARCHIVO;
		librocket_enviarMensaje(fsSocket, &borrarArchivo,
				sizeof(PROTOCOLO_KERNEL_A_FS), infoLogger);
		librocket_enviarMensaje(fsSocket, &tamanioNombreArchivo,
				sizeof(int32_t), infoLogger);
		librocket_enviarMensaje(fsSocket, nombreArchivo, tamanioNombreArchivo,
				infoLogger);

		PROTOCOLO_FS_A_KERNEL respuestaFs;
		librocket_recibirMensaje(fsSocket, &respuestaFs,
				sizeof(PROTOCOLO_FS_A_KERNEL), infoLogger);

		if (respuestaFs == ARCHIVO_BORRADO_OK) {
			PROTOCOLO_KERNEL_A_CPU respuesta = BORRADO_EXITOSO;
			librocket_enviarMensaje(socketCPU, &respuesta,
					sizeof(PROTOCOLO_KERNEL_A_CPU), infoLogger);
		} else {
			PROTOCOLO_KERNEL_A_CPU respuesta = BORRADO_ERRONEO;
			librocket_enviarMensaje(socketCPU, &respuesta,
					sizeof(PROTOCOLO_KERNEL_A_CPU), infoLogger);
		}

	} else {
		PROTOCOLO_KERNEL_A_CPU respuesta = BORRADO_ERRONEO;
		librocket_enviarMensaje(socketCPU, &respuesta,
				sizeof(PROTOCOLO_KERNEL_A_CPU), infoLogger);
	}

}

void pedido_cerrar_archivo(int32_t socketCPU) {

	int32_t pid = 0;
	t_descriptor_archivo descriptor;

	librocket_recibirMensaje(socketCPU, &pid, sizeof(int32_t), infoLogger);
	librocket_recibirMensaje(socketCPU, &descriptor,
			sizeof(t_descriptor_archivo), infoLogger);

	//si el descriptor es 0,1 o 2 devolver archivo invalido
	if (descriptor < 3) {
		PROTOCOLO_KERNEL_A_FS respuesta = ARCHIVO_INVALIDO;
		librocket_enviarMensaje(socketCPU, &respuesta, sizeof(int32_t),
				infoLogger);
		return;
	}

	int resultado = cerrarArchivo(descriptor, pid);
	if (resultado == -1) {
		PROTOCOLO_KERNEL_A_FS respuesta = ARCHIVO_INVALIDO;
		librocket_enviarMensaje(socketCPU, &respuesta, sizeof(int32_t),
				infoLogger);
	} else {
		PROTOCOLO_KERNEL_A_FS respuesta = CERRADO_EXITOSO;
		librocket_enviarMensaje(socketCPU, &respuesta, sizeof(int32_t),
				infoLogger);
	}
}

void pedido_mover_cursor_archivo(int32_t socketCPU) {

	int32_t pid = 0;
	t_descriptor_archivo descriptor;
	t_valor_variable posicion;

	librocket_recibirMensaje(socketCPU, &pid, sizeof(int32_t), infoLogger);
	librocket_recibirMensaje(socketCPU, &descriptor,
			sizeof(t_descriptor_archivo), infoLogger);
	librocket_recibirMensaje(socketCPU, &posicion, sizeof(t_valor_variable),
			infoLogger);

	PROTOCOLO_KERNEL_A_CPU response = moverCursorArchivo(descriptor, posicion,
			pid);

	librocket_enviarMensaje(socketCPU, &response, sizeof(int32_t), infoLogger);

}

void pedido_escribir_archivo(int32_t socketCPU) {

	int32_t pid = 0;
	t_descriptor_archivo descriptor;
	t_valor_variable tamanio;
	void* informacion;

	librocket_recibirMensaje(socketCPU, &pid, sizeof(int32_t), infoLogger);
	librocket_recibirMensaje(socketCPU, &descriptor,
			sizeof(t_descriptor_archivo), infoLogger);
	librocket_recibirMensaje(socketCPU, &tamanio, sizeof(t_valor_variable),
			infoLogger);
	informacion = malloc(tamanio);
	librocket_recibirMensaje(socketCPU, informacion, tamanio, infoLogger);

	entrada_pft* entrada_pft = obtenerEntradaPFT(descriptor, pid);
	agregarSyscallAPcb(pid);
	if (entrada_pft == NULL) {
		PROTOCOLO_KERNEL_A_CPU respuesta = ARCHIVO_INVALIDO;
		librocket_enviarMensaje(socketCPU, &respuesta,
				sizeof(PROTOCOLO_KERNEL_A_CPU), infoLogger);

	} else if (descriptor == 1) {

		PROTOCOLO_KERNEL_A_CONSOLA impresion = NUEVA_IMPRESION;

		entrada_programaSocket_pid* programaQueQuiereImprimir =
				buscarHiloProgramaByPID(pid);

		if (programaQueQuiereImprimir == NULL) {
			//la consola no esta activa, no se puede imprimir ameo
			log_info(infoLogger,
					"Se quiso hacer un print de pantalla por el pid %i pero su consola se cayo",
					pid);
			//lemando a la cpu que se escribio porque sino me va a decir que aborta el programa y no es lo que quiero, el mismo se mata en la proxima termine_sentencia
			PROTOCOLO_KERNEL_A_CPU respuesta = ESCRITURA_EXITOSA;
			librocket_enviarMensaje(socketCPU, &respuesta,
					sizeof(PROTOCOLO_KERNEL_A_CPU), infoLogger);
		} else {
			librocket_enviarMensaje(programaQueQuiereImprimir->programaSocket,
					&impresion, sizeof(PROTOCOLO_KERNEL_A_CONSOLA), infoLogger);
			librocket_enviarMensaje(programaQueQuiereImprimir->programaSocket,
					&tamanio, sizeof(t_valor_variable), infoLogger);
			librocket_enviarMensaje(programaQueQuiereImprimir->programaSocket,
					informacion, tamanio, infoLogger);

			log_info(infoLogger, "El proceso de PID: %i.\n", pid);
			log_info(infoLogger,
					"Conectado con una consola en el Socket: %i.\n",
					programaQueQuiereImprimir->programaSocket);
			log_info(infoLogger, "Pide imprimir por pantalla.\n",
					programaQueQuiereImprimir->programaSocket);

			PROTOCOLO_CONSOLA_A_KERNEL respuestaConsola;
			int32_t resultado = librocket_recibirMensaje(
					programaQueQuiereImprimir->programaSocket,
					&respuestaConsola, sizeof(PROTOCOLO_CONSOLA_A_KERNEL),
					infoLogger);

			if (resultado == 0) {
				log_error(infoLogger, "Se cayo la Consola.\n");
				exit(EXIT_FAILURE);
			}

			if (respuestaConsola == IMPRESION_EXITOSA) {
				PROTOCOLO_KERNEL_A_CPU respuesta = ESCRITURA_EXITOSA;
				librocket_enviarMensaje(socketCPU, &respuesta,
						sizeof(PROTOCOLO_KERNEL_A_CPU), infoLogger);
			} else if (respuestaConsola == IMPRESION_FALLIDA) {
				PROTOCOLO_KERNEL_A_CPU respuesta = ARCHIVO_INVALIDO;
				librocket_enviarMensaje(socketCPU, &respuesta,
						sizeof(PROTOCOLO_KERNEL_A_CPU), infoLogger);
			}
		}

	} else {
		entrada_gft* entrada_gft = obtenerEntradaGFT(entrada_pft);
		char* path = entrada_gft->nombre;
		if (!string_contains(entrada_pft->flags, "w")) {
			//no tiene permiso de escritura
			PROTOCOLO_KERNEL_A_CPU permisoInvalido = PERMISO_ARCHIVO_INVALIDO;
			librocket_enviarMensaje(socketCPU, &permisoInvalido,
					sizeof(PROTOCOLO_KERNEL_A_CPU), infoLogger);
			log_info(infoLogger,
					"el proceso %d quiso escribir el archivo %s con permisos %d",
					pid, path, entrada_pft->flags);
			return;
		}

		int32_t offset = entrada_pft->cursor;
		int32_t largoPath = string_length(path);
		largoPath++;

		PROTOCOLO_KERNEL_A_FS mensaje = GUARDAR_DATOS;
		librocket_enviarMensaje(fsSocket, &mensaje,
				sizeof(PROTOCOLO_KERNEL_A_FS), infoLogger);
		librocket_enviarMensaje(fsSocket, &largoPath, sizeof(int32_t),
				infoLogger);
		librocket_enviarMensaje(fsSocket, path, largoPath, infoLogger);

		librocket_enviarMensaje(fsSocket, &offset, sizeof(int32_t), infoLogger);

		librocket_enviarMensaje(fsSocket, &tamanio, sizeof(t_valor_variable),
				infoLogger);
		librocket_enviarMensaje(fsSocket, informacion, tamanio, infoLogger);

		PROTOCOLO_FS_A_KERNEL respuestaFs;
		int32_t resultado = librocket_recibirMensaje(fsSocket, &respuestaFs,
				sizeof(PROTOCOLO_FS_A_KERNEL), infoLogger);

		if (resultado == 0) {
			log_info(infoLogger, "Se cayo el Filesystem.\n");
			exit(EXIT_FAILURE);
		}

		if (respuestaFs == ESCRIBIR_OK) {
			entrada_pft->cursor += tamanio;
			PROTOCOLO_KERNEL_A_CPU respuesta = ESCRITURA_EXITOSA;
			librocket_enviarMensaje(socketCPU, &respuesta,
					sizeof(PROTOCOLO_KERNEL_A_CPU), infoLogger);
		} else {
			PROTOCOLO_KERNEL_A_CPU respuesta = ARCHIVO_INVALIDO;
			librocket_enviarMensaje(socketCPU, &respuesta,
					sizeof(PROTOCOLO_KERNEL_A_CPU), infoLogger);
		}
	}
}

bool existeArchivo(char* nombre) {
	log_info(infoLogger, "Validando la existencia del archivo %s", nombre);
	PROTOCOLO_KERNEL_A_FS existeArchivoMsj = VALIDAR_ARCHIVO;
	int32_t tamanioNombre = string_length(nombre);
	tamanioNombre++;

	librocket_enviarMensaje(fsSocket, &existeArchivoMsj,
			sizeof(PROTOCOLO_KERNEL_A_FS), infoLogger);
	librocket_enviarMensaje(fsSocket, &tamanioNombre, sizeof(int32_t),
			infoLogger);
	librocket_enviarMensaje(fsSocket, nombre, tamanioNombre, infoLogger);

	PROTOCOLO_FS_A_KERNEL respuesta;
	librocket_recibirMensaje(fsSocket, &respuesta,
			sizeof(PROTOCOLO_KERNEL_A_FS), infoLogger);

	if (respuesta == EXISTE_ARCHIVO) {
		log_info(infoLogger, "El archivo existe!");
		return true;
	} else if (respuesta == NO_EXISTE_ARCHIVO) {
		log_info(infoLogger, "El archivo %s no existe", nombre);
		return false;
	} else {
		log_error(infoLogger, "----FS RESPONDIO ALGO QUE NO ENTIENDO!!");
		return false;
	}

}

bool crearArchivo(char* nombre) {

	log_info(infoLogger, "Pidiendo a FS la creacion del archivo con nombre %s",
			nombre);
	PROTOCOLO_KERNEL_A_FS crearArchivo = CREAR_ARCHIVO;
	int tamanioNombre = string_length(nombre);
	tamanioNombre++;

	librocket_enviarMensaje(fsSocket, &crearArchivo,
			sizeof(PROTOCOLO_KERNEL_A_FS), infoLogger);
	librocket_enviarMensaje(fsSocket, &tamanioNombre,
			sizeof(PROTOCOLO_KERNEL_A_FS), infoLogger);
	librocket_enviarMensaje(fsSocket, nombre, tamanioNombre, infoLogger);

	PROTOCOLO_FS_A_KERNEL respuesta;
	librocket_recibirMensaje(fsSocket, &respuesta,
			sizeof(PROTOCOLO_FS_A_KERNEL), infoLogger);

	if (respuesta == ARCHIVO_CREADO_CON_EXITO) {
		log_info(infoLogger, "Se creo con exito el archivo");
		return true;
	} else if (respuesta == FALLO_EN_LA_CREACION_DE_ARCHIVO) {
		log_error(infoLogger, "Fallo la creacion de un archivo");
		return false;
	} else {
		log_error(infoLogger, "ERROR: No entiendo lo que me dice el FS");
		return false;
	}

}

void pedido_leer_archivo(int32_t socketCPU) {

	int32_t pid = 0;
	t_descriptor_archivo descriptor;
	t_valor_variable tamanio;

	librocket_recibirMensaje(socketCPU, &pid, sizeof(int32_t), infoLogger);
	librocket_recibirMensaje(socketCPU, &descriptor,
			sizeof(t_descriptor_archivo), infoLogger);
	librocket_recibirMensaje(socketCPU, &tamanio, sizeof(t_valor_variable),
			infoLogger);
	agregarSyscallAPcb(pid);
	entrada_pft* entrada_pft = obtenerEntradaPFT(descriptor, pid);
	if (entrada_pft == NULL) {
		PROTOCOLO_KERNEL_A_CPU respuesta = ARCHIVO_INVALIDO;
		librocket_enviarMensaje(socketCPU, &respuesta,
				sizeof(PROTOCOLO_KERNEL_A_CPU), infoLogger);
	} else {
		char* path = obtenerEntradaGFT(entrada_pft)->nombre;
		if (!string_contains(entrada_pft->flags, "r")) {
			//no tiene permiso de lectura
			PROTOCOLO_KERNEL_A_CPU permisoInvalido = PERMISO_ARCHIVO_INVALIDO;
			librocket_enviarMensaje(socketCPU, &permisoInvalido,
					sizeof(PROTOCOLO_KERNEL_A_CPU), infoLogger);
			log_error(infoLogger,
					"el proceso %d quiso leer el archivo %s con permisos %d",
					pid, path, entrada_pft->flags);
			return;
		}

		int32_t largoPath = string_length(path);
		int32_t offset = entrada_pft->cursor;
		largoPath++;

		PROTOCOLO_KERNEL_A_FS mensaje = OBTENER_DATOS;
		librocket_enviarMensaje(fsSocket, &mensaje,
				sizeof(PROTOCOLO_KERNEL_A_FS), infoLogger);
		librocket_enviarMensaje(fsSocket, &largoPath, sizeof(int32_t),
				infoLogger);
		librocket_enviarMensaje(fsSocket, path, largoPath, infoLogger);
		librocket_enviarMensaje(fsSocket, &offset, sizeof(t_puntero),
				infoLogger);
		librocket_enviarMensaje(fsSocket, &tamanio, sizeof(t_valor_variable),
				infoLogger);

		PROTOCOLO_FS_A_KERNEL respuestaFs;
		librocket_recibirMensaje(fsSocket, &respuestaFs,
				sizeof(PROTOCOLO_FS_A_KERNEL), infoLogger);

		if (respuestaFs == ENVIO_DATOS) {

			int32_t sizeLectura;
			librocket_recibirMensaje(fsSocket, &sizeLectura, sizeof(int32_t),
					infoLogger);

			char* bytesLeidos = malloc(sizeLectura);
			librocket_recibirMensaje(fsSocket, bytesLeidos, sizeLectura,
					infoLogger);

			PROTOCOLO_KERNEL_A_CPU respuesta = LECTURA_EXITOSA;
			librocket_enviarMensaje(socketCPU, &respuesta,
					sizeof(PROTOCOLO_KERNEL_A_CPU), infoLogger);

			librocket_enviarMensaje(socketCPU, bytesLeidos, sizeLectura,
					infoLogger);

			entrada_pft->cursor = +tamanio;

		} else {
			PROTOCOLO_KERNEL_A_CPU respuesta = ARCHIVO_INVALIDO;
			librocket_enviarMensaje(socketCPU, &respuesta,
					sizeof(PROTOCOLO_KERNEL_A_CPU), infoLogger);
		}
	}

}

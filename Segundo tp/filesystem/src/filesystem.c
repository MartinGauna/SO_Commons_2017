#include "headers/filesystem.h"

int main(int argc, char* argv[]) {

	iniciarFilesystem(argc, argv);

	librocket_levantarServidorSimple(filesystem_config->puerto, 10,
			&servidorFileSystem);

	liberarMemoria();

	return EXIT_SUCCESS;

}

void iniciarFilesystem(int argc, char* argv[]) {

	printf("File System inicializado.\n");

	if (argc < 2) {
		printf("No se paso por parametro el path de las configs \n");
		exit(1);
	}

	infoLogger = log_create("info_logs.txt", "filesystem", false,
			LOG_LEVEL_INFO);

	cargarConfig(argv);
	cargarPathsFileSystem();
	cargarMetadata();
	//crearFileSystem();
	mapBitMap();
	actualizarBloquesLibres();

}

//funcion auxiliar para la creacion de el fileSystem Limpio
void crearFileSystem() {

	FILE * nuevoArchivoBitMAp = fopen(filesystem_paths->pathArchivoBitMap,
			"wb");

	int i;
	int cantidadDeCharsAEscribir = filesystem_metadata->cantidadBloques / 8;
	for (i = 0; i < cantidadDeCharsAEscribir; i++) {

		fwrite("A", 1, 1, nuevoArchivoBitMAp);
	}
	fclose(nuevoArchivoBitMAp);

	char * NUEVO_BITMAP;

	int fd;
	struct stat s;
	int status;

	fd = open(filesystem_paths->pathArchivoBitMap, O_RDWR);

	status = fstat(fd, &s);
	sizeBitMap = s.st_size;

	NUEVO_BITMAP = mmap(0, sizeBitMap, PROT_READ | PROT_WRITE, MAP_SHARED, fd,
			0);

	int j;

	for (j = 0; j < cantidadDeCharsAEscribir; j++) {

		int numeroDeBitDentroDelBloque;
		for (numeroDeBitDentroDelBloque = 0; numeroDeBitDentroDelBloque <= 7;
				numeroDeBitDentroDelBloque++) {

			unsigned char byte = NUEVO_BITMAP[j];
			unsigned char factor = 128;
			factor = factor >> numeroDeBitDentroDelBloque;
			byte = byte & ~factor;
			NUEVO_BITMAP[j] = byte;
		}
	}
	munmap(NUEVO_BITMAP, s.st_size);

	mkdir(filesystem_paths->pathDirectorioArchivos, S_IRWXU);
	mkdir(filesystem_paths->pathDirectorioBloques, S_IRWXU);

	int k;
	for (k = 0; k < filesystem_metadata->cantidadBloques; k++) {

		char * pathBloque = string_new();
		string_append(&pathBloque, filesystem_paths->pathDirectorioBloques);
		string_append(&pathBloque, itoa(k));
		string_append(&pathBloque, ".bin");

		FILE * nuevoBloque = fopen(pathBloque, "wb");
		fclose(nuevoBloque);

	}

}

void testBitMap() {
	marcarBloqueBitMapUsado(0);
	marcarBloqueBitMapUsado(1);
	marcarBloqueBitMapUsado(2);
	marcarBloqueBitMapUsado(3);
	marcarBloqueBitMapUsado(4);
	marcarBloqueBitMapUsado(5);
	marcarBloqueBitMapUsado(6);
	marcarBloqueBitMapUsado(7);

	marcarBloqueBitMapUsado(8);
	marcarBloqueBitMapUsado(9);
	marcarBloqueBitMapUsado(10);
	marcarBloqueBitMapUsado(11);
	marcarBloqueBitMapUsado(12);
	marcarBloqueBitMapUsado(13);
	marcarBloqueBitMapUsado(14);
	marcarBloqueBitMapUsado(15);

	int bloqueLibre = buscarPrimerBloqueLibre();

}

void testEscrituraYLecturaArchivo() {
	char * textoPrueba =
			"Se basa en crear un arbol binario completo, que representa la codificacion de los mensajes de la fuente, en el que cada nodo intermetio es menor que sus hijos";
	int32_t size = string_length(textoPrueba);
	int offset = 123;
	char * path = "/directorio1/directorio2/directorio3/directorio4/archivo.bin";

	escribirDatos(path, offset, size, textoPrueba);

	t_archivo_metadata * metadata = leerArchivoMetadata(path);

	char * leido = leerArchivoFs(metadata, offset, size);

}

//todo Desmapear bitMap cuando se cierra
void mapBitMap() {

	int fd;
	struct stat s;
	int status;

	fd = open(filesystem_paths->pathArchivoBitMap, O_RDWR);

	status = fstat(fd, &s);
	sizeBitMap = s.st_size;

	BIT_MAP = mmap(0, sizeBitMap, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	//todo acordarse de sincronizar siempre que se cambia algo
	if (msync(BIT_MAP, sizeBitMap, MS_SYNC) == -1) {
		perror(
				"Could not sync tvoid cargarDatosDeAccesoRapidoFileSystem()he file to disk");
	}
	log_info(infoLogger, "BitMap mapeado con exito en memoria (mmap)");

	//DESMAPEAR CUANDO SE CIERRA
	/*if (munmap(BIT_MAP, sizeBitMap) == -1) {
	 close(fd);
	 perror("Error desmapeando");
	 exit(EXIT_FAILURE);
	 }
	 */

}

void cargarPathsFileSystem() {
	log_info(infoLogger, "Cargando paths FileSystem");
	filesystem_paths = malloc(sizeof(t_filesystem_paths));
	filesystem_paths->pathDirectorioArchivos = string_new();
	filesystem_paths->pathDirectorioBloques = string_new();
	filesystem_paths->pathDirectorioMetadata = string_new();
	filesystem_paths->pathArchivoMetadata = string_new();
	filesystem_paths->pathArchivoBitMap = string_new();

	//Directorio archivos
	string_append(&(filesystem_paths->pathDirectorioArchivos),
			filesystem_config->puntoMontaje);
	string_append(&(filesystem_paths->pathDirectorioArchivos), "Archivos/");

	//Directorio bloques
	string_append(&(filesystem_paths->pathDirectorioBloques),
			filesystem_config->puntoMontaje);
	string_append(&(filesystem_paths->pathDirectorioBloques), "Bloques/");

	//Directorio metadata
	string_append(&(filesystem_paths->pathDirectorioMetadata),
			filesystem_config->puntoMontaje);
	string_append(&(filesystem_paths->pathDirectorioMetadata), "Metadata/");

	//Directorio archivo metadata
	string_append(&(filesystem_paths->pathArchivoMetadata),
			filesystem_paths->pathDirectorioMetadata);
	string_append(&(filesystem_paths->pathArchivoMetadata), "Metadata.bin");

	//Directorio archivo bitMap
	string_append(&(filesystem_paths->pathArchivoBitMap),
			filesystem_paths->pathDirectorioMetadata);
	string_append(&(filesystem_paths->pathArchivoBitMap), "Bitmap.bin");

	log_info(infoLogger, "Paths cargados exitosamente");

}

void abrirFicheroBitMap() {
	ficheroBitMap = fopen(filesystem_paths->pathArchivoBitMap, "r+w");
}

//TODO preguntar si esta bien como levante el archivo binario
void cargarMetadata() {

	log_info(infoLogger, "Cargando Metadata FileSystem");

	filesystem_metadata = malloc(sizeof(t_filesystem_metadata));
	filesystem_metadata->magicNumber = string_new();

	char * tamanioBloque = string_new();
	char * cantidadBloques = string_new();
	char * magicNumber = string_new();

	t_config* config = config_create(filesystem_paths->pathArchivoMetadata);

	string_append(&tamanioBloque,
			config_get_string_value(config, "TAMANIO_BLOQUES"));
	string_append(&cantidadBloques,
			config_get_string_value(config, "CANTIDAD_BLOQUES"));
	string_append(&magicNumber,
			config_get_string_value(config, "MAGIC_NUMBER"));

	filesystem_metadata->cantidadBloques = atoi(cantidadBloques);
	filesystem_metadata->tamanioBLoques = atoi(tamanioBloque);
	filesystem_metadata->magicNumber = magicNumber;

	log_info(infoLogger, "Metadata FileSystem cargada correctamente");

}

void * servidorFileSystem(int socket) {

	PROTOCOLO_KERNEL_A_FS tipoMensajeRecibido;

	librocket_recibirMensaje(socket, &tipoMensajeRecibido,
			sizeof(PROTOCOLO_KERNEL_A_FS), infoLogger);

	if (tipoMensajeRecibido == HANDSHAKE_CONECTAR_A_FS) {
		//printf("Se conecto el kernel \n");
		//acepto la conexion del kernel
		PROTOCOLO_FS_A_KERNEL conexionAceptada =
				HANDSHAKE_CONEXION_ACEPTADA_AL_KERNEL;
		librocket_enviarMensaje(socket, &conexionAceptada,
				sizeof(PROTOCOLO_FS_A_KERNEL), infoLogger);
		escucharMensajesDelKernel(socket);

	} else {
		printf("fallo handhsake con kernel \n");
		fflush(stdout);
	}

	return NULL;
}

void escucharMensajesDelKernel(int socket) {

	PROTOCOLO_KERNEL_A_FS mensaje;

	while (1) {
		int result = librocket_recibirMensaje(socket, &mensaje,
				sizeof(PROTOCOLO_KERNEL_A_FS), infoLogger);

		if (result == 0) {
			printf("Se desconecto el kernel \n");
			return;
		}
		interpreteMensajes(mensaje, socket);

	}
}

void cargarConfig(char** argv) {

	log_info(infoLogger, "Cargando config FileSystem");
	cantidadBloquesLibres = 0;

	filesystem_config = malloc(sizeof(t_filesystem_config));
	filesystem_config->puntoMontaje = string_new();
	filesystem_config->puerto = string_new();

	char* path = string_new();
	string_append(&path, argv[1]);

	t_config* config = config_create(path);

	string_append(&(filesystem_config->puntoMontaje),
			config_get_string_value(config, "PUNTO_MONTAJE"));

	string_append(&(filesystem_config->puerto),
			config_get_string_value(config, "PUERTO"));

	printf("Punto de montaje: %s\n", filesystem_config->puntoMontaje);
	printf("Puerto: %s\n", filesystem_config->puerto);

	free(path);
	config_destroy(config);

	log_info(infoLogger, "Config cargada exitosamente");
}

void liberarMemoria() {

	free(filesystem_config->puntoMontaje);
	free(filesystem_config);
	log_destroy(infoLogger);

}

void interpreteMensajes(PROTOCOLO_KERNEL_A_FS protocolo, int32_t sockFd) {

	switch (protocolo) {
	case VALIDAR_ARCHIVO:
		validarArchivo(sockFd);
		break;
	case CREAR_ARCHIVO:
		crearArchivo(sockFd);
		break;
	case BORRAR_ARCHIVO:
		borrarArchivo(sockFd);
		break;
	case OBTENER_DATOS:
		obtenerDatos(sockFd);
		break;
	case GUARDAR_DATOS:
		guardarDatos(sockFd);
		break;
	}

}

void guardarDatos(int32_t sockFd) {
	log_info(infoLogger, "Solicitud de escribirArchivo socket %d", sockFd);

	//Path, Offset, Size, Buffer

	int32_t pathSize;
	char * pathBuffer;
	int32_t offset;
	int32_t size;
	char* buffer;

	//Se obtiene el size del path
	librocket_recibirMensaje(sockFd, &pathSize, sizeof(int32_t), infoLogger);
	pathBuffer = malloc(sizeof(char) * pathSize);

	//Se obtiene el path
	librocket_recibirMensaje(sockFd, pathBuffer, pathSize, infoLogger);

	//recibo offset
	librocket_recibirMensaje(sockFd, &offset, sizeof(int32_t), infoLogger);

	//recibo sizeAEscribir
	librocket_recibirMensaje(sockFd, &size, sizeof(int32_t), infoLogger);

	//bufferAEscribir
	buffer = malloc(sizeof(char) * size);
	librocket_recibirMensaje(sockFd, buffer, size, infoLogger);

	PROTOCOLO_FS_A_KERNEL respuesta = escribirDatos(pathBuffer, offset, size,
			buffer);

	librocket_enviarMensaje(sockFd, &respuesta, sizeof(PROTOCOLO_FS_A_KERNEL),
			infoLogger);

}

void obtenerDatos(int32_t sockFd) {
	log_info(infoLogger, "Solicitud de leerArchivo socket %d", sockFd);

	int32_t pathSize;
	char * pathBuffer;

//Se obtiene el size del path
	librocket_recibirMensaje(sockFd, &pathSize, sizeof(int32_t), infoLogger);
	pathBuffer = malloc(sizeof(char) * pathSize);

//Se obtiene el path
	librocket_recibirMensaje(sockFd, pathBuffer, pathSize, infoLogger);

//recibo offset
	int32_t offset;
	librocket_recibirMensaje(sockFd, &offset, sizeof(int32_t), infoLogger);

//recibo sizeQueQuieroLeerDesdeElOffset
	int32_t size;
	librocket_recibirMensaje(sockFd, &size, sizeof(int32_t), infoLogger);

	log_info(infoLogger,
			"Se intentan leer size %d en el offset %d del archivo %s", size,
			offset, pathBuffer);

	t_archivo_metadata * metadataArchivo = leerArchivoMetadata(pathBuffer);

	if (metadataArchivo != NULL
			&& offset + size <= metadataArchivo->tamanioArchivo) {
		char * bytesLeidos = leerArchivoFs(metadataArchivo, offset, size);

		log_info(infoLogger,
				"Se leyereron  bytes:%d en el offset: %d del archivo %s, el contenido es &s",
				size, offset, pathBuffer, bytesLeidos);

		PROTOCOLO_FS_A_KERNEL mensaje = ENVIO_DATOS;
		librocket_enviarMensaje(sockFd, &mensaje, sizeof(PROTOCOLO_FS_A_KERNEL),
				infoLogger);

		librocket_enviarMensaje(sockFd, &size, sizeof(int32_t), infoLogger);

		librocket_enviarMensaje(sockFd, bytesLeidos, size, infoLogger);

	} else {
		PROTOCOLO_FS_A_KERNEL mensaje = FALLO_DATOS;
		librocket_enviarMensaje(sockFd, &mensaje, sizeof(mensaje), infoLogger);
	}

}

void borrarArchivo(int32_t sockFd) {
	log_info(infoLogger, "Solicitud de borrarArchivo socket %d", sockFd);

	int32_t pathSize;
	char * pathBuffer;

//Se obtiene el size del path
	librocket_recibirMensaje(sockFd, &pathSize, sizeof(int32_t), infoLogger);
	pathBuffer = malloc(sizeof(char) * pathSize);

//Se obtiene el path
	librocket_recibirMensaje(sockFd, pathBuffer, pathSize, infoLogger);

	marcarBloquesDeArchivoMetadataDelArchivoComoLibres(pathBuffer);
	int respuesta = borrarRecursivo(pathBuffer);

	if (!respuesta) {
		PROTOCOLO_FS_A_KERNEL respuesta = ARCHIVO_BORRADO_OK;
		librocket_enviarMensaje(sockFd, &respuesta,
				sizeof(PROTOCOLO_FS_A_KERNEL), infoLogger);
		log_info(infoLogger, "Archivo borrado con exito %s", pathBuffer);

	} else {
		PROTOCOLO_FS_A_KERNEL respuesta = ARCHIVO_BORRADO_ERROR;
		librocket_enviarMensaje(sockFd, &respuesta,
				sizeof(PROTOCOLO_FS_A_KERNEL), infoLogger);
		log_info(infoLogger, "Fallo en el borradod el archivo %s", pathBuffer);

	}

}

/*Devuelve  EXISTE_ARCHIVO en caso de que exista el archivo
 *   		NO_EXISTE_ARCHIVO en caso contrario
 */
void validarArchivo(int32_t sockFd) {

	log_info(infoLogger, "Solicitud de validarArchivo socket %d", sockFd);

	int32_t pathSize;
	char * pathBuffer;

//Se obtiene el size del path
	librocket_recibirMensaje(sockFd, &pathSize, sizeof(int32_t), infoLogger);
	pathBuffer = malloc(sizeof(char) * pathSize);

//Se obtiene el path
	librocket_recibirMensaje(sockFd, pathBuffer, pathSize, infoLogger);

	log_info(infoLogger, "Solicitud de validarArchivo path  %s", pathBuffer);

	PROTOCOLO_FS_A_KERNEL response = validarExistenciaArchivo(pathBuffer);

	librocket_enviarMensaje(sockFd, &response, sizeof(int32_t), infoLogger);
}

/*Devuelve ARCHIVO_CREADO_CON_EXITOS si pudo crear
 *    	   FALLO_EN_LA_CREACION_DE_ARCHIVO DE ARCHIVO en caso contrario.
 */
void crearArchivo(int32_t sockFd) {

	log_info(infoLogger, "Solicitud de crearArchivo socket %d", sockFd);

	int32_t pathSize;
	char * pathBuffer;
	PROTOCOLO_FS_A_KERNEL mensaje;

//Se obtiene el size del path
	librocket_recibirMensaje(sockFd, &pathSize, sizeof(int32_t), infoLogger);
	pathBuffer = malloc(sizeof(char) * pathSize);

//Se obtiene el path
	librocket_recibirMensaje(sockFd, pathBuffer, pathSize, infoLogger);

	if (NO_EXISTE_ARCHIVO == validarExistenciaArchivo(pathBuffer)
			&& cantidadBloquesLibres >= 1) {

		int32_t bloqueAAsignar = buscarPrimerBloqueLibre();
		if (bloqueAAsignar >= 0) {
			crearFileMetadata(bloqueAAsignar, pathBuffer);
			marcarBloqueBitMapUsado(bloqueAAsignar);

			log_info(infoLogger, "archivo %s creado con exito", pathBuffer);
			mensaje = ARCHIVO_CREADO_CON_EXITO;
			librocket_enviarMensaje(sockFd, &mensaje, sizeof(mensaje),
					infoLogger);
		} else {
			mensaje = FALLO_EN_LA_CREACION_DE_ARCHIVO;
			log_info(infoLogger, "Fallo en la creaciond de archivo &s",
					pathBuffer);

			librocket_enviarMensaje(sockFd, &mensaje, sizeof(mensaje),
					infoLogger);

		}
	} else {
		mensaje = FALLO_EN_LA_CREACION_DE_ARCHIVO;
		log_info(infoLogger, "Fallo en la creaciond de archivo &s", pathBuffer);

		librocket_enviarMensaje(sockFd, &mensaje, sizeof(mensaje), infoLogger);
	}
}


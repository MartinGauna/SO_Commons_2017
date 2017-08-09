#include "headers/logicaFileSystem.h"

//====CREAR====

void crearFileMetadata(int32_t bloqueAAsignar, char * pathBuffer) {

	char** pathParts = string_split(pathBuffer, "/");

	int i = 0;

//contar cantidad de directorios
	int32_t directorios = 0;
	char * palabra = pathParts[i];
	while (palabra != NULL) {
		directorios++;
		i++;
		palabra = pathParts[i];
	}

//Creando estructura de directorios
	int j;
	int r = i - 1;

	char * pathDirectorio = string_new();

	string_append(&pathDirectorio, filesystem_paths->pathDirectorioArchivos);
	for (j = 0; j < r; j++) {
		string_append(&pathDirectorio, pathParts[j]);
		mkdir(pathDirectorio, S_IRWXU);
		string_append(&pathDirectorio, "/");
	}
	string_append(&pathDirectorio, pathParts[j]);

	//Creando archivo
	crearArchivoMetadataDeUnArchivo(pathDirectorio, bloqueAAsignar);

	log_info(infoLogger, "Archivo de metadata %s creado con exito",
			pathDirectorio);
}

void crearArchivoMetadataDeUnArchivo(char * pathDirectorio,
		int32_t bloqueAAsignar) {

	FILE * nuevoArchivoMetadata = fopen(pathDirectorio, "w");
	char * stringDeBloques = string_new();
	string_append(&stringDeBloques, "BLOQUES=[");
	string_append(&stringDeBloques, itoa(bloqueAAsignar));
	string_append(&stringDeBloques, "]");

	int tamanioArchivo = filesystem_metadata->tamanioBLoques;

	char * size = string_new();
	string_append(&size, "TAMANIO=");
	string_append(&size, itoa(tamanioArchivo));
	string_append(&size, "\n");

	fwrite(size, string_length(size), 1, nuevoArchivoMetadata);
	fwrite(stringDeBloques, string_length(stringDeBloques), 1,
			nuevoArchivoMetadata);

	fclose(nuevoArchivoMetadata);

	marcarBloqueBitMapUsado(bloqueAAsignar);

}

//=====BORRAR===

int borrarRecursivo(char * pathRelativoArchivo) {
	marcarBloquesDeArchivoMetadataDelArchivoComoLibres(pathRelativoArchivo);
	char * pathArchivo = obtenerDireccionGlobalArchivoBin(pathRelativoArchivo);
	return remove(pathArchivo);
}

//====LEER====

//TODO VERIFICAR QUE LEE DE MAS
char * leerArchivoFs(t_archivo_metadata * metadataArchivo, int32_t offset,
		int32_t size) {

	int i, byte_inicial;

	int bloque_de_lectura_inicial = redondearDivisionPaArriba(offset,
			filesystem_metadata->tamanioBLoques);
	int ultimo_bloque_permitido = list_size(metadataArchivo->bloques);

	if (bloque_de_lectura_inicial == 0) {
		i = bloque_de_lectura_inicial;
		byte_inicial = offset
				- ((bloque_de_lectura_inicial)
						* filesystem_metadata->tamanioBLoques);
	} else {
		i = bloque_de_lectura_inicial - 1;
		byte_inicial = offset
				- ((bloque_de_lectura_inicial - 1)
						* filesystem_metadata->tamanioBLoques);
	}

	int real_size = size;

	char * buffer;
	char * lectura = string_new();

	FILE * ARCHIVO_BLOQUE_ACTUAL;
	char * stringParaLeerExacto;

	while (i <= ultimo_bloque_permitido) {

		if (real_size == 0)
			return lectura;

		char * pathBloque = string_new();
		string_append(&pathBloque, filesystem_paths->pathDirectorioBloques);
		string_append(&pathBloque, list_get(metadataArchivo->bloques, i));
		string_append(&pathBloque, ".bin");

		ARCHIVO_BLOQUE_ACTUAL = fopen(pathBloque, "rb+");

		fseek(ARCHIVO_BLOQUE_ACTUAL, byte_inicial, SEEK_SET);

		if (byte_inicial + real_size > filesystem_metadata->tamanioBLoques) {
			int bytes_a_leer = filesystem_metadata->tamanioBLoques
					- byte_inicial;
			buffer = malloc(sizeof(char) * bytes_a_leer);
			int result = fread(buffer, 1, bytes_a_leer, ARCHIVO_BLOQUE_ACTUAL);
			stringParaLeerExacto = string_substring(buffer, 0, bytes_a_leer);
			real_size -= bytes_a_leer;

		} else {
			buffer = malloc(sizeof(char) * real_size);
			int result = fread(buffer, 1, real_size, ARCHIVO_BLOQUE_ACTUAL);
			stringParaLeerExacto = string_substring(buffer, 0, real_size);
			i = ultimo_bloque_permitido;
		}

		string_append(&lectura, stringParaLeerExacto);
		free(buffer);
		i++;
		byte_inicial = 0;

	}
	return lectura;
}

//====ESCRIBIR====

PROTOCOLO_FS_A_KERNEL escribirDatos(char *pathBuffer, int32_t offset,
		int32_t size, char * buffer) {

	t_archivo_metadata * metadataArchivo = leerArchivoMetadata(pathBuffer);

	int cantidad_bloques_que_tiene = list_size(metadataArchivo->bloques);

	int bloques_que_necesita_en_total = redondearDivisionPaArriba(offset + size,
			filesystem_metadata->tamanioBLoques);

	int cantidad_bloques_nuevos = bloques_que_necesita_en_total
			- cantidad_bloques_que_tiene;
	actualizarBloquesLibres();

	if (cantidadBloquesLibres < cantidad_bloques_nuevos) {
		log_info(infoLogger, "fallo estitura del archivo %s",pathBuffer);

		return ESCRIBIR_FALLO;
	}

	if (cantidad_bloques_que_tiene < bloques_que_necesita_en_total) {

		asignar_nuevos_bloques(metadataArchivo, cantidad_bloques_nuevos,
				pathBuffer);
		escribirDatosEnBloques(metadataArchivo, offset, size, buffer);

	} else {

		escribirDatosEnBloques(metadataArchivo, offset, size, buffer);

	}

	log_info(infoLogger, "Se escribieron %d bytes, en el offset %d del archivo %s",size,offset,pathBuffer);

	return ESCRIBIR_OK;
}

void escribirDatosEnBloques(t_archivo_metadata * metadataArchivo,
		int32_t offset, int32_t size, char * buffer) {

	int byteDentroDelBloqueDondeComenzarAEscribir, i;

	int indiceBloqueDondeComenzarAEscribir = redondearDivisionPaArriba(offset,
			filesystem_metadata->tamanioBLoques);

	if (indiceBloqueDondeComenzarAEscribir == 0) {
		byteDentroDelBloqueDondeComenzarAEscribir = offset;
		i = indiceBloqueDondeComenzarAEscribir;
	} else {
		byteDentroDelBloqueDondeComenzarAEscribir = offset
				- (filesystem_metadata->tamanioBLoques
						* (indiceBloqueDondeComenzarAEscribir - 1));
		i = indiceBloqueDondeComenzarAEscribir - 1;
	}

	int bytes_restantes = size;
	int start = 0;

	FILE * BLOQUE_A_ESCRIBIR;

	while (bytes_restantes > 0) {
		char * numeroDeBloqueAEscribir = string_new();
		;
		string_append(&numeroDeBloqueAEscribir,
				list_get(metadataArchivo->bloques, i));

		char * pathBloque = string_new();
		string_append(&pathBloque, filesystem_paths->pathDirectorioBloques);
		string_append(&pathBloque, numeroDeBloqueAEscribir);
		string_append(&pathBloque, ".bin");

		char * bufer_escritura;

		BLOQUE_A_ESCRIBIR = fopen(pathBloque, "rb+");

		fseek(BLOQUE_A_ESCRIBIR, byteDentroDelBloqueDondeComenzarAEscribir,
		SEEK_SET);

		if (byteDentroDelBloqueDondeComenzarAEscribir + bytes_restantes
				>= filesystem_metadata->tamanioBLoques) {

			int bytes_a_escribir = filesystem_metadata->tamanioBLoques
					- byteDentroDelBloqueDondeComenzarAEscribir;
			bufer_escritura = malloc(sizeof(char) * bytes_a_escribir);
			bufer_escritura = string_substring(buffer, start,
					(start + bytes_a_escribir));

			fwrite(bufer_escritura, 1, bytes_a_escribir, BLOQUE_A_ESCRIBIR);

			bytes_restantes -= bytes_a_escribir;
			start += bytes_a_escribir;

		} else {
			bufer_escritura = malloc(
					sizeof(char) * (start - (start + bytes_restantes)));
			bufer_escritura = string_substring(buffer, start,
					(start + bytes_restantes));

			fwrite(bufer_escritura, 1, bytes_restantes, BLOQUE_A_ESCRIBIR);

			bytes_restantes -= bytes_restantes;
		}

		fclose(BLOQUE_A_ESCRIBIR);
		i++;
		byteDentroDelBloqueDondeComenzarAEscribir = 0;
	}

}

//====VALIDAR====

/*
 * Recibe el pathd de una archivo en forma local, lo transforma al path en el punto de montaje
 * Devuelve EXISTE_ARCHIVO en caso de que exista el archivo
 * 			NO_EXISTE_ARCHIVO en caso contrario
 */
PROTOCOLO_FS_A_KERNEL validarExistenciaArchivo(char * pathRelativoArchivo) {
	char * pathArchivo = obtenerDireccionGlobalArchivoBin(pathRelativoArchivo);
	FILE * archivo = fopen(pathArchivo, "r");

	return archivo != NULL ? EXISTE_ARCHIVO : NO_EXISTE_ARCHIVO;
}

//====OPERACIONES BITMAP====

//todo que pasa si no hay bloques libres??????
int32_t buscarPrimerBloqueLibre() {

	log_info(infoLogger, "Buscando primer Bloque libre");

	int flag = 0;
	unsigned char factor = 128;
	char factorCorrido;
	char charDeOchoBloques;
	int numeroDeBloque;
	int numeroDeBit;
	for (numeroDeBloque = 0; numeroDeBloque < sizeBitMap; numeroDeBloque++) {
		charDeOchoBloques = BIT_MAP[numeroDeBloque];

		for (numeroDeBit = 0; numeroDeBit < 8; numeroDeBit++) {
			factorCorrido = factor >> numeroDeBit;
			char charModificado = charDeOchoBloques | factorCorrido;
			int a = (int) charModificado;
			int b = (int) charDeOchoBloques;
			if (a != b) {
				flag = 1;
				break;
			}
		}
		if (flag == 1) {
			break;
		}

	}

	if (((numeroDeBloque - 1) * 8 + numeroDeBit)
			== filesystem_metadata->cantidadBloques) {
		factorCorrido = factor >> numeroDeBit;
		charDeOchoBloques = BIT_MAP[numeroDeBloque];
		char charModificado = charDeOchoBloques | factorCorrido;
		int a = (int) charModificado;
		int b = (int) charDeOchoBloques;
		if (a == b) {
			return -1;
		}
	}

	return (int32_t) ((numeroDeBloque) * 8) + numeroDeBit;
}

//cambie int a int32_t probar esa kk
void marcarBloqueBitMapLibre(int32_t bloque) {
	//parte entera
	int32_t numeroDeByteEnElQueSeEncuentraElBitAModificar = bloque / 8;
	int32_t numeroDeBitDentroDelBloque;

	if (numeroDeByteEnElQueSeEncuentraElBitAModificar == 0) {
		numeroDeBitDentroDelBloque = bloque % 8;
	} else {
		numeroDeBitDentroDelBloque = bloque
				% (numeroDeByteEnElQueSeEncuentraElBitAModificar * 8);
	}

	unsigned char byteAModificar =
			BIT_MAP[numeroDeByteEnElQueSeEncuentraElBitAModificar];
	unsigned char factor = 128;
	factor = factor >> numeroDeBitDentroDelBloque;
	byteAModificar = byteAModificar & ~factor;

	BIT_MAP[numeroDeByteEnElQueSeEncuentraElBitAModificar] = byteAModificar;

	if (msync(BIT_MAP, sizeBitMap, MS_SYNC) == -1) {
		perror("Could not sync the file to disk");
	}
}

//cambie int a int32_t probar esa kk
void marcarBloqueBitMapUsado(int32_t bloque) {
	//parte entera
	int32_t numeroDeByteEnElQueSeEncuentraElBitAModificar = bloque / 8;
	int32_t numeroDeBitDentroDelBloque;

	if (numeroDeByteEnElQueSeEncuentraElBitAModificar == 0) {
		numeroDeBitDentroDelBloque = bloque % 8;
	} else {
		numeroDeBitDentroDelBloque = bloque
				% (numeroDeByteEnElQueSeEncuentraElBitAModificar * 8);
	}

	unsigned char byteAModificar =
			BIT_MAP[numeroDeByteEnElQueSeEncuentraElBitAModificar];
	unsigned char factor = 128;
	factor = factor >> numeroDeBitDentroDelBloque;
	byteAModificar = byteAModificar | factor;

	BIT_MAP[numeroDeByteEnElQueSeEncuentraElBitAModificar] = byteAModificar;

	if (msync(BIT_MAP, sizeBitMap, MS_SYNC) == -1) {
		perror("Could not sync the file to disk");
	}
}

void actualizarBloquesLibres() {

	unsigned char factor = 128;
	char factorCorrido;
	char charDeOchoBloques;
	int numeroDeBloque;
	int numeroDeBit;
	int contadorBloquesLibres = 0;
	for (numeroDeBloque = 0; numeroDeBloque < sizeBitMap; numeroDeBloque++) {
		charDeOchoBloques = BIT_MAP[numeroDeBloque];

		for (numeroDeBit = 0; numeroDeBit < 8; numeroDeBit++) {
			factorCorrido = factor >> numeroDeBit;
			char charModificado = charDeOchoBloques | factorCorrido;
			int a = (int) charModificado;
			int b = (int) charDeOchoBloques;
			if (a != b) {
				contadorBloquesLibres++;
			}
		}

	}

	cantidadBloquesLibres = contadorBloquesLibres;

}

//====OPERACIONES ARCHIVOS METADATA====

void marcarBloquesDeArchivoMetadataDelArchivoComoLibres(
		char * pathRelativoArchivo) {

	t_archivo_metadata * metadataArchivo = leerArchivoMetadata(
			pathRelativoArchivo);

	int cantidadBloques = list_size(metadataArchivo->bloques);

	int i;
	for (i = 0; i < cantidadBloques; i++) {
		char * bloqueAMarcarComoLibre = malloc(2);
		bloqueAMarcarComoLibre = list_get(metadataArchivo->bloques, i);
		int bloque = atoi(bloqueAMarcarComoLibre);
		marcarBloqueBitMapLibre(bloque);
	}

}

void actualizarArchivoMetadata(t_list * bloques, char * path) {
	char** pathParts = string_split(path, "/");

	int i = 0;

	//contar cantidad de directorios
	int32_t directorios = 0;
	char * palabra = pathParts[i];
	while (palabra != NULL) {
		directorios++;
		i++;
		palabra = pathParts[i];
	}

	//Creando estructura de directorios
	int j;
	int r = i - 1;

	char * pathDirectorio = string_new();

	string_append(&pathDirectorio, filesystem_paths->pathDirectorioArchivos);
	for (j = 0; j < r; j++) {
		string_append(&pathDirectorio, pathParts[j]);
		mkdir(pathDirectorio, S_IRWXU);
		string_append(&pathDirectorio, "/");
	}
	string_append(&pathDirectorio, pathParts[j]);

	FILE * nuevoArchivoMetadata = fopen(pathDirectorio, "w");
	char * stringDeBloques = string_new();
	string_append(&stringDeBloques, "BLOQUES=[");

	int z;
	int cantidadBloques = list_size(bloques);

	for (z = 0; z < cantidadBloques; z++) {
		string_append(&stringDeBloques, list_get(bloques, z));
		if (z != (cantidadBloques - 1)) {
			string_append(&stringDeBloques, ",");
		}
	}
	string_append(&stringDeBloques, "]");

	int tamanioArchivo = filesystem_metadata->tamanioBLoques * cantidadBloques;

	char * size = string_new();
	string_append(&size, "TAMANIO=");
	string_append(&size, itoa(tamanioArchivo));
	string_append(&size, "\n");

	fwrite(size, string_length(size), 1, nuevoArchivoMetadata);
	fwrite(stringDeBloques, string_length(stringDeBloques), 1,
			nuevoArchivoMetadata);

	fclose(nuevoArchivoMetadata);

}

void asignar_nuevos_bloques(t_archivo_metadata * metadataArchivo,
		int cantidad_bloques_nuevos, char * path) {

	int32_t bloqueLibre;
	int i;
	for (i = 0; i < cantidad_bloques_nuevos; i++) {
		bloqueLibre = buscarPrimerBloqueLibre();
		marcarBloqueBitMapUsado(bloqueLibre);
		list_add(metadataArchivo->bloques, itoa(bloqueLibre));
	}
	actualizarArchivoMetadata(metadataArchivo->bloques, path);
}

/*
 * 	Recibe un path Relativo,y devuelve la estructura de la metadata del archivo, tamanio y bloques
 *
 * 	//TODO
 * 	Se deberia validar que es un path correcto, adentro tiene la validacion, es redundante.
 */
t_archivo_metadata * leerArchivoMetadata(char * pathRelativoArchivo) {

	log_info(infoLogger, "Leyendo archivo metadata de archivo %s",pathRelativoArchivo);

	t_archivo_metadata * archivoMetadata = malloc(sizeof(t_archivo_metadata));
	char * tamanioArchivo = string_new();
	if (validarExistenciaArchivo(pathRelativoArchivo)) {
		char * pathArchivo = obtenerDireccionGlobalArchivoBin(
				pathRelativoArchivo);

		t_config* config = config_create(pathArchivo);

		string_append(&tamanioArchivo,
				config_get_string_value(config, "TAMANIO"));

		char ** bloques = config_get_array_value(config, "BLOQUES");

		archivoMetadata->tamanioArchivo = atoi(tamanioArchivo);
		archivoMetadata->bloques = list_create();

		char * bloque;
		int i;
		for (i = 0;; i++) {
			bloque = bloques[i];
			if (bloque != NULL) {
				list_add(archivoMetadata->bloques, bloque);
			} else {
				break;
			}
		}

	}

	return archivoMetadata;
}

//====UTILS====

/*
 *  Recibe como parametro el path del archivo de metadata en forma local, y lo transforma
 *  a un path en la estructura de archivos
 */

char * obtenerDireccionGlobalArchivoBin(char * pathRelativoArchivo) {

	char * patGlobalArchivo = string_new();
	if (string_substring(pathRelativoArchivo, 0, (1))[0] == '/') {
		string_append(&patGlobalArchivo,
				filesystem_paths->pathDirectorioArchivos);
		string_append(&patGlobalArchivo,
				string_substring(pathRelativoArchivo, 1,
						string_length(pathRelativoArchivo)));
	} else {
		string_append(&patGlobalArchivo,
				filesystem_paths->pathDirectorioArchivos);
		string_append(&patGlobalArchivo, pathRelativoArchivo);
	}
	return patGlobalArchivo;
}

int redondearDivisionPaArriba(int dividendo, int divisor) {
	if (divisor == 0) {
		perror("SE QUISO DIVIDIR POR CERO");
		exit(1);
	}
	unsigned int division = dividendo / divisor;
	if (dividendo % divisor > 0) {
		division += 1;
	}
	return division;
}


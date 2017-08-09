#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <commons/string.h>
#include <dirent.h>
#include <fcntl.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <sys/stat.h>
#include <protocolos/protocols.h>
#include <commons/config.h>
#include <sys/mman.h>
#include <structs/structs.h>

typedef struct {
	char* puntoMontaje;
	char* puerto;
} t_filesystem_config;

typedef struct {
	char* pathDirectorioMetadata;
	char* pathDirectorioBloques;
	char* pathDirectorioArchivos;
	char* pathArchivoMetadata;
	char* pathArchivoBitMap;
} t_filesystem_paths;

typedef struct {
	int32_t tamanioBLoques;
	int32_t cantidadBloques;
	char* magicNumber;
} t_filesystem_metadata;

typedef struct {
	int32_t tamanioArchivo;
	t_list* bloques;
} t_archivo_metadata;

int32_t cantidadBloquesLibres;
int sizeBitMap;
t_filesystem_metadata* filesystem_metadata;
t_filesystem_config* filesystem_config;
t_filesystem_paths * filesystem_paths;
char * BIT_MAP;
t_log* infoLogger;

FILE *ficheroBitMap;

char * obtenerDireccionGlobalArchivoBin(char *);

void marcarBloqueBitMapComoLibre();
void marcarBloqueBitMapLibre();
void marcarBloqueBitMapUsado();
void actualizarBloquesLibres();
void crearFileMetadata(int32_t, char *);
void crearArchivoMetadataDeUnArchivo(char *, int32_t);
void escribirDatosEnBloques(t_archivo_metadata *, int32_t, int32_t, char *);
void asignar_nuevos_bloques(t_archivo_metadata *, int, char *);
int borrarRecursivo(char *);
void interpreteMensajes(PROTOCOLO_KERNEL_A_FS, int32_t);
int redondearDivisionPaArriba(int, int);
void marcarBloquesDeArchivoMetadataDelArchivoComoLibres(char *);
void actualizarArchivoMetadata(t_list * bloques, char * path);
char * leerBloquesArchivo(t_archivo_metadata *, int32_t, int32_t);
char * leerArchivoFs(t_archivo_metadata * metadataArchivo, int32_t offset,
		int32_t size);
int buscarPrimerBloqueLibre();
PROTOCOLO_FS_A_KERNEL validarExistenciaArchivo(char*);
PROTOCOLO_FS_A_KERNEL escribirDatos(char *, int32_t, int32_t, char *);
t_archivo_metadata * leerArchivoMetadata(char *);

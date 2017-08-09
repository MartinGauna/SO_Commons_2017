#include "headers/tests.h"

void correrTests() {

	testCodigoLimpio();

}

void testCodigoLimpio() {

	t_programa* nuevoPrograma = malloc(sizeof(t_programa));

	nuevoPrograma->pid = 0;
	nuevoPrograma->tamanioCodigo = 0;
	nuevoPrograma->kernelSocket = -1;

	nuevoPrograma->codigo = string_new();
	nuevoPrograma->path = string_new();
	string_append(&(nuevoPrograma->path),
			"/home/utnso/Escritorio/tp-2017-1c-Equipo-Rocket/consola/file.txt");

	if (access(nuevoPrograma->path, F_OK) == -1) {
		perror("NO EXISTE EL ARCHIVO!!!!");
		return;
	}

	contarCaracteresYObtenerCodigo(nuevoPrograma);

	printf("%i\n", nuevoPrograma->tamanioCodigo);
	printf("%s\n", nuevoPrograma->codigo);

	t_metadata_program* metadata = metadata_desde_literal(
			nuevoPrograma->codigo);

	printf("Cantidad de etiquetas: %i\n", metadata->cantidad_de_etiquetas);
	printf("Cantidad de funciones: %i\n", metadata->cantidad_de_funciones);
	printf("Etiquetas serializado: %s\n", metadata->etiquetas);
	printf("Tamanio de etiquetas serializado: %i\n", metadata->etiquetas_size);
	printf("Puntero instruccion de inicio: %i\n", metadata->instruccion_inicio);

	int32_t cantidadDeInstrucciones = metadata->instrucciones_size, contador;
	for (contador = 0; contador < cantidadDeInstrucciones; contador++) {

		printf("Instruccion %i offset: %i\n", contador,
				metadata->instrucciones_serializado[contador].start);
		printf("Instruccion %i offset: %i\n", contador,
				metadata->instrucciones_serializado[contador].offset);

		int32_t indexInstruccion =
				metadata->instrucciones_serializado[contador].start, comienzo =
				metadata->instrucciones_serializado[contador].start, offset =
				metadata->instrucciones_serializado[contador].offset;

		printf("Instruccion: %i\n", contador);

		while (indexInstruccion < comienzo + offset) {

			printf("%c", nuevoPrograma->codigo[indexInstruccion]);
			indexInstruccion++;

		}

		printf("\n");

	}

	printf("Tamanio de instrucciones serializado: %i\n",
			metadata->instrucciones_size);

}

#include "fileManager.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>

int copy_file(char *old_filename, char *new_filename) {
	FILE * arch, *copia;

	arch = fopen(old_filename, "r");
	copia = fopen(new_filename, "w");

	if (arch < 0 || copia < 0) {
		perror("error al abrir los archivos a copiar");
		return -1;
	}

	char a;

	a = fgetc(arch);
	while (feof(arch) == 0) {
		fputc(a, copia);
		a = fgetc(arch);
	}
	fclose(arch);
	fclose(copia);

	return 0;
}

#include "time.h"

void* calcular(char* instanteInicio, char* instanteFin) {

	char* tiempoTranscurrido = string_new();

	char** tiempoInicioV = malloc(strlen("hh:mm:ss:mmmm"));
	char** tiempoFinV = malloc(strlen("hh:mm:ss:mmmm"));

	tiempoInicioV = string_split(instanteInicio, ":");
	tiempoFinV = string_split(instanteFin, ":");

	int indexTime = 0, inicio = 0, fin = 0, total = 0;
	char* totalString;

	while (indexTime < 4) {
		switch (indexTime) {
		case 0:
			inicio = atoi(tiempoInicioV[indexTime]);
			fin = atoi(tiempoFinV[indexTime]);
			total = fin - inicio;
			if (total < 0) {
				total = total + 24;
			} else if (total > 0) {
				total--;
			}
			totalString = string_itoa(total);
			string_append(&tiempoTranscurrido, totalString);
			string_append(&tiempoTranscurrido, ":");
			indexTime++;
			break;

		case 1 ... 2:
			inicio = atoi(tiempoInicioV[indexTime]);
			fin = atoi(tiempoFinV[indexTime]);
			total = fin - inicio;
			if (total < 0) {
				total = total + 60;
			} else if (total > 0) {
				total--;
			}
			totalString = string_itoa(total);
			string_append(&tiempoTranscurrido, totalString);
			string_append(&tiempoTranscurrido, ":");
			indexTime++;
			break;

		case 3:
			inicio = atoi(tiempoInicioV[indexTime]);
			fin = atoi(tiempoFinV[indexTime]);
			total = fin - inicio;
			if (total < 0) {
				total = total + 1000;
			}
			totalString = string_itoa(total);
			string_append(&tiempoTranscurrido, totalString);
			indexTime++;
			break;

		}
	}

	free(tiempoInicioV);
	free(tiempoFinV);

	return tiempoTranscurrido;

}

char* sumar(char* unTiempo, char* otroTiempo) {

	char* tiempoTotal = string_new();

	char** unTiempoV = malloc(strlen("hh:mm:ss:mmmm"));
	char** otroTiempoV = malloc(strlen("hh:mm:ss:mmmm"));

	unTiempoV = string_split(unTiempo, ":");
	otroTiempoV = string_split(otroTiempo, ":");

	int horasUnTiempo = atoi(unTiempoV[0]);
	int minutosUnTiempo = atoi(unTiempoV[1]);
	int segundosUnTiempo = atoi(unTiempoV[2]);
	int milisegundosUnTiempo = atoi(unTiempoV[3]);

	int horasOtroTiempo = atoi(otroTiempoV[0]);
	int minutosOtroTiempo = atoi(otroTiempoV[1]);
	int segundosOtroTiempo = atoi(otroTiempoV[2]);
	int milisegundosOtroTiempo = atoi(otroTiempoV[3]);

	int totalHoras = horasUnTiempo + horasOtroTiempo;
	int totalMinutos = minutosUnTiempo + minutosOtroTiempo;
	int totalSegundos = segundosUnTiempo + segundosOtroTiempo;
	int totalMilisegundos = milisegundosUnTiempo + milisegundosOtroTiempo;

	while (totalMilisegundos > 1000) {

		totalSegundos++;
		totalMilisegundos = totalMilisegundos - 1000;

	}

	while (totalSegundos > 59) {

		totalMinutos++;
		totalSegundos = totalSegundos - 60;

	}

	while (totalMinutos > 59) {

		totalHoras++;
		totalMinutos = totalMinutos - 60;

	}

	char* horasString = string_itoa(totalHoras);
	char* minutosString = string_itoa(totalMinutos);
	char* segundosString = string_itoa(totalSegundos);
	char* milisegundosString = string_itoa(totalMilisegundos);

	string_append(&tiempoTotal, horasString);
	string_append(&tiempoTotal, ":");
	string_append(&tiempoTotal, minutosString);
	string_append(&tiempoTotal, ":");
	string_append(&tiempoTotal, segundosString);
	string_append(&tiempoTotal, ":");
	string_append(&tiempoTotal, milisegundosString);

	return tiempoTotal;

}

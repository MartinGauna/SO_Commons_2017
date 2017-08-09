#include "headers/cpu.h"

int main(int argc, char* argv[]) {

	iniciarCpu(argc, argv);

	conectarseAMemoria();
	conectarseAlKernel();

	return EXIT_SUCCESS;

}

void iniciarCpu(int argc, char* argv[]) {

	infoLogger = log_create(LOG_PATH, "cpu", false, LOG_LEVEL_INFO);
	infoEjecucion = log_create(LOG_EJECUCION, "cpu", false, LOG_LEVEL_TRACE);

	if (argc < 2) {
		log_info(infoLogger,
				"No se recibio por parametro el path del config.txt.");
		printf("No se recibio por parametro el path del config.txt.");
		exit(EXIT_FAILURE);
	}

	cargarConfig(argv);

	inicializarPrimitivas();
	inicializarSeniales();
	inicializarSemaforos();

	desconexion = false;
	pcb_ejecutando = false;

	printf("CPU inicializado correctamente.\n");
	log_info(infoLogger, "CPU inicializado correctamente.");

}

void cargarConfig(char** argv) {

	cpu_config = malloc(sizeof(t_cpu_config));
	cpu_config->ipKernel = string_new();
	cpu_config->puertoKernel = string_new();
	cpu_config->ipMemoria = string_new();
	cpu_config->puertoMemoria = string_new();

	char* path = string_new();
	string_append(&path, argv[1]);

	t_config* config = config_create(path);

	string_append(&(cpu_config->ipKernel),
			config_get_string_value(config, "IP_KERNEL"));

	string_append(&(cpu_config->puertoKernel),
			config_get_string_value(config, "PUERTO_KERNEL"));

	string_append(&(cpu_config->puertoMemoria),
			config_get_string_value(config, "PUERTO_MEMORIA"));

	string_append(&(cpu_config->ipMemoria),
			config_get_string_value(config, "IP_MEMORIA"));

	free(path);
	config_destroy(config);
	logearConfig();
	imprimirConfig();

}

void logearConfig() {

	log_info(infoLogger, "IP del Kernel: %s.", cpu_config->ipKernel);
	log_info(infoLogger, "IP de la Memoria: %s.", cpu_config->ipMemoria);
	log_info(infoLogger, "Puerto del Kernel: %s.", cpu_config->puertoKernel);
	log_info(infoLogger, "Puerto de la Memoria: %s.",
			cpu_config->puertoMemoria);

}

void imprimirConfig() {

	printf("IP del Kernel: %s \n", cpu_config->ipKernel);
	printf("IP de la Memoria: %s \n", cpu_config->ipMemoria);
	printf("Puerto del Kernel: %s \n", cpu_config->puertoKernel);
	printf("Puerto de la Memoria: %s \n", cpu_config->puertoMemoria);

}

void liberarMemoria() {

	pthread_mutex_destroy(&semaforoDesconexion);

	free(cpu_config->ipMemoria);
	free(cpu_config->puertoMemoria);
	free(cpu_config->ipKernel);
	free(cpu_config->puertoKernel);
	free(cpu_config);

	log_destroy(infoLogger);
	log_destroy(infoEjecucion);

}

void destruirElemento(void* data) {
	free(data);
}

void destruir_pcb() {

	list_destroy_and_destroy_elements(pcbActual->indice_codigo,
			destruirElemento);
	list_destroy_and_destroy_elements(pcbActual->indice_stack,
			destruirElemento);

	if (pcbActual->etiquetas_size > 0) {
		free(pcbActual->indice_etiquetas);
	}

	free(pcbActual);

}

void inicializarSemaforos() {
	pthread_mutex_init(&semaforoDesconexion, NULL);
}

void inicializarSeniales() {
	signal(SIGUSR1, desconectarse);
}

void desconectarse() {

	desconexion = true;

	if (pcb_ejecutando) {

		pthread_mutex_lock(&semaforoDesconexion);

		PROTOCOLO_CPU_A_KERNEL meDesconecto = ME_DESCONECTO;

		librocket_enviarMensaje(kernelSocket, &meDesconecto,
				sizeof(PROTOCOLO_CPU_A_KERNEL), infoLogger);

		pcbActual->EC = DESCONEXION_CPU;

		log_info(infoLogger, "Se desconecta la CPU por SIGUSR1.");
		log_info(infoLogger, "Estaba ejecutando el programa con PID: %i.",
				pcbActual->PID);

		enviarPcb(kernelSocket, pcbActual, infoLogger);
		destruir_pcb();

	}

	liberarMemoria();

	exit(EXIT_SUCCESS);

}

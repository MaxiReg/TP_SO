#include <cpu.h>

int main(void) {
	/* ******** CREAR ESTRUCTURAS GENERALES ******* */
	// 1. Crear logger
	logger = log_create("log/cpu.log", "cpu", 1, LOG_LEVEL_INFO);
	log_info(logger, "[============= CREAR ESTRUCTURAS GENERALES =============]");

	// 2. Obtener la directorio actual
	char* directorio_actual = getcwd(NULL, 0);
	log_info(logger, "   - DIR. ACTUAL: %s", directorio_actual);

	// 3. Obtener configuraciones relevantes
	t_config* config = crear_config(directorio_actual, "/cfg/cpu.config");
	//free(directorio_actual);
	log_info(logger, "   - CONFIG CREADO");

	// 3. b. Obtener configuraciones de CPU
	config_cpu = obtener_configuraciones_cpu(config);
	log_info(logger, "   - CONFIGURACIONES OBTENIDAS");

	config_destroy(config);
	log_info(logger, "   - CONFIG DESTRUIDO");

	// 3. c. Obtener configuraciones de Memoria (Handshake)
	handshake_cpu();
	log_info(logger, "   - HANDSHAKE REALIZADO");

	log_info(logger, "[=======================================================]\n");

	// 4. Imprimir configuraciones
	imprimir_configuraciones_cpu();

	/* *********** EJECUCION DE PROCESOS ********** */
	pthread_t h_conexion[2];

	// 1. Crear mutex
    pthread_mutex_init(&mx_interrupcion, NULL);

	// 2. Crear hilos
	pthread_create(&h_conexion[CICLO_INSTRUCCION], NULL, (void*) ciclo_instruccion, NULL);
	pthread_create(&h_conexion[INTERRUPT], NULL, (void*) escuchar_interrupciones, NULL);

	/* ******** DESTRUCCION DE ESTRUCTURAS ******** */
	// 1. Destuccion hilos
	pthread_join(h_conexion[CICLO_INSTRUCCION], NULL);
	pthread_join(h_conexion[INTERRUPT], NULL);

	// 2. Destuccion mutex
    pthread_mutex_destroy(&mx_interrupcion);

	// 3. Destuccion config_cpu
	liberar_config_cpu();

	// 4. Destuccion logger
	log_destroy(logger);

	return EXIT_SUCCESS;
}

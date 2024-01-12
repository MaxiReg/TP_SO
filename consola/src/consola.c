#include <consola.h>

int main(int argc, char **argv) {
	/* ******** CREAR ESTRUCTURAS GENERALES ******* */
	// 1. Crear logger
	logger = log_create("log/consola.log", "con", 1, LOG_LEVEL_INFO);
	log_info(logger, "[============= CREAR ESTRUCTURAS GENERALES =============]");

	// 2. Obtener la directorio actual
	char* directorio_actual = getcwd(NULL, 0);
	log_info(logger, "   - DIR. ACTUAL: %s", directorio_actual);

	// 3. Obtener configuraciones relevantes
	t_config* config = crear_config(directorio_actual, "/cfg/consola.config");
	log_info(logger, "   - CONFIG CREADO");

	// 3. b. Obtener configuraciones de Consola
	t_config_consola* config_consola = obtener_configuraciones_consola(config);
	log_info(logger, "   - CONFIGURACIONES OBTENIDAS");

	config_destroy(config);
	log_info(logger, "   - CONFIG DESTRUIDO");

	// 4. Obtener parametros
	// 4. a. Obtener tamaño del proceso
	uint32_t tamanio_proceso = strtol(argv[2], NULL, 10);
	log_info(logger, "   - TAMANIO PROCESO OBTENIDO");

	// 4. b. Obtener instrucciones del proceso
	char* direccion_archivo_pseudocodigo = obtener_direccion_archivo_pseudocodigo(directorio_actual, argv[1]);

	// Parsear el archivo de pseudocodigo
	t_list* l_instrucciones = parsear_archivo(direccion_archivo_pseudocodigo);
	log_info(logger, "   - INSTRUCCIONES OBTENIDAS");
	free(direccion_archivo_pseudocodigo);
	free(directorio_actual);

	// 5. Cronometrar comienzo del proceso
	time_t tiempo_inicializacion = time(NULL);

	log_info(logger, "[=======================================================]\n");

	// 6. Imprimir informacion obtenida del proceso
	log_info(logger, "[==================== INFO. PROCESO ====================]");

	log_info(logger, "   * TAMAÑO PROCESO: %db", tamanio_proceso);
	imprimir_instrucciones(logger, l_instrucciones);

	/* ******** ENVIO/RECEPCION DEL PROCESO ******* */
	// 1. Crear conexión con el Kernel
	int conexion = crear_conexion(config_consola->ip_kernel, config_consola->puerto_kernel);
	log_info(logger, "[===================== HILO: KERNEL ====================]");
	log_info(logger, "   - PUERTO: %s IP: %s", config_consola->ip_kernel, config_consola->puerto_kernel);

	// 2. Armar y enviar el paquete
	enviar_paquete_instrucciones(conexion, l_instrucciones, tamanio_proceso);
	log_info(logger, "   - PAQUETE ENVIADO");

	// 3. Cerrar conexión con el Kernel
	terminar_conexion_consola(conexion);
	log_info(logger, "   - CONEXION LIBERADA");

	// 4. Cronometrar tiempo finalizacion
	time_t tiempo_finalizacion = time(NULL);
	double tiempo_duracion = difftime(tiempo_finalizacion, tiempo_inicializacion);
	log_info(logger, "   - DURACION PROCESO: %02d:%02d", (int) (tiempo_duracion / 60), ((int) tiempo_duracion) % 60);

	log_info(logger, "[=======================================================]\n");

	// Destruir elementos pendientes
	liberar_config_consola(config_consola);
	log_destroy(logger);
	//list_destroy_and_destroy_elements(l_instrucciones, free); TODO: Explota

	return EXIT_SUCCESS;
}

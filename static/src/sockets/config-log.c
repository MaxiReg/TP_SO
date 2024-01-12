#include <sockets/config-log.h>

t_log* iniciar_logger(void) {
	t_log* logger;

	if((logger = log_create("log/kernel.log", "ker", 1, LOG_LEVEL_INFO)) == NULL){

		perror("No se pudo crear el log/n");
		abort();
	}
	return logger;
}

t_config* crear_config(char* directorio_actual, char* direccion_config) {
	char* direccion = string_new();

	string_append(&direccion, directorio_actual);
	string_append(&direccion, direccion_config);

	return config_create(direccion);
}

t_config* iniciar_config(t_log* logger) {
	log_info(logger, "[============= CREAR ESTRUCTURAS GENERALES =============]");
	// Obtener la directorio actual
	char* directorio_actual = getcwd(NULL, 0);
	log_info(logger, "   - DIR. ACTUAL: %s", directorio_actual);

	t_config* config = crear_config(directorio_actual, "/cfg/kernel.config");
	free(directorio_actual);
	log_info(logger, "   - CONFIG CREADO");

	log_info(logger, "[=======================================================]\n");

	if(config == NULL){
		perror("No se pudo crear la config\n");
		abort();
	}

	return config;

}

char* obtener_de_config(t_config* config, char* key) {

	char* valor;

	if((valor = config_get_string_value(config, key)) == NULL){

		perror("No se pudo obtener el valor del archivo de configuraciones!\n");
		abort();
	}

	return valor;
}

int obtener_int_de_config(t_config* config, char* key){

	int valor;

	if((valor = config_get_int_value(config, key)) < 0){

		perror("No se pudo obtener el valor del archivo de configuraciones!\n");
		abort();
	}

	return valor;
}

float obtener_float_de_config(t_config* config, char* key){

	double valor;

	if((valor = config_get_double_value(config, key)) < 0){

		perror("No se pudo obtener el valor del archivo de configuraciones!\n");
		abort();
	}

	return (float) valor;
}

void terminar_programa(int conexion, t_config* config) {
	config_destroy(config);
    close(conexion);
}


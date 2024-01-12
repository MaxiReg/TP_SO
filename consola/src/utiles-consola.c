#include <utiles-consola.h>

void terminar_conexion_consola(int conexion) {
	if((int) recibir_mensaje(conexion) == 1)
		log_info(logger, "   - PROCESO EJECUTADO EXITOSAMENTE");
	else
		log_info(logger, "   - PROCESO EJECUTADO FALLIDAMENTE");

	terminar_conexion(conexion);
}

t_config* crear_config(char* directorio_actual, char* direccion_config) {
	char* direccion = string_new();

	string_append_with_format(&direccion, "%s%s", directorio_actual, direccion_config);

	return config_create(direccion);
}

char* obtener_direccion_archivo_pseudocodigo(char* directorio_actual, char* archivo_pseudocodigo) {
	char* direccion_archivo_pseudocodigo = string_new();

	string_append(&direccion_archivo_pseudocodigo, directorio_actual);
	string_append_with_format(&direccion_archivo_pseudocodigo, "/archivos-pseudocodigo/%s", archivo_pseudocodigo);

	return direccion_archivo_pseudocodigo;
}

void liberar_config_consola(t_config_consola* config_consola) {
	free(config_consola->ip_kernel);
	free(config_consola->puerto_kernel);
	free(config_consola);
}

t_config_consola* obtener_configuraciones_consola(t_config* config) {
	t_config_consola* config_consola = malloc(sizeof(t_config_consola));

	char* ip = config_get_string_value(config, "IP");
	config_consola->ip_kernel = strdup(ip);

	char* puerto_memoria = config_get_string_value(config, "PUERTO");
	config_consola->puerto_kernel = strdup(puerto_memoria);

	return config_consola;
}

void liberar_array_cadenas(char** l_cadenas) {
	int i = 0;

	while(l_cadenas[i] != NULL) {
		free(l_cadenas[i]);
		i++;
	}

	free(l_cadenas);
}

e_id_operacion obtener_id_operacion(char* operacion) {
	if(!strcmp(operacion, "NO_OP"))
		return NO_OP;

	if(!strcmp(operacion, "I/O"))
		return I_O;

	if(!strcmp(operacion, "READ"))
		return READ;

	if(!strcmp(operacion, "COPY"))
		return COPY;

	if(!strcmp(operacion, "WRITE"))
		return WRITE;

	if(!strcmp(operacion, "EXIT"))
		return EXIT_I;

	return -1;
}

t_instruccion* crear_instruccion(char* cadena) {
	t_instruccion* instruccion = malloc(sizeof(t_instruccion));

	char** c_separada = string_split(cadena, " ");

	instruccion->identificador = obtener_id_operacion(c_separada[0]);
	instruccion->parametros[0] = (uint32_t) NULL;
	instruccion->parametros[1] = (uint32_t) NULL;

	if(c_separada[1] != NULL) {
		instruccion->parametros[0] = (uint32_t) strtol(c_separada[1], NULL, 10);

		if(c_separada[2] != NULL) {
			instruccion->parametros[1] = (uint32_t) strtol(c_separada[2], NULL, 10);
		}
	}

	liberar_array_cadenas(c_separada);

	return instruccion;
}

char* obtener_archivo(char* path) {
	FILE* a_pseudocodigo = fopen(path, "r");

	if(a_pseudocodigo == NULL) {
		printf("Fallo de apertura de archivo");
		exit(1);
	};

	// Obtener longitud archivo
	fseek(a_pseudocodigo, SEEK_SET, SEEK_END);
	int longitud_archivo = ftell(a_pseudocodigo);
	rewind(a_pseudocodigo);

	// Reservar memoria para el archivo entero
	char* c_archivo = calloc(1, longitud_archivo + 1);

	// Copiar archivo en la cadena
	if(fread(c_archivo, longitud_archivo, 1, a_pseudocodigo) != 1) {
		fclose(a_pseudocodigo);
		free(c_archivo);
		fputs("Fallo de lectura de archivo", stderr);
		exit(1);
	}

	fclose(a_pseudocodigo);

	return c_archivo;
}

t_list* crear_lista_instrucciones(char** c_separada) {
	t_list* l_instrucciones = list_create();

	int i = 0;
	while(c_separada[i] != NULL) {
		t_instruccion* instruccion = crear_instruccion(c_separada[i]);

		if(instruccion->identificador == NO_OP) {
			int cantidad_no_op = (int) instruccion->parametros[0];
			for(int j = 0; j < cantidad_no_op; j++) {
				instruccion->parametros[0] = 0;
				list_add(l_instrucciones, instruccion);
			}
		} else {
			list_add(l_instrucciones, instruccion);
		}

		//free(instruccion); //TODO: Resuleve los Memory Leaks, pero surgen mas problemas en Valgrind

		i++;
	}

	return l_instrucciones;
}

t_list* parsear_archivo(char* path) {
	char* cadena = obtener_archivo(path);
	char** c_separada = string_split(cadena, "\n");

	t_list* l_instrucciones = crear_lista_instrucciones(c_separada);

	free(cadena);
	liberar_array_cadenas(c_separada);

	return l_instrucciones;
}

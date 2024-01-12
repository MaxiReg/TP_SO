#ifndef INCLUDE_SOCKETS_CONFIG_LOG_H_
#define INCLUDE_SOCKETS_CONFIG_LOG_H_
	/* ******* BIBLIOTECA ******* */
	// ESTÁNDAR
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <unistd.h>

	// COMMONS
	#include <commons/log.h>
	#include <commons/config.h>
	#include <commons/string.h>

	/* ******** FUNCIONES ******* */
	/**
	* @NAME: crear_config
	* @DESC:
	*/
	t_config* crear_config(char* directorio_actual, char* direccion_config);

	/**
	* @NAME: iniciar_config
	* @DESC:
	*/
	t_config* iniciar_config(t_log* logger);

	/**
	* @NAME: obtener_de_config
	* @DESC:
	*/
	char* obtener_de_config(t_config* config, char* key);

	/**
	* @NAME: obtener_int_de_config
	* @DESC:
	*/
	int obtener_int_de_config(t_config* config, char* key);

	/**
	* @NAME: obtener_float_de_config
	* @DESC:
	*/
	float obtener_float_de_config(t_config* config, char* key);

	/**
	* @NAME: iniciar_logger
	* @DESC:
	*/
	t_log* iniciar_logger(void);

	/**
	* @NAME: terminar_programa
	* @DESC:
	*/
	void terminar_programa(int conexion, t_config* config);

#endif /* INCLUDE_SOCKETS_CONFIG_LOG_H_ */

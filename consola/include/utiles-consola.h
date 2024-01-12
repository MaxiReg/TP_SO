#ifndef INCLUDE_UTILES_CONSOLA_H_
#define INCLUDE_UTILES_CONSOLA_H_

	/* ******* BIBLIOTECA ******* */
	// ESTÁNDAR
	#include <stdio.h>
	#include <stdlib.h>
	#include <stdint.h>
	#include <unistd.h>

	// MISCELÁNEO
	#include <readline/readline.h>
	#include <time.h>

	// COMMONS
	#include <commons/collections/list.h>
	#include <commons/config.h>
	#include <commons/log.h>
	#include <commons/string.h>

	// COMPARTIDO
	#include <serializacion/serializacion.h>
	#include <serializacion/serializacion-instruccion.h>
	#include <sockets/utiles-cliente.h>

	/* ******** FUNCIONES ******* */
	t_log* logger;

	typedef struct {
		char* ip_kernel;
		char* puerto_kernel;
	} t_config_consola;

	/* ******** FUNCIONES ******* */
	/**
	* @NAME: terminar_conexion_consola
	* @DESC: Espera la finalizacion de ejecucion de un proceso
	* 		 y cierra la conexion de consola
	*/
	void terminar_conexion_consola(int conexion);

	/**
	 * @NAME: recibir_respuesta
	 * @DESC: recibe respuesta del servidor
	 */
	int recibir_respuesta(int conexion);

	/**
	* @NAME: crear_config
	* @DESC:
	*/
	t_config* crear_config(char* directorio_actual, char* direccion_config);

	/**
	* @NAME: obtener_direccion_archivo_pseudocodigo
	* @DESC:
	*/
	char* obtener_direccion_archivo_pseudocodigo(char* directorio_actual, char* archivo_pseudocodigo);

	/**
	* @NAME: obtener_configuraciones_consola
	* @DESC: Obtiene configuraciones para la consola
	*/
	t_config_consola* obtener_configuraciones_consola(t_config* config);

	/**
	* @NAME: liberar_config_consola
	* @DESC: Libera un t_config_consola*
	*/
	void liberar_config_consola(t_config_consola* config_consola);

	/**
	* @NAME: liberar_array_cadenas
	* @DESC: Libera un char** y los char* a lo que apuntaba
	*/
	void liberar_array_cadenas(char** l_cadenas);

	/**
	* @NAME: obtener_id_operacion
	* @DESC: Convierte una cadena en id_operacion
	*/
	e_id_operacion obtener_id_operacion(char* operacion);

	/**
	* @NAME: crear_instruccion
	* @DESC: Crea una instrucción de una cadena
	*/
	t_instruccion* crear_instruccion(char* cadena);

	/**
	* @NAME: obtener_archivo
	* @DESC: Obtiene un archivo en formato cadena
	*/
	char* obtener_archivo(char* path);

	/**
	* @NAME: cargar_lista_instrucciones
	* @DESC: Crea lista de instrucciones desde array de instrucciones
	*/
	t_list* crear_lista_instrucciones(char** c_separada);

	/**
	* @NAME: parsear_archivo
	* @DESC: Convierte un archivo de pseudocódigo en una lista de instrucciones
	*/
	t_list* parsear_archivo(char* path);

#endif /* UTILES_CONSOLA_H_ */

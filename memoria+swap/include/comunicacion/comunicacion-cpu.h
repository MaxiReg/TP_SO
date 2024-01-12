#ifndef INCLUDE_COMUNICACION_COMUNICACION_CPU_H_
#define INCLUDE_COMUNICACION_COMUNICACION_CPU_H_

	/* ******* BIBLIOTECA ******* */
	// ESTÁNDAR
	#include <stdio.h>
	#include <stdlib.h>

	// MISCELÁNEO
	#include <pthread.h>
	#include <readline/readline.h>

	// COMMONS
	#include <commons/log.h>
	#include <commons/string.h>
	#include <commons/config.h>

	// COMPARTIDO
	#include <utiles-memoria+swap.h>
	#include <comunicacion/gestion-paginas/algoritmo-reemplazo.h>

	/* ******** FUNCIONES ******* */
	/**
	* @NAME: obtener_indice_marco
	* @DESC:
	*/
	uint32_t enviar_indice_marco(int socket_servidor);

	/**
	* @NAME: obtener_valor_marco
	* @DESC:
	*/
	void obtener_valor_marco(int socket_cliente, int pid);

	/**
	* @NAME: modificar_valor_marco
	* @DESC:
	*/
	void modificar_valor_marco(int socket_cliente, int pid);

#endif /* INCLUDE_COMUNICACION_COMUNICACION_CPU_H_ */

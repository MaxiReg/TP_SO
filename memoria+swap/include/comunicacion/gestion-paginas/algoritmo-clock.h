#ifndef INCLUDE_COMUNICACION_ALGORITMO_CLOCK_H_
#define INCLUDE_COMUNICACION_ALGORITMO_CLOCK_H_

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

	/* ******** ESTRUCTURAS ******* */

	/* ******** FUNCIONES ******* */
	/**
	* @NAME: avanzar_puntero
	* @DESC:
	*/
	uint32_t avanzar_puntero(t_entrada_tp_2do_nivel* entrada_tp_2do_nivel, int* puntero);

#endif /* INCLUDE_COMUNICACION_ALGORITMO_CLOCK_H_ */

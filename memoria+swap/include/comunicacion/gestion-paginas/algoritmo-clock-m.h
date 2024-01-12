#ifndef INCLUDE_COMUNICACION_GESTION_PAGINAS_ALGORITMO_CLOCK_M_H_
#define INCLUDE_COMUNICACION_GESTION_PAGINAS_ALGORITMO_CLOCK_M_H_

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
	int modo_busqueda;

	typedef enum {
		BUSQUEDA_BIT_USO,
		BUSQUEDA_BIT_MODIFICADO
	} e_busqueda_clock_m;

	/* ******** FUNCIONES ******* */
	/**
	* @NAME: cambiar_modo_busqueda
	* @DESC:
	*/
	void cambiar_modo_busqueda();

	/**
	* @NAME: busqueda_uso
	* @DESC:
	*/
	uint32_t busqueda_bit_uso(t_entrada_tp_2do_nivel* entrada_tp_2do_nivel, int* puntero);

	/**
	* @NAME: busqueda_modificado
	* @DESC:
	*/
	uint32_t busqueda_bit_modificado(t_entrada_tp_2do_nivel* entrada_tp_2do_nivel, int* puntero);

	/**
	* @NAME: avanzar_puntero_m
	* @DESC:
	*/
	uint32_t avanzar_puntero_m(t_entrada_tp_2do_nivel* entrada_tp_2do_nivel, int* puntero);

#endif /* INCLUDE_COMUNICACION_GESTION_PAGINAS_ALGORITMO_CLOCK_M_H_ */

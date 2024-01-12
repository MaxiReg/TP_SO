#ifndef INCLUDE_COMUNICACION_ALGORITMO_REEMPLAZO_H_
#define INCLUDE_COMUNICACION_ALGORITMO_REEMPLAZO_H_

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

	/* ******** ESTRUCTURAS ******* */

	/* ******** FUNCIONES ******* */

	/**
	* @NAME: mostrar_puntero
	* @DESC:
	*/
	void mostrar_puntero(t_algoritmo_reemplazo* e_a_reemplazo);

	/**
	* @NAME: obtener_indice_marco_clock_m
	* @DESC:
	*/
	uint32_t obtener_indice_marco_clock_m(int pid);

	/**
	* @NAME: obtener_indice_marco_clock
	* @DESC:
	*/
	uint32_t obtener_indice_marco_clock(int pid);


	//REEMPLAZO DE PAGINA
	/**
	* @NAME: obtener_entrada_tp_2do_nivel_por_marco
	* @DESC:
	*/
	t_entrada_tp_2do_nivel* obtener_entrada_tp_2do_nivel_por_marco(t_list* l_tp_1er_nivel, int indice_marco);

	/**
	* @NAME: intercambiar_paginas
	* @DESC:
	*/
	void intercambiar_paginas(t_entrada_tp_2do_nivel* entrada_tp_2do_nivel, t_list* l_tp_1er_nivel, t_peticion_memoria_indice* peticion_memoria_indice, int indice_marco);

	/**
	* @NAME: algoritmo_reemplazo_pagina
	* @DESC:
	*/
	uint32_t algoritmo_reemplazo_pagina(t_entrada_tp_2do_nivel* entrada_tp_2do_nivel, t_list* l_tp_1er_nivel, t_peticion_memoria_indice* peticion_memoria_indice);

	//ASIGNACION FIFO
	/**
	* @NAME: inicializar_entrada_tp_2do_nivel
	* @DESC:
	*/
	void inicializar_entrada_tp_2do_nivel(t_entrada_tp_2do_nivel* entrada_tp_2do_nivel, uint32_t indice_marco);

	/**
	* @NAME: asignar_nuevo_marco
	* @DESC:
	*/
	uint32_t asignar_nuevo_marco(int pid, t_entrada_tp_2do_nivel* entrada_tp_2do_nivel);

	//OBTENER INDICE MARCO
	/**
	* @NAME: hay_espacio_memoria
	* @DESC:
	*/
	int hay_espacio_memoria();

	/**
	* @NAME: maximo_marcos_por_proceso
	* @DESC:
	*/
	int maximo_marcos_por_proceso(t_list* l_tp_1er_nivel);

	/**
	* @NAME: obtener_indice_marco
	* @DESC:
	*/
	uint32_t obtener_indice_marco(t_entrada_tp_2do_nivel* entrada_tp_2do_nivel, t_list* l_tp_1er_nivel, t_peticion_memoria_indice* peticion_memoria_indice);

#endif /* INCLUDE_COMUNICACION_ALGORITMO_REEMPLAZO_H_ */

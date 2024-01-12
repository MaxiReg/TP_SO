#ifndef INCLUDE_COMUNICACION_COMUNICACION_KERNEL_H_
#define INCLUDE_COMUNICACION_COMUNICACION_KERNEL_H_

	/* ******* BIBLIOTECA ******* */
	// ESTÁNDAR
	#include <stdio.h>
	#include <stdlib.h>

	// MISCELÁNEO
	#include <pthread.h>
	#include <readline/readline.h>
	#include <time.h>

	// COMMONS
	#include <commons/log.h>
	#include <commons/string.h>
	#include <commons/config.h>

	// COMPARTIDO
	#include <utiles-memoria+swap.h>

	/* ******** FUNCIONES ******* */
	// OPERACIONES DE TABLAS DE PAGINAS
	/**
	* @NAME: liberar_memoria_proceso
	* @DESC:
	*/
	void liberar_memoria_proceso(int tabla_paginas);

	/**
	* @NAME: crear_estructuras_algoritmo_reemplazo
	* @DESC:
	*/
	void crear_estructuras_algoritmo_reemplazo(int pid);

	/**
	* @NAME: crear_entrada_tp_2do_nivel
	* @DESC:
	*/
	t_entrada_tp_2do_nivel* crear_entrada_tp_2do_nivel();

	/**
	* @NAME: crear_tp_2do_nivel
	* @DESC:
	*/
	t_tp_2do_nivel* crear_tp_2do_nivel(int index);

	/**
	* @NAME: crear_tp_1er_nivel
	* @DESC:
	*/
	t_list* crear_tp_1er_nivel(int tamanio);

	// OPERACIONES DE KERNEL
	/**
	* @NAME: inicializacion_proceso
	* @DESC:
	*/
	void inicializacion_proceso(t_pcb_reducido* pcb_reducido, int socket_servidor);

	/**
	* @NAME: suspension_proceso
	* @DESC:
	*/
	void suspension_proceso(t_pcb_reducido* pcb_reducido, int socket_servidor);

	/**
	* @NAME: finalizacion_proceso
	* @DESC:
	*/
	void finalizacion_proceso(t_pcb_reducido* pcb_reducido, int socket_servidor);

#endif /* INCLUDE_COMUNICACION_COMUNICACION_KERNEL_H_ */

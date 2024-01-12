#ifndef INCLUDE_MMU_MMU_H_
#define INCLUDE_MMU_MMU_H_

	/* ******* BIBLIOTECA ******* */
	// ESTÁNDAR
	#include <stdio.h>
	#include <stdlib.h>
	#include <stdint.h>
	#include <math.h>

	// MISCELÁNEO
	#include <pthread.h>
	#include <readline/readline.h>

	// COMMONS
	#include <commons/collections/list.h>
	#include <commons/config.h>
	#include <commons/log.h>
	#include <commons/string.h>

	// INTERNO
	#include <utiles-cpu.h>
	#include <mmu/tlb.h>
	#include <mmu/crear-direccion.h>

	// COMPARTIDO
	#include <serializacion/serializacion-peticion-memoria.h>

	/* ******** FUNCIONES ******* */
	/**
	* @NAME: enviar_peticion_indice
	* @DESC:
	*/
	void enviar_peticion_indice(t_pcb* pcb, t_direccion_logica* direccion_logica, int socket_cliente);

	/**
	* @NAME: obtener_indice_tabla_2do_nivel
	* @DESC:
	*/
	int obtener_indice_tabla_2do_nivel(t_pcb* pcb, t_direccion_logica* direccion_logica, int socket_cliente);

	/**
	* @NAME: obtener_indice_marco_tp
	* @DESC:
	*/
	int obtener_indice_marco_tp(int entrada_tabla_2do_nivel, int socket_cliente);

	/**
	* @NAME: obtener_direccion_fisica
	* @DESC:
	*/
	t_direccion_fisica* obtener_direccion_fisica(t_direccion_logica* direccion_logica, t_pcb* pcb, int socket_cliente);

	/**
	* @NAME: enviar_peticion_io
	* @DESC:
	*/
	void enviar_peticion_io(int codigo_operacion, t_direccion_fisica* direccion_fisica, int numero_pagina, int a_escribir, int socket_cliente);

	/**
	* @NAME: obtener_valor_marco
	* @DESC:
	*/
	uint32_t obtener_valor_marco(t_direccion_fisica* direccion_fisica, int numero_pagina, int socket_cliente);

	/**
	* @NAME: enviar_valor_a_escribir
	* @DESC:
	*/
	uint32_t enviar_valor_a_escribir(t_direccion_fisica* direccion_fisica, int numero_pagina, int a_escribir, int socket_cliente);

#endif /* INCLUDE_MMU_MMU_H_ */

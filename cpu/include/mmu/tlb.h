#ifndef INCLUDE_MMU_TLB_H_
#define INCLUDE_MMU_TLB_H_

	/* ******* BIBLIOTECA ******* */
	// ESTÁNDAR
	#include <stdio.h>
	#include <stdlib.h>
	#include <stdint.h>

	// MISCELÁNEO
	#include <pthread.h>
	#include <readline/readline.h>

	// COMMONS
	#include <commons/collections/list.h>
	#include <commons/config.h>
	#include <commons/log.h>
	#include <commons/string.h>

	// COMPARTIDO

	// INTERNO
	#include <utiles-cpu.h>

	/* ******** FUNCIONES ******* */
	/**
	* @NAME: limpiar_tlb
	* @DESC:
	*/
	void limpiar_tlb();

	/**
	* @NAME: eliminar_entrada_invalida
	* @DESC:
	*/
	void eliminar_entrada_invalida(t_entrada_tlb* entrada_tlb);

	/**
	* @NAME: reemplazar_entrada_tlb
	* @DESC:
	*/
	void reemplazar_entrada_tlb(t_entrada_tlb* entrada_tlb);

	/**
	* @NAME: agregar_entrada_tlb
	* @DESC:
	*/
	void agregar_entrada_tlb(t_entrada_tlb* entrada_tlb);

	/**
	* @NAME: actualizar_tlb
	* @DESC:
	*/
	void actualizar_tlb(t_entrada_tlb* entrada_tlb);

	/**
	* @NAME: son_iguales_a_pagina
	* @DESC:
	*/
	bool son_iguales_a_pagina(t_entrada_tlb* entrada_tlb, int numero_pagina);

	/**
	* @NAME: algoritmo_busqueda_lru
	* @DESC:
	*/
	int algoritmo_busqueda_lru(int numero_pagina);

	/**
	* @NAME: algoritmo_busqueda_fifo
	* @DESC:
	*/
	int algoritmo_busqueda_fifo(int numero_pagina);

	/**
	* @NAME: obtener_indice_marco_tlb
	* @DESC:
	*/
	int obtener_indice_marco_tlb(int numero_pagina);


#endif /* INCLUDE_MMU_TLB_H_ */

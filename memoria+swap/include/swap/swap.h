#ifndef INCLUDE_SWAP_SWAP_H_
#define INCLUDE_SWAP_SWAP_H_

	/* ******* BIBLIOTECA ******* */
	// ESTÁNDAR
	#include <stdio.h>
	#include <stdlib.h>

	// MISCELÁNEO
	#include <pthread.h>
	#include <semaphore.h>
	#include <readline/readline.h>

	// COMMONS
	#include <commons/log.h>
	#include <commons/string.h>
	#include <commons/config.h>

	// COMPARTIDO
	#include <utiles-memoria+swap.h>

	/* ******** FUNCIONES ******* */
	// AUXILIARES
	/**
	* @NAME: retardar_swap
	* @DESC: Simula el retardo en el acceso a memoria
	*/
	void retardar_swap();

	/**
	* @NAME: crear_direccion_swap
	* @DESC: Crea el nobre del archivo ("pid.swap")
	* 		 y lo ubica en la carpeta adecuada
	*/
	char* crear_direccion_swap(int pid);

	/**
	* @NAME: escribir_pagina_swap
	* @DESC:
	*/
	void escribir_pagina_swap(int pid, int pagina, int marco);

	/**
	* @NAME: escribir_pagina_memoria
	* @DESC:
	*/
	void escribir_pagina_memoria(int pid, int pagina, int marco);

	// OPERACIONES
	/**
	* @NAME: crear_archivo_swap
	* @DESC:
	*/
	void crear_archivo_swap(t_pcb_reducido* pcb_reducido);

	/**
	* @NAME: suspender_proceso
	* @DESC:
	*/
	void suspender_proceso(t_pcb_reducido* pcb_reducido);

	/**
	* @NAME: reemplazar_pagina
	* @DESC:
	*/
	void reemplazar_pagina();

	/**
	* @NAME: traer_pagina
	* @DESC:
	*/
	void traer_pagina();

	/**
	* @NAME: borrar_archivo_swap
	* @DESC:
	*/
	void borrar_archivo_swap(int pid);

#endif /* INCLUDE_SWAP_SWAP_H_ */

#ifndef INCLUDE_MMU_CREAR_DIRECCION_H_
#define INCLUDE_MMU_CREAR_DIRECCION_H_

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

	/* ******** ESTRUCTURAS ******* */
	/**
	* @NAME: numero_pagina
	* @DESC: Calcular el numero pagina
	*/
	int calcular_numero_pagina(int direccion_logica);

	/**
	* @NAME: entrada_tabla_1er_nivel
	* @DESC: Calcular el entrada de tabla de 1er nivel
	*/
	int calcular_entrada_tabla_1er_nivel(int numero_pagina);

	/**
	* @NAME: entrada_tabla_2do_nivel
	* @DESC: Calcular el entrada de tabla de 2do nivel
	*/
	int calcular_entrada_tabla_2do_nivel(int numero_pagina);

	/**
	* @NAME: desplazamiento
	* @DESC: Calcular el desplazamiento
	*/
	int calcular_desplazamiento(int direccion_logica, int numero_pagina);

	/**
	* @NAME: crear_direccion_logica
	* @DESC:
	*/
	t_direccion_logica* crear_direccion_logica(int direccion_logica);

	/**
	* @NAME: crear_direccion_logica
	* @DESC:
	*/
	t_direccion_fisica* crear_direccion_fisica(int marco, int desplazamiento);

	#endif /* INCLUDE_MMU_CREAR_DIRECCION_H_ */

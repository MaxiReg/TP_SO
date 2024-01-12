#ifndef INCLUDE_GESTION_ESPACIO_LIBRE_H_
#define INCLUDE_GESTION_ESPACIO_LIBRE_H_

	/* ******* BIBLIOTECA ******* */
	// ESTÁNDAR
	#include <stdio.h>
	#include <stdlib.h>
	#include <unistd.h>
	#include <stdint.h>
	#include <math.h>

	// MISCELÁNEO
	#include <pthread.h>
	#include <readline/readline.h>
	#include <semaphore.h>
	#include <fcntl.h> //open()
	#include <sys/mman.h>
	#include <sys/stat.h>

	// COMMONS
	#include <commons/collections/list.h>
	#include <commons/log.h>
	#include <commons/string.h>
	#include <commons/config.h>

	/* ******** FUNCIONES ******* */
	/**
	* @NAME: ocupar_marco
	* @DESC:
	*/
	void ocupar_marco(t_list* l_marcos_libres, int indice);

	/**
	* @NAME: liberar_marco
	* @DESC:
	*/
	void liberar_marco(t_list* l_marcos_libres, int indice);

	/**
	* @NAME: crear_l_marcos_ocupados
	* @DESC:
	*/
	t_list* crear_l_marcos_libres(int tamanio_memoria, int tamanio_pagina);

#endif /* INCLUDE_GESTION_ESPACIO_LIBRE_H_ */

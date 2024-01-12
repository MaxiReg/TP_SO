#ifndef INCLUDE_GESTION_ARCHIVO_H_
#define INCLUDE_GESTION_ARCHIVO_H_

/* ******* BIBLIOTECA ******* */
	// ESTÁNDAR
	#include <stdio.h>
	#include <stdlib.h>
	#include <stdint.h>
	#include <unistd.h>
	#include <math.h>

	// MISCELÁNEO
	#include <pthread.h>
	#include <readline/readline.h>
	#include <semaphore.h>
	#include <fcntl.h> //open()
	#include <sys/mman.h>
	#include <sys/stat.h>

	// COMMONS
	#include <commons/bitarray.h>
	#include <commons/log.h>
	#include <commons/string.h>
	#include <commons/config.h>

	#define TAMANIO_MEMORIA 4096
	#define TAMANIO_PAGINA 16
	#define TAMANIO_MARCO 16

	/* ******** ESTRUCTURAS ******* */
	// GENERAL
	void* memoria_test;


	/* ******** FUNCIONES ******* */
	// CONEXIONES
	/**
	* @NAME: comunicacion_kernel
	* @DESC:
	*/

#endif /* INCLUDE_GESTION_ARCHIVO_H_ */

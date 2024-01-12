#ifndef INCLUDE_CICLO_INSTRUCCION_CICLO_INSTRUCCION_H_
#define INCLUDE_CICLO_INSTRUCCION_CICLO_INSTRUCCION_H_

	/* ******* BIBLIOTECA ******* */
	// ESTÁNDAR
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>

	// MISCELÁNEO
	#include <netdb.h>
	#include <signal.h>
	#include <sys/socket.h>
	#include <unistd.h>

	// COMMONS
	#include <commons/collections/list.h>
	#include <commons/config.h>
	#include <commons/log.h>
	#include <commons/string.h>

	// COMPARTIDO

	// INTERNO
	#include <utiles-cpu.h>
	#include <ciclo-instruccion/ejecucion-instruccion.h>

	/* ******** ESTRUCTURAS ******* */
	uint8_t hay_interrupcion;
	pthread_mutex_t mx_interrupcion;

	/* ******** FUNCIONES ******* */
	// CICLO DE INSTRUCCION
	/**
	* @NAME: escuchar_interrupciones
	* @DESC:
	*/
	void escuchar_interrupciones();

	/**
	* @NAME: procesar_instruccion
	* @DESC:
	*/
	int procesar_instruccion(t_pcb* pcb, int socket_cliente);

	/**
	* @NAME: ciclo_instruccion
	* @DESC:
	*/
	void ciclo_instruccion();

#endif /* INCLUDE_CICLO_INSTRUCCION_CICLO_INSTRUCCION_H_ */

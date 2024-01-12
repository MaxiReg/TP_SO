#ifndef INCLUDE_CICLO_INSTRUCCION_EJECUCION_INSTRUCCION_H_
#define INCLUDE_CICLO_INSTRUCCION_EJECUCION_INSTRUCCION_H_

	/* ******* BIBLIOTECA ******* */
	// ESTÁNDAR
	#include <stdio.h>
	#include <stdlib.h>

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
	#include <mmu/mmu.h>

	/* ******** FUNCIONES ******* */
	/**
	* @NAME: ejecutar_noop
	* @DESC: Esperar un tiempo determinado
	* 		 (retardo_noop esta en milisegundos)
	*/
	void ejecutar_noop(int retardo_noop);

	/**
	* @NAME: devolver_paquete_pcb
	* @DESC: Envia el PCB a un modulo conectado
	*/
	void devolver_paquete_pcb(t_pcb* pcb, int conexion);

	/**
	* @NAME: ejecutar_read
	* @DESC: Obtien el valor de memoria de esa dirección
	* 		 lógica y lo loggea
	*/
	int ejecutar_read(int direccion_logica, t_pcb* pcb, int socket_cliente);

	/**
	* @NAME: ejecutar_write
	* @DESC: Escribir en memoria el segundo parámetro en
	* 		 la dirección del primer parámetro.
	*/
	void ejecutar_write(int direccion_logica, int a_escribir, t_pcb* pcb, int socket_cliente);

	/**
	* @NAME: ejecutar_copy
	* @DESC:
	*/
	void ejecutar_copy(int direccion_logica_destino, int direccion_logica_origen, t_pcb* pcb, int socket_cliente);

#endif /* INCLUDE_CICLO_INSTRUCCION_EJECUCION_INSTRUCCION_H_ */

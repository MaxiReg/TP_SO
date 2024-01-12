#ifndef INCLUDE_SERIALIZACION_SERIALIZACION_PETICION_MEMORIA_H_
#define INCLUDE_SERIALIZACION_SERIALIZACION_PETICION_MEMORIA_H_

/* ******* BIBLIOTECA ******* */
	// ESTÁNDAR
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>

	// MISCELÁNEO
	#include <netdb.h>
	#include <signal.h>
	#include <sys/socket.h>

	// COMMONS
	#include <commons/log.h>
	#include <commons/config.h>
	#include <commons/collections/list.h>

	// COMPARTIDO
	#include <serializacion/serializacion.h>

	/* ******** ESTRUCTURAS ******* */
	typedef struct {
		uint32_t entrada_tabla_1er_nivel;
		uint32_t numero_pagina;
		uint32_t pid;
		uint32_t tabla_1er_nivel;
		uint32_t tamanio_proceso;
	} t_peticion_memoria_indice;

	typedef struct {
		uint32_t pagina;
		uint32_t marco;
		uint32_t desplazamiento;
		uint32_t a_escribir;
	} t_peticion_memoria_io;

	/* ******** FUNCIONES ******* */
	/**
	* @NAME: agregar_peticion_memoria_indice_a_paquete
	* @DESC:
	*/
	void agregar_peticion_memoria_indice_a_paquete(t_paquete* paquete, t_peticion_memoria_indice* peticion_memoria_indice);

	/**
	* @NAME: agregar_peticion_memoria_io_a_paquete
	* @DESC:
	*/
	void agregar_peticion_memoria_io_a_paquete(t_paquete* paquete, t_peticion_memoria_io* peticion_memoria_io);

	/**
	* @NAME: recibir_peticion_memoria_indice
	* @DESC:
	*/
	t_peticion_memoria_indice* recibir_peticion_memoria_indice(int socket_cliente);


	/**
	* @NAME: recibir_peticion_memoria_io
	* @DESC:
	*/
	t_peticion_memoria_io* recibir_peticion_memoria_io(int socket_cliente);

#endif /* INCLUDE_SERIALIZACION_SERIALIZACION_PETICION_MEMORIA_H_ */

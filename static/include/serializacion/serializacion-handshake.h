#ifndef INCLUDE_SERIALIZACION_SERIALIZACION_HANDSHAKE_H_
#define INCLUDE_SERIALIZACION_SERIALIZACION_HANDSHAKE_H_

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
	#include <commons/log.h>
	#include <commons/config.h>
	#include <commons/collections/list.h>

	// COMMONS
	#include <serializacion/serializacion.h>

	/* ******** ESTRUCTURAS ******* */
	typedef struct {
		uint32_t cantidad_entradas_tabla_paginas;
		uint32_t tamanio_pagina;
	} t_config_handshake;

	/* ******** FUNCIONES ******* */
	// SERIALIZACIÓN
	/**
	* @NAME: enviar_paquete_handshake
	* @DESC: Permite agregar un elemento a un paquete
	*/
	void enviar_paquete_handshake(t_config_handshake* configuraciones, int conexion);

	// DE-SERIALIZACIÓN
	/**
	* @NAME: recibir_paquete_handshake
	* @DESC: Recibe un t_paquete y lo convierte en una t_list*
	*/
	t_config_handshake* recibir_paquete_handshake(int socket_cliente);


#endif /* INCLUDE_SERIALIZACION_SERIALIZACION_HANDSHAKE_H_ */

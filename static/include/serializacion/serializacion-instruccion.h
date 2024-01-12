#ifndef INCLUDE_SOCKETS_SERIALIZACION_INSTRUCCION_H_
#define INCLUDE_SOCKETS_SERIALIZACION_INSTRUCCION_H_

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
	typedef enum {
		NO_OP,
		I_O,
		READ,
		WRITE,
		COPY,
		EXIT_I
	} e_id_operacion;

	typedef struct {
		uint8_t identificador;
		uint32_t parametros[2];
	} t_instruccion;

	/* ******** FUNCIONES ******* */
	// SERIALIZACIÓN
	/**
	* @NAME: agregar_tamanio_proceso_a_paquete
	* @DESC: Permite agregar socketsun elemento a un paquete
	*/
	void agregar_tamanio_proceso_a_paquete(t_paquete* paquete, uint32_t tamanio_proceso);

	/**
	* @NAME: agregar_instruccion_a_paquete
	* @DESC: Permite agregar un elemento a un paquete
	*/
	void agregar_instruccion_a_paquete(t_paquete* paquete, t_instruccion* instruccion);

	// DE-SERIALIZACIÓN
	/**
	* @NAME: recibir_tamanio_proceso
	* @DESC: Recibe un tamanio de un proceso
	*/
	uint32_t recibir_tamanio_proceso(int socket_cliente);

	/**
	* @NAME: recibir_paquete_instrucciones
	* @DESC: Recibe un t_paquete y lo convierte en una t_list*
	*/
	t_list* recibir_paquete_instrucciones(int socket_cliente);

	// GENERAL
	/**
	* @NAME: cantidad_parametros_instruccion
	* @DESC: Recibe un identificador de instruccion y devuelve su cantidad de parametros
	*/
	int cantidad_parametros_instruccion(e_id_operacion identificador_operacion);

	/**
	* @NAME: imprimir_instrucciones
	* @DESC: Imprime una lista de instrucciones en un logger
	*/
	void imprimir_instrucciones(t_log* logger, t_list* l_instrucciones);

	/**
	* @NAME: enviar_paquete_instrucciones
	* @DESC: Serializa, envia y elimina el paquete de instrucciones
	*/
	void enviar_paquete_instrucciones(int conexion, t_list* l_instrucciones, uint32_t tamanio_proceso);


#endif /* INCLUDE_SOCKETS_SERIALIZACION_INSTRUCCION_H_ */

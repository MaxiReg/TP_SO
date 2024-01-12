#ifndef INCLUDE_SERIALIZACION_SERIALIZACION_INSTRUCCION_H_
#define INCLUDE_SERIALIZACION_SERIALIZACION_INSTRUCCION_H_

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

	/* ****** ESTRUCTURAS ******* */
	typedef enum {
		FIN = -1,
		INSTRUCCIONES,
		PCB,
		INTERRUPCION,
		HANDSHAKE,
		INDICE_MARCO,
		PCB_REDUCIDO
	} e_codigo_operacion;

	typedef struct {
		uint32_t tamanio;
		void* stream;
	} t_buffer;

	typedef struct {
		e_codigo_operacion codigo_operacion;
		t_buffer* buffer;
	} t_paquete;

	/* ******** FUNCIONES ******* */
	// SERIALIZACIÓN
	/**
	* @NAME: crear_paquete
	* @DESC: Crea un t_paquete vacio
	*/
	t_paquete* crear_paquete(e_codigo_operacion codigo_operacion);

	/**
	* @NAME: agregar_elemento_a_paquete
	* @DESC: Permite agregar un elemento generico a un paquete, dado su tamaño
	*/
	void agregar_elemento_a_paquete(t_paquete* paquete, void* stream, uint32_t tamanio_stream);

	/**
	* @NAME: serializar_paquete
	* @DESC: Convierte un t_paquete en un void*, para poder ser enviado por un socket
	*/
	void* serializar_paquete(t_paquete* paquete, uint32_t tamanio_stream);

	/**
	* @NAME: enviar_paquete
	* @DESC: Envia un t_paquete por un socket
	*/
	void enviar_paquete(t_paquete* paquete, int socket_cliente);

	/**
	* @NAME: enviar_mensaje
	* @DESC: Envia un mensaje por un socket
	*/
	void enviar_mensaje(uint32_t mensaje, uint32_t tamanio, int socket_cliente);

	/**
	* @NAME: enviar_paquete
	* @DESC: Elimina completamente un t_paquete
	*/
	void eliminar_paquete(t_paquete* paquete);

	// DE-SERIALIZACIÓN
	/**
	* @NAME: recibir_buffer
	* @DESC: Obtiene el buffer del t_paquete recibido
	*/
	void* recibir_buffer(uint32_t* tamanio, int socket_cliente);

	/**
	* @NAME: recibir_paquete
	* @DESC: Recibe un t_paquete y lo convierte en una t_list*
	*/
	t_list* recibir_paquete(int socket_cliente);

	/**
	* @NAME: recibir_operacion
	* @DESC: Obtiene el codigo de operacion de un t_paquete recibido
	*/
	int recibir_operacion(int socket_cliente);

	/**
	* @NAME: recibir_mensaje
	* @DESC: Recibe un mensaje por un socket
	*/
	void* recibir_mensaje(int socket_cliente);

#endif /* SERIALIZACION_SERIALIZACION_INSTRUCCION_H_ */

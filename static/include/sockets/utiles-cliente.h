#ifndef INCLUDE_SOCKETS_UTILES_CLIENTE_H_
#define INCLUDE_SOCKETS_UTILES_CLIENTE_H_
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

	/* ******** FUNCIONES ******* */
	/**
	* @NAME: crear_conexion
	* @DESC: Crea un socket y lo conecta a un servidor previamente iniciado
	*/
	int crear_conexion(char* ip, char* puerto);

	/**
	* @NAME: terminar_conexion
	* @DESC: Cierra la conexion de un socket
	*/
	void terminar_conexion(int conexion);

#endif /* SOCKETS_UTILES_CLIENTE_H_ */

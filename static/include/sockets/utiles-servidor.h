#ifndef INCLUDE_SOCKETS_UTILES_SERVIDOR_H_
#define INCLUDE_SOCKETS_UTILES_SERVIDOR_H_
	/* ******* BIBLIOTECA ******* */
	// ESTÁNDAR
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>

	// MISCELÁNEO
	#include <assert.h>
	#include <netdb.h>
	#include <sys/socket.h>
	#include <unistd.h>

	// COMMONS
	#include <commons/log.h>
	#include <commons/config.h>
	#include <commons/collections/list.h>

	/* ******** FUNCIONES ******* */
	/**
	* @NAME: crear_conexion
	* @DESC: Crea un socket y lo deja esperando a un cliente posteriormente iniciado
	*/
	int iniciar_servidor(char* ip, char* puerto);

	/**
	* @NAME: aceptar_cliente
	* @DESC: Acepta a un cliente que intenta conectarse al puerto donde esta el socket del servidor
	*/
	int aceptar_cliente(/*t_log* logger, const char* name,*/ int socket_servidor);

#endif /* SOCKETS_UTILES_SERVIDOR_H_ */

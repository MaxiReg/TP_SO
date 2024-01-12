#ifndef INCLUDE_COMUNICACIONCPU
#define INCLUDE_COMUNICACIONCPU

// COMMONS
	#include <commons/log.h>
	#include <commons/config.h>

// ESTANDAR
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>

	#include <kernel.h>

// COMPARTIDO
	#include <sockets/utiles-cliente.h>
	#include <sockets/utiles-servidor.h>
	#include <serializacion/serializacion-pcb.h>
	#include <serializacion/serializacion-handshake.h>

// FUNCIONES

void cargar_paquete_pcb(int conexion,t_pcb* proceso);

void ciclo_instruccion();

void enviar_interrupcion();


#endif /* INCLUDE_COMUNICACIONCPU */

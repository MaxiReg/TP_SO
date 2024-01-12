#ifndef INCLUDE_SERIALIZACION_SERIALIZACION_PCB_H_
#define INCLUDE_SERIALIZACION_SERIALIZACION_PCB_H_

	/* ******* BIBLIOTECA ******* */
	// ESTÁNDAR
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>

	// MISCELÁNEO
	#include <netdb.h>
	#include <signal.h>
	#include <sys/socket.h>
	#include <time.h>
	#include <unistd.h>

	// COMMONS
	#include <commons/log.h>
	#include <commons/config.h>
	#include <commons/collections/list.h>
	#include <commons/string.h>

	// COMPARTIDO
	#include <serializacion/serializacion.h>
	#include <serializacion/serializacion-instruccion.h>

	/* ******** ESTRUCTURAS ******* */
	typedef enum {
		NEW,
		READY,
		EXEC,
		BLOCKED,
		SUSPENDED_BLOCKED,
		SUSPENDED_READY,
		EXIT_P
	} e_estado_proceso;

	typedef struct {
		// Planificacion General
		uint16_t pid;
		uint32_t tamanio;
		t_list* l_instrucciones;
		uint8_t program_counter;
		uint32_t tabla_paginas;
		uint32_t tiempo_bloqueo;
		uint8_t estado;

		// Planificacion SRT
		int64_t estimacion_rafaga;
		int64_t estimacion_rafaga_anterior;
		int64_t tiempo_entro_a_ejecutar;
		int64_t tiempo_salio_de_ejecutar;
	} t_pcb;

	typedef struct {
		uint16_t pid;
		uint32_t tamanio;
		uint32_t tabla_paginas;
		uint8_t estado;
	} t_pcb_reducido;

	/* ******** FUNCIONES ******* */
	// SERIALIZACIÓN
	/**
	* @NAME: agregar_instruccion_a_paquete
	* @DESC: Permite agregar un PCB a un paquete
	*/
	void agregar_pcb_a_paquete(t_paquete* paquete, t_pcb* pcb);

	/**
	* @NAME: agregar_pcb_reducido_a_paquete
	* @DESC: Permite agregar un PCB reducido a un paquete
	*/
	void agregar_pcb_reducido_a_paquete(t_paquete* paquete, t_pcb_reducido* pcb_reducido);

	/**
	* @NAME: enviar_pcb_reducido
	* @DESC: Enivarun PCB reducido
	*/
	void enviar_pcb_reducido(t_pcb* pcb, int socket_cliente);

	// DE-SERIALIZACIÓN
	/**
	* @NAME: recibir_paquete_instrucciones
	* @DESC: Recibe un t_paquete y lo convierte en una t_pcb*
	*/
	t_pcb* recibir_paquete_pcb(int socket_cliente);

	/**
	* @NAME: recibir_paquete_pcb_reducido
	* @DESC: Recibe un t_paquete y lo convierte en una 	t_pcb* recibir_paquete_pcb(int socket_cliente);
	*/
	t_pcb_reducido* recibir_paquete_pcb_reducido(int socket_cliente);

	// GENERAL
	/**
	* @NAME: convertir_a_horario
	* @DESC: Convierte un UNIX timestamp en un string hh:mm:ss
	*/
	char* convertir_a_horario(time_t time);

	/**
	* @NAME: imprimir_pcb
	* @DESC: Imprime un PCB con las instrucciones
	*/
	void imprimir_pcb_completo(t_log* logger, t_pcb* pcb);

	/**
	* @NAME: imprimir_pcb_completo
	* @DESC: Imprime un PCB sin las instrucciones
	*/
	void imprimir_pcb_incompleto(t_log* logger, t_pcb* pcb);

	/**
	* @NAME: imprimir_pcb_mini
	* @DESC: Imprime un PCB muy basico
	*/
	void imprimir_pcb_mini(t_log* logger, t_pcb* pcb);

	/**
	* @NAME: liberar_pcb
	* @DESC: Libera un PCB
	*/
	void liberar_pcb(t_pcb* pcb);

#endif /* SERIALIZACION_SERIALIZACION_PCB_H_ */

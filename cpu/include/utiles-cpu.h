#ifndef INCLUDE_UTILES_CPU_H_
#define INCLUDE_UTILES_CPU_H_

	/* ******* BIBLIOTECA ******* */
	// ESTÁNDAR
	#include <stdio.h>
	#include <stdlib.h>
	#include <unistd.h>
	#include <math.h>

	// MISCELÁNEO
	#include <pthread.h>
	#include <readline/readline.h>

	// COMMONS
	#include <commons/collections/list.h>
	#include <commons/config.h>
	#include <commons/log.h>
	#include <commons/string.h>

	// COMPARTIDO
	#include <sockets/utiles-servidor.h>
	#include <sockets/utiles-cliente.h>

	#include <serializacion/serializacion-handshake.h>
	#include <serializacion/serializacion-instruccion.h>
	#include <serializacion/serializacion-pcb.h>

	/* ******** ESTRUCTURAS ******* */
	// GENERAL
	t_log* logger;

	t_list* tlb;

	typedef struct {
		uint8_t entradas_tlb;
		uint8_t reemplazo_tlb;
		uint16_t retardo_noop;
		char* ip;
		char* puerto_memoria;
		char* puerto_escucha_dispatch;
		char* puerto_escucha_interrupt;
		uint32_t cantidad_entradas_tabla_paginas;
		uint32_t tamanio_pagina;
	} t_config_cpu;

	t_config_cpu* config_cpu;

	// CICLO DE INSTRUCCION
	typedef enum {
		CICLO_INSTRUCCION,
		INTERRUPT
	} e_conexion;

	// MMU
	typedef enum {
		FIFO,
		LRU
	} e_reemplazo_tlb;

	typedef struct {
		uint32_t numero_pagina;
		uint32_t entrada_tabla_1er_nivel;
		uint32_t entrada_tabla_2do_nivel;
		uint32_t desplazamiento;
	} t_direccion_logica;

	typedef struct {
		uint32_t marco;
		uint32_t desplazamiento;
	} t_direccion_fisica;

	typedef struct {
		uint32_t pagina;
		uint32_t marco;
	} t_entrada_tlb;

	/* ******** FUNCIONES ******* */
	// CONFIGURACIONES
	/**
	* @NAME: imprimir_configuraciones_cpu
	* @DESC: Loguea las configuraciones de la CPU
	*/
	void imprimir_configuraciones_cpu();

	/**
	* @NAME: crear_config
	* @DESC:
	*/
	t_config* crear_config(char* directorio_actual, char* direccion_config);

	/**
	* @NAME: obtener_configuraciones
	* @DESC: Convierte un t_config* en un t_cpu_config*
	*/
	t_config_cpu* obtener_configuraciones_cpu(t_config* config);

	/**
	* @NAME: handshake_cpu
	* @DESC: Realiza el proceso completo de Handshake con Memoria
	*/
	void handshake_cpu();

	/**
	* @NAME: liberar_config_cpu
	* @DESC: Libera un t_cpu_config* y sus componentes
	*/
	void liberar_config_cpu();

	// MMU
	/**
	* @NAME: imprimir_direccion_logica
	* @DESC: Loguea una direccion logica
	*/
	void imprimir_direccion_logica(t_direccion_logica* direccion_logica);

	/**
	* @NAME: imprimir_entrada_tlb
	* @DESC: Loguea una entrada de TLB
	*/
	void imprimir_entrada_tlb(t_entrada_tlb* entrada_tlb);

	#endif /* INCLUDE_UTILES_CPU_H_ */

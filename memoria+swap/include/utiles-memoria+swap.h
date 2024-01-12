#ifndef INCLUDE_UTILES_MEMORIA_SWAP_H_
#define INCLUDE_UTILES_MEMORIA_SWAP_H_

	/* ******* BIBLIOTECA ******* */
	// ESTÁNDAR
	#include <stdio.h>
	#include <stdlib.h>
	#include <unistd.h>
	#include <math.h>

	// MISCELÁNEO
	#include <pthread.h>
	#include <readline/readline.h>
	#include <semaphore.h>
	#include <fcntl.h> //open()
	#include <sys/mman.h>
	#include <sys/stat.h>

	// COMMONS
	#include <commons/bitarray.h>
	#include <commons/log.h>
	#include <commons/string.h>
	#include <commons/config.h>

	// INTERNO
	#include <gestion-espacio-libre.h>

	// COMPARTIDO
	#include <sockets/utiles-servidor.h>
	#include <sockets/utiles-cliente.h>

	#include <serializacion/serializacion-handshake.h>
	#include <serializacion/serializacion-instruccion.h>
	#include <serializacion/serializacion-pcb.h>
	#include <serializacion/serializacion-peticion-memoria.h>

	/* ******** ESTRUCTURAS ******* */
	// GENERAL
	t_log* logger;
	sem_t sem_conexiones;
	sem_t sem_swap;

	typedef enum {
		KERNEL,
		SWAP,
		CPU
	} e_conexion;

	typedef struct {
		char* ip;
		char* puerto_escucha;
		uint32_t tamanio_memoria;
		uint32_t tamanio_pagina;
		uint32_t cantidad_entradas_tabla_paginas;
		uint16_t retardo_memoria;
		uint8_t algoritmo_reemplazo;
		uint16_t marcos_por_proceso;
		uint16_t retardo_swap;
		char* path_swap;
	} t_config_memoria;

	t_config_memoria* config_memoria;

	// MEMORIA
	void* memoria;
	pthread_mutex_t mx_memoria;
	pthread_mutex_t mx_marcos_libres;
	t_list* l_marcos_libres;
	t_list* l_tabla_paginas;

	typedef struct {
		uint16_t index;
		t_list* l_entradas_tp_2do_nivel;
	} t_tp_2do_nivel;

	typedef struct {
		uint16_t index;
		uint32_t marco;
		uint8_t bit_presencia;
		uint8_t bit_uso;
		uint8_t bit_modificado;
	} t_entrada_tp_2do_nivel;

	typedef struct {
		uint8_t puntero;
		t_list* l_marcos_asignados;
	} t_marcos_asignados;

	// SWAP
	sem_t sem_sincronizar_peticiones;
	sem_t sem_realizar_peticion;
	sem_t sem_esperar_swap;


	typedef enum {
		REEMPLAZAR_PAGINA = 10,
		TRAER_PAGINA
	} e_operacion_swap;

	typedef struct {
		int codigo_peticion;
		t_pcb_reducido* pcb_reducido;
		uint32_t pid_reemplazo;
		uint32_t numero_pagina;
		t_entrada_tp_2do_nivel* entrada_tp_2do_nivel;
		t_entrada_tp_2do_nivel* entrada_tp_2do_nivel_a_intercambiar;
	} t_peticion_swap;

	t_peticion_swap* peticion_swap;

	// ALGORITMO
	typedef enum {
		CLOCK,
		CLOCK_M
	} e_reemplazo_paginas;

	typedef struct {
		int puntero;
		t_list* l_marcos_asignados;
	} t_algoritmo_reemplazo;

	t_list* l_marcos_por_proceso;

	/* ******** FUNCIONES ******* */
	// CONEXIONES
	/**
	* @NAME: comunicacion_kernel
	* @DESC:
	*/
	void comunicacion_kernel(int socket_servidor);

	/**
	* @NAME: conexion_swap
	* @DESC:
	*/
	void conexion_swap();

	/**
	* @NAME: comunicacion_cpu
	* @DESC:
	*/
	void comunicacion_cpu(int socket_servidor);

	// GESTION MEMORIA
	/**
	* @NAME: retardar_memoria
	* @DESC:
	*/
	void retardar_memoria();

	// REALIZAR PETICIONES
	/**
	* @NAME: realizar_peticion_swap
	* @DESC:
	* */
	void realizar_peticion_swap_kernel(t_pcb_reducido* pcb_reducido);

	/**
	* @NAME: realizar_peticion_swap
	* @DESC:
	* */
	void realizar_peticion_swap_cpu(t_entrada_tp_2do_nivel* entrada_tp_2do_nivel, t_entrada_tp_2do_nivel* entrada_tp_2do_nivel_a_intercambiar, t_peticion_memoria_indice* peticion_memoria_indice);

	/**
	* @NAME: realizar_peticion_swap
	* @DESC:
	* */
	void realizar_peticion_swap_cpu_bis(t_entrada_tp_2do_nivel* entrada_tp_2do_nivel, int pid);

	/**
	* @NAME: escribir_memoria
	* @DESC:
	*/
	uint32_t escribir_memoria(uint32_t numero_marco, uint32_t desplazamiento_escritura, uint32_t a_escribir);

	/**
	* @NAME: leer_memoria
	* @DESC:
	*/
	uint32_t leer_memoria(uint32_t numero_marco, uint32_t desplazamiento_lectura);

	// CONFIGURACIONES
	/**
	* @NAME: imprimir_configuraciones_memoria
	* @DESC: Loguea las configuraciones de la Memoria
	*/
	void imprimir_configuraciones_memoria();

	/**
	* @NAME: crear_config
	* @DESC:
	*/
	t_config* crear_config(char* directorio_actual, char* direccion_config);

	/**
	* @NAME: obtener_configuraciones
	* @DESC: Convierte un t_config* en un t_cpu_config*
	*/
	t_config_memoria* obtener_configuraciones_memoria(t_config* config);

	/**
	* @NAME: handshake_memoria
	* @DESC: Realiza el proceso completo de Handshake con CPU
	*/
	void handshake_memoria(t_config_memoria* config_memoria, int socket_servidor);

	/**
	* @NAME: liberar_config_memoria
	* @DESC: Libera un config_memoria* y sus componentes
	*/
	void liberar_config_memoria(t_config_memoria* config_memoria);

	/**
	* @NAME: liberar_estructuras_generales
	* @DESC:
	*/
	void liberar_estructuras_generales();

#endif /* INCLUDE_UTILES_MEMORIA_SWAP_H_ */

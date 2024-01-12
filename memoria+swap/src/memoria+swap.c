#include <memoria+swap.h>

int main(void) {
	/* ******** CREAR ESTRUCTURAS GENERALES ******* */
	// 1. Crear logger
	logger = log_create("log/memoria.log", "mem", 1, LOG_LEVEL_INFO);
	log_info(logger, "[============= CREAR ESTRUCTURAS GENERALES =============]");

	// 2. Obtener la directorio actual
	char* directorio_actual = getcwd(NULL, 0);
	log_info(logger, "   - DIR. ACTUAL: %s", directorio_actual);

	// 3. Obtener configuraciones relevantes
	t_config* config = crear_config(directorio_actual, "/cfg/memoria+swap.config");
	log_info(logger, "   - CONFIG CREADO");

	// 3. b. Obtener configuraciones de Memoria
	config_memoria = obtener_configuraciones_memoria(config);
	log_info(logger, "   - CONFIGURACIONES OBTENIDAS");

	config_destroy(config);
	log_info(logger, "   - CONFIG DESTRUIDO");
	free(directorio_actual);

	// 3. c. Enviar configuraciones a CPU (Handshake)
	int socket_servidor = iniciar_servidor(config_memoria->ip, config_memoria->puerto_escucha);
	handshake_memoria(config_memoria, socket_servidor);

	log_info(logger, "[=======================================================]\n");

	// 4. Imprimir configuraciones
	imprimir_configuraciones_memoria();

	/* ************* GESTION DE MEMORIA *********** */
	// 1. Crear memoria
	memoria = malloc(config_memoria->tamanio_memoria);
	memset(memoria, '0', config_memoria->tamanio_memoria);

	// 1. b. Crear lista de marcos ocupados
	l_marcos_libres = crear_l_marcos_libres(config_memoria->tamanio_memoria, config_memoria->tamanio_pagina);

	// 2. Crear espacio para las tablas de paginas
	l_tabla_paginas = list_create();

	// 3. Crear espacio estructuras de algoritmos
	l_marcos_por_proceso = list_create();

	/* ********** RESPUESTA A PETICIONES ********** */
	// 1. Crear espacio para realizar peticiones a SWAP
	peticion_swap = malloc(sizeof(t_peticion_swap));

	// 2. Comunicacion Kernel, CPU y SWAP
	pthread_t h_conexion[3];

	sem_init(&sem_conexiones, 0, 0);
	sem_init(&sem_sincronizar_peticiones, 0, 1);
	sem_init(&sem_realizar_peticion, 0, 0);
	sem_init(&sem_esperar_swap, 0, 0);
	//pthread_mutex_init(&mx_memoria, NULL);
	//pthread_mutex_init(&mx_marcos_libres, NULL);


	pthread_create(&h_conexion[CPU], NULL, (void*) comunicacion_cpu, (void*) socket_servidor);
	pthread_create(&h_conexion[KERNEL], NULL, (void*) comunicacion_kernel, (void*) socket_servidor);
	pthread_create(&h_conexion[SWAP], NULL, (void*) conexion_swap, NULL);

	/* ******** DESTRUCCION DE ESTRUCTURAS ******** */
	// 1. Destuccion hilos
	pthread_join(h_conexion[CPU], NULL);
	pthread_join(h_conexion[KERNEL], NULL);
	pthread_join(h_conexion[SWAP], NULL);

	// 2. Cerrar conexion
	close(socket_servidor);

	// 3. Liberar estructuras generales
	liberar_estructuras_generales();

    return EXIT_SUCCESS;
}





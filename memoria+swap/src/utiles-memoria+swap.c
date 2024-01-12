#include <utiles-memoria+swap.h>

void retardar_memoria() {
	int retardo_memoria_microsegundos = config_memoria->retardo_memoria * 1000;
	usleep(retardo_memoria_microsegundos);
}

void realizar_peticion_swap_kernel(t_pcb_reducido* pcb_reducido) {
	sem_wait(&sem_sincronizar_peticiones);

	peticion_swap->codigo_peticion = pcb_reducido->estado;
	peticion_swap->pcb_reducido = pcb_reducido;
	peticion_swap->pid_reemplazo = (uint32_t) NULL;
	peticion_swap->numero_pagina = (uint32_t) NULL;
	peticion_swap->entrada_tp_2do_nivel = NULL;

	sem_post(&sem_realizar_peticion);
	sem_wait(&sem_esperar_swap);
}

void realizar_peticion_swap_cpu(t_entrada_tp_2do_nivel* entrada_tp_2do_nivel, t_entrada_tp_2do_nivel * entrada_tp_2do_nivel_a_intercambiar, t_peticion_memoria_indice* peticion_memoria_indice) {
	sem_wait(&sem_sincronizar_peticiones);

	peticion_swap->codigo_peticion = REEMPLAZAR_PAGINA;
	peticion_swap->pcb_reducido = NULL;
	peticion_swap->pid_reemplazo = peticion_memoria_indice->pid;
	peticion_swap->numero_pagina = peticion_memoria_indice->numero_pagina;
	peticion_swap->entrada_tp_2do_nivel = entrada_tp_2do_nivel;
	peticion_swap->entrada_tp_2do_nivel_a_intercambiar = entrada_tp_2do_nivel_a_intercambiar;

	sem_post(&sem_realizar_peticion);
	sem_wait(&sem_esperar_swap);
}

void realizar_peticion_swap_cpu_bis(t_entrada_tp_2do_nivel* entrada_tp_2do_nivel, int pid) {
	sem_wait(&sem_sincronizar_peticiones);

	peticion_swap->codigo_peticion = TRAER_PAGINA;
	peticion_swap->pcb_reducido = NULL;
	peticion_swap->pid_reemplazo = pid;
	peticion_swap->numero_pagina = entrada_tp_2do_nivel->index;
	peticion_swap->entrada_tp_2do_nivel = entrada_tp_2do_nivel;
	peticion_swap->entrada_tp_2do_nivel_a_intercambiar = NULL;

	sem_post(&sem_realizar_peticion);
	sem_wait(&sem_esperar_swap);
}

uint32_t escribir_memoria(uint32_t numero_marco, uint32_t desplazamiento_escritura, uint32_t a_escribir) {
	uint32_t desplazamiento = numero_marco * config_memoria->tamanio_pagina + desplazamiento_escritura;

	// Escribir valor en memoria
	pthread_mutex_lock(&mx_memoria);
	memcpy(memoria + desplazamiento, &(a_escribir), sizeof(a_escribir));
	pthread_mutex_unlock(&mx_memoria);

	return 1;
}

uint32_t leer_memoria(uint32_t numero_marco, uint32_t desplazamiento_lectura) {
	uint32_t desplazamiento = numero_marco * config_memoria->tamanio_pagina + desplazamiento_lectura;

	// Leer valor de memoria
	uint32_t leido;

	pthread_mutex_lock(&mx_memoria);
	memcpy(&leido, memoria + desplazamiento, sizeof(leido));
	pthread_mutex_unlock(&mx_memoria);


	return leido;
}

void imprimir_configuraciones_memoria() {
	log_info(logger, "[************* CONFIGURACIONES MEMORIA+SWAP ************]");

	log_info(logger, "   * TAMAÑO DE MEMORIA ............ %d", config_memoria->tamanio_memoria);
	log_info(logger, "   * TAMAÑO DE PAGINA ............. %d", config_memoria->tamanio_pagina);
	log_info(logger, "   * ENTRADAS TABLA PAGINA ........ %d", config_memoria->cantidad_entradas_tabla_paginas);
	log_info(logger, "   * RETARDO MEMORIA .............. %d", config_memoria->retardo_memoria);

	if(config_memoria->algoritmo_reemplazo == CLOCK)
		log_info(logger, "   * ALG. DE REEMPLAZO ............ CLOCK");
	else
		log_info(logger, "   * ALG. DE REEMPLAZO ............ CLOCK-M");

	log_info(logger, "   * MARCOS P/ PROCESO ............ %d", config_memoria->marcos_por_proceso);
	log_info(logger, "   * RETARDO SWAP ................. %d", config_memoria->retardo_swap);
	log_info(logger, "   * PATH SWAP .................... %s", config_memoria->path_swap);

	log_info(logger, "[*******************************************************]\n");
}

t_config* crear_config(char* directorio_actual, char* direccion_config) {
	char* direccion = string_new();

	string_append_with_format(&direccion, "%s%s", directorio_actual, direccion_config);

	return config_create(direccion);
}

t_config_memoria* obtener_configuraciones_memoria(t_config* config) {
	t_config_memoria* config_memoria = malloc(sizeof(t_config_memoria));

	// Obtener configuraciones de Memoria
	config_memoria->tamanio_memoria = config_get_int_value(config, "TAM_MEMORIA");
	config_memoria->tamanio_pagina = config_get_int_value(config, "TAM_PAGINA");
	config_memoria->cantidad_entradas_tabla_paginas = config_get_int_value(config, "ENTRADAS_POR_TABLA");
	config_memoria->retardo_memoria = config_get_int_value(config, "RETARDO_MEMORIA");
	config_memoria->marcos_por_proceso = config_get_int_value(config, "MARCOS_POR_PROCESO");
	config_memoria->retardo_swap = config_get_int_value(config, "RETARDO_SWAP");

	char* path_swap = config_get_string_value(config, "PATH_SWAP");
	config_memoria->path_swap = strdup(path_swap);

	char* algoritmo_reemplazo = config_get_string_value(config, "ALGORITMO_REEMPLAZO");
	if(!strcmp(algoritmo_reemplazo, "CLOCK")) {
		config_memoria->algoritmo_reemplazo = CLOCK;
	} else if(!strcmp(algoritmo_reemplazo, "CLOCK-M")) {
		config_memoria->algoritmo_reemplazo = CLOCK_M;
	}

	// Obtener configuraciones de sockets
	config_memoria->ip = "0.0.0.0";

	char* puerto_escucha = config_get_string_value(config, "PUERTO_ESCUCHA");
	config_memoria->puerto_escucha = strdup(puerto_escucha);

	return config_memoria;
}

void handshake_memoria(t_config_memoria* config_memoria, int socket_servidor) {
	int socket_cliente = aceptar_cliente(socket_servidor);

	t_config_handshake* config_handshake = malloc(sizeof(t_config_handshake));
	config_handshake->cantidad_entradas_tabla_paginas = config_memoria->cantidad_entradas_tabla_paginas;
	config_handshake->tamanio_pagina = config_memoria->tamanio_pagina;

	enviar_paquete_handshake(config_handshake, socket_cliente);
	free(config_handshake);
	log_info(logger, "   - CONFIGURACIONES ENVIADAS");

	uint8_t resultadoHandshake;
	recv(socket_cliente, &resultadoHandshake, sizeof(uint8_t), 0); // TODO: reemplazar por enviar_mensaje()

	if(resultadoHandshake == HANDSHAKE)
		log_info(logger, "   - HANDSHAKE EXITOSO");
	else
		log_info(logger, "   - HANDSHAKE FALLIDO");

	terminar_conexion(socket_cliente); // TODO: DEBERIA MANTENER CONEXION?
}

void liberar_config_memoria(t_config_memoria* config_memoria) {
	free(config_memoria->puerto_escucha);
	free(config_memoria->path_swap);
	free(config_memoria);
}

void liberar_estructuras_generales() {
	liberar_config_memoria(config_memoria);
	log_destroy(logger);
	free(memoria);
	list_destroy(l_marcos_libres);
	list_destroy_and_destroy_elements(l_tabla_paginas, free);
}

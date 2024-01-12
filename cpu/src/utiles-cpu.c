#include <utiles-cpu.h>

void imprimir_configuraciones_cpu() {
	log_info(logger, "[***************** CONFIGURACIONES CPU *****************]");

	log_info(logger, "   * ENTRADAS TLB ................. %d", config_cpu->entradas_tlb);
	log_info(logger, "   * RETARDO NO_OP ................ %d", config_cpu->retardo_noop);

	if(config_cpu->reemplazo_tlb == FIFO)
		log_info(logger, "   * REEMPLAZO TLB ................ FIFO");
	else
		log_info(logger, "   * REEMPLAZO TLB ................ LRU");

	log_info(logger, "   * ENTRADAS TABLA PAGINA ........ %d", config_cpu->cantidad_entradas_tabla_paginas);
	log_info(logger, "   * TAMAÑO DE PAGINA ............. %d", config_cpu->tamanio_pagina);

	log_info(logger, "[*******************************************************]\n");
}

t_config* crear_config(char* directorio_actual, char* direccion_config) {
	char* direccion = string_new();

	string_append(&direccion, directorio_actual);
	string_append(&direccion, direccion_config);

	return config_create(direccion);
}

t_config_cpu* obtener_configuraciones_cpu(t_config* config) {
	t_config_cpu* config_cpu = malloc(sizeof(t_config_cpu));

	// Obtener configuraciones de CPU
	config_cpu->entradas_tlb = config_get_int_value(config, "ENTRADAS_TLB");

	config_cpu->retardo_noop = config_get_int_value(config, "RETARDO_NOOP");

	char* reemplazo_tlb = config_get_string_value(config, "REEMPLAZO_TLB");
	if(!strcmp(reemplazo_tlb, "FIFO")) {
		config_cpu->reemplazo_tlb = FIFO;
	} else if(!strcmp(reemplazo_tlb, "LRU")) {
		config_cpu->reemplazo_tlb = LRU;
	}

	// Obtener configuraciones de sockets
	char* ip = config_get_string_value(config, "IP_MEMORIA");
	config_cpu->ip = strdup(ip);

	char* puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
	config_cpu->puerto_memoria = strdup(puerto_memoria);

	char* puerto_escucha_dispatch = config_get_string_value(config, "PUERTO_ESCUCHA_DISPATCH");
	config_cpu->puerto_escucha_dispatch = strdup(puerto_escucha_dispatch);

	char* puerto_escucha_interrupt = config_get_string_value(config, "PUERTO_ESCUCHA_INTERRUPT");
	config_cpu->puerto_escucha_interrupt = strdup(puerto_escucha_interrupt);

	return config_cpu;
}

void handshake_cpu() {
	// Conectar con Memoria
	int socket_cliente = crear_conexion(config_cpu->ip, config_cpu->puerto_memoria);

	// Obtener configuraciones de Memoria
	uint8_t resultadoHandshake = recibir_operacion(socket_cliente);
	if(resultadoHandshake == HANDSHAKE) {
		t_config_handshake* config_handshake = recibir_paquete_handshake(socket_cliente);
		config_cpu->cantidad_entradas_tabla_paginas = config_handshake->cantidad_entradas_tabla_paginas;
		config_cpu->tamanio_pagina = config_handshake->tamanio_pagina;
		free(config_handshake); // TODO: PUEDE TIRAR ERROR creo
	}

	// Responder a Memoria
	send(socket_cliente, &resultadoHandshake, sizeof(uint8_t), 0);

	close(socket_cliente);
	// TODO: Sirve cerrar conexion de memoria solo, o tambien hay que cerrar este servidor?
}

void liberar_config_cpu() {
	free(config_cpu->ip);
	free(config_cpu->puerto_memoria);
	free(config_cpu->puerto_escucha_dispatch);
	free(config_cpu->puerto_escucha_interrupt);
	free(config_cpu);
}

void imprimir_direccion_logica(t_direccion_logica* direccion_logica) {
	log_info(logger, "     > [MMU] D. LOGICA [ E1 #%05d | E2 #%05d | D #%05d ]", direccion_logica->entrada_tabla_1er_nivel, direccion_logica->entrada_tabla_2do_nivel, direccion_logica->desplazamiento);
}

void imprimir_entrada_tlb(t_entrada_tlb* entrada_tlb) {
	log_info(logger, "     > [TLB] ENTRADA [ P #%05d | M #%05d ]", entrada_tlb->pagina, entrada_tlb->marco);
}

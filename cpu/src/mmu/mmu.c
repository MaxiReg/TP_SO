#include <mmu/mmu.h>

void enviar_peticion_indice(t_pcb* pcb, t_direccion_logica* direccion_logica, int socket_cliente) {
	t_paquete* paquete = crear_paquete(INDICE_MARCO);

	t_peticion_memoria_indice* peticion_memoria_indice = malloc(sizeof(t_peticion_memoria_indice));
	peticion_memoria_indice->entrada_tabla_1er_nivel = direccion_logica->entrada_tabla_1er_nivel;
	peticion_memoria_indice->numero_pagina = direccion_logica->numero_pagina;
	peticion_memoria_indice->pid = pcb->pid;
	peticion_memoria_indice->tabla_1er_nivel = pcb->tabla_paginas;
	peticion_memoria_indice->tamanio_proceso = pcb->tamanio;

	agregar_peticion_memoria_indice_a_paquete(paquete, peticion_memoria_indice);

	enviar_paquete(paquete, socket_cliente);
	free(peticion_memoria_indice);
	eliminar_paquete(paquete);
}

int obtener_indice_tabla_2do_nivel(t_pcb* pcb, t_direccion_logica* direccion_logica, int socket_cliente) {
	int indice_tabla_2do_nivel;

	// a. Peticion a Memoria: indice de tabla de 2do nivel
	enviar_peticion_indice(pcb, direccion_logica, socket_cliente);

	// b. Recepcion de Memoria: indice de tabla de 2do nivel
	indice_tabla_2do_nivel = (int) recibir_mensaje(socket_cliente);

	return indice_tabla_2do_nivel;
}

int obtener_indice_marco_tp(int entrada_tabla_2do_nivel, int socket_cliente) {
	int indice_marco;

	// a. Peticion a Memoria: indice de marco
	enviar_mensaje(entrada_tabla_2do_nivel, sizeof(uint32_t), socket_cliente);

	// b. Recepcion de Memoria: indice de marco
	indice_marco = (int) recibir_mensaje(socket_cliente);

	return indice_marco;
}

t_direccion_fisica* obtener_direccion_fisica(t_direccion_logica* direccion_logica, t_pcb* pcb, int socket_cliente) {
	// 1. Obtener marco (por TLB)
	int indice_marco = obtener_indice_marco_tlb(direccion_logica->numero_pagina);
	//int indice_marco = -1;

	if(indice_marco >= 0)
		log_info(logger, "    * [TLB] HIT");

	// 2. Obtener marco (por TP)
	if(indice_marco < 0) {
		log_info(logger, "    * [TLB] MISS");

		// 2. a. Acceso: Obtener indice de tabla de segundo nivel
		obtener_indice_tabla_2do_nivel(pcb, direccion_logica, socket_cliente);

		// 2. b. Acceso: Obtener marco // 2. c. Obtener referencia para cargar en TLB
		t_entrada_tlb* entrada_tlb = malloc(sizeof(t_entrada_tlb));
		entrada_tlb->pagina = direccion_logica->numero_pagina;
		entrada_tlb->marco = indice_marco = obtener_indice_marco_tp(direccion_logica->entrada_tabla_2do_nivel, socket_cliente);

		// 2. d. Buscar y eliminar entradas invalidas
		eliminar_entrada_invalida(entrada_tlb);

		// 2. e. Agregar referencia para en TLB //TODO: La funcion tiene otro nombre
		agregar_entrada_tlb(entrada_tlb);
	}

	// 3. Crear direccion fisica
	t_direccion_fisica* direccion_fisica = crear_direccion_fisica(indice_marco, direccion_logica->desplazamiento);

	return direccion_fisica;
}

void enviar_peticion_io(int codigo_operacion, t_direccion_fisica* direccion_fisica, int numero_pagina, int a_escribir, int socket_cliente) {
	t_paquete* paquete = crear_paquete(codigo_operacion);

	t_peticion_memoria_io* peticion_memoria_io = malloc(sizeof(t_peticion_memoria_io));
	peticion_memoria_io->pagina = numero_pagina;
	peticion_memoria_io->marco = direccion_fisica->marco;
	peticion_memoria_io->desplazamiento = direccion_fisica->desplazamiento;
	peticion_memoria_io->a_escribir = a_escribir;

	agregar_peticion_memoria_io_a_paquete(paquete, peticion_memoria_io);

	enviar_paquete(paquete, socket_cliente);
	free(peticion_memoria_io);
	eliminar_paquete(paquete);
}

uint32_t obtener_valor_marco(t_direccion_fisica* direccion_fisica, int numero_pagina, int socket_cliente) {
	uint32_t valor_marco;

	// 1. Enviar peticion: READ
	enviar_peticion_io(READ, direccion_fisica, numero_pagina, (uint32_t) NULL, socket_cliente); //TODO PUEDE EXPLOTAR NULL

	// 2. Recbir valor leido
	valor_marco = (uint32_t) recibir_mensaje(socket_cliente);

	// 3. Recibir resultado READ
	uint32_t resultado_operacion = (uint32_t) recibir_mensaje(socket_cliente);

	if(resultado_operacion)
			log_info(logger, "    ~ [MEM] LECTURA EXITOSA");
		else
			log_info(logger, "    ~ [MEM] LECTURA FALLIDA");

	return valor_marco;
}

uint32_t enviar_valor_a_escribir(t_direccion_fisica* direccion_fisica, int numero_pagina, int a_escribir, int socket_cliente) {
	// 1. Enviar peticion: WRITE
	enviar_peticion_io(WRITE, direccion_fisica, numero_pagina, a_escribir, socket_cliente);

	// 2. Recibir resultado WRITE
	uint32_t resultado_operacion = (uint32_t) recibir_mensaje(socket_cliente);

	return resultado_operacion;
}


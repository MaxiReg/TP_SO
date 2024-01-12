#include <comunicacion/gestion-paginas/algoritmo-reemplazo.h>

void mostrar_puntero(t_algoritmo_reemplazo* e_a_reemplazo){ //TODO: Ponela donde quieras Caru, podria ir abajo creo
	if(e_a_reemplazo->puntero >= list_size(e_a_reemplazo->l_marcos_asignados)) {
		log_info(logger, "   - POSICION PUNTERO: 00");
	} else {
		log_info(logger, "   - POSICION PUNTERO: %02d", e_a_reemplazo->puntero);
	}
}
t_entrada_tp_2do_nivel* obtener_entrada_tp_2do_nivel_por_marco(t_list* l_tp_1er_nivel, int indice_marco) {
	t_entrada_tp_2do_nivel* entrada_tp_2do_nivel;

	void recorrer_tp_1er_nivel(t_tp_2do_nivel* tp_2do_nivel) {
		void recorrer_tp_2do_nivel(t_entrada_tp_2do_nivel* entrada_tp_2do_nivel_obtenida) {
			// 3. Contar marco si esta presente
			if(entrada_tp_2do_nivel_obtenida->bit_presencia && entrada_tp_2do_nivel_obtenida->marco == indice_marco)
				entrada_tp_2do_nivel = entrada_tp_2do_nivel_obtenida;
		}

		// 2. Iterar tabla de paginas de 2do nivel
		list_iterate(tp_2do_nivel->l_entradas_tp_2do_nivel, (void*) recorrer_tp_2do_nivel);
	}

	// 1. Iterar tabla de paginas de 1er nivel
	list_iterate(l_tp_1er_nivel, (void*) recorrer_tp_1er_nivel);

	return entrada_tp_2do_nivel;
}

void intercambiar_paginas(t_entrada_tp_2do_nivel* entrada_tp_2do_nivel, t_list* l_tp_1er_nivel, t_peticion_memoria_indice* peticion_memoria_indice, int indice_marco) {
	// 1. Obtener pagina a intercambiar
	t_entrada_tp_2do_nivel* entrada_tp_2do_nivel_a_intercambiar = obtener_entrada_tp_2do_nivel_por_marco(l_tp_1er_nivel, indice_marco);

	// 2. Actualizar tablas de paginas
	// 2. a. Actualizar vieja
	entrada_tp_2do_nivel_a_intercambiar->bit_presencia = 0;

	// 2. b. Actualizar nueva
	inicializar_entrada_tp_2do_nivel(entrada_tp_2do_nivel, indice_marco);
	log_info(logger, "   - MARCO DE REEMPLAZO: #%05d", entrada_tp_2do_nivel->marco);

	// 3. Actualizar lista de marcos por proceso
	t_algoritmo_reemplazo* e_a_reemplazo = list_get(l_marcos_por_proceso, peticion_memoria_indice->pid);
	list_replace(e_a_reemplazo->l_marcos_asignados, e_a_reemplazo->puntero - 1, entrada_tp_2do_nivel);

	// 4. Reemplazar paginas //TODO Reemplazar paginas
	realizar_peticion_swap_cpu(entrada_tp_2do_nivel, entrada_tp_2do_nivel_a_intercambiar, peticion_memoria_indice);
}

uint32_t algoritmo_reemplazo_pagina(t_entrada_tp_2do_nivel* entrada_tp_2do_nivel, t_list* l_tp_1er_nivel, t_peticion_memoria_indice* peticion_memoria_indice) {
	uint32_t indice_marco;

	// 1. Obtener indice de marco para el reemplazo
	switch(config_memoria->algoritmo_reemplazo) {
		case CLOCK:
			indice_marco = obtener_indice_marco_clock(peticion_memoria_indice->pid);
			break;
		case CLOCK_M:
			indice_marco = obtener_indice_marco_clock_m(peticion_memoria_indice->pid);
			break;
	}

	// 2. Intercambiar paginas
	intercambiar_paginas(entrada_tp_2do_nivel, l_tp_1er_nivel, peticion_memoria_indice, indice_marco);

	return indice_marco;
}

void inicializar_entrada_tp_2do_nivel(t_entrada_tp_2do_nivel* entrada_tp_2do_nivel, uint32_t indice_marco) {
	entrada_tp_2do_nivel->marco = indice_marco;
	entrada_tp_2do_nivel->bit_presencia = 1;
	entrada_tp_2do_nivel->bit_uso = 1;
	entrada_tp_2do_nivel->bit_modificado = 0;
}

uint32_t asignar_nuevo_marco(int pid, t_entrada_tp_2do_nivel* entrada_tp_2do_nivel) {
	// 0. Valor por defecto (por si no entra en el condicional)
	uint32_t indice_marco = 0;

	// 1. Buscar el marco libre
	//pthread_mutex_lock(&mx_marcos_libres);
	while(!list_get(l_marcos_libres, indice_marco))
		indice_marco++;

	// 2. Marcar marco como ocupado
	ocupar_marco(l_marcos_libres, indice_marco);
	//pthread_mutex_unlock(&mx_marcos_libres);

	// 3. Inicializar entrada de tabla de 2do nivel
	inicializar_entrada_tp_2do_nivel(entrada_tp_2do_nivel, indice_marco);

	realizar_peticion_swap_cpu_bis(entrada_tp_2do_nivel, pid);

	// 4. Agregar a estructura de algoritmo
	t_algoritmo_reemplazo* e_a_reemplazo = list_get(l_marcos_por_proceso, pid);
	list_add(e_a_reemplazo->l_marcos_asignados, (void*) entrada_tp_2do_nivel);

	return indice_marco;
}

int hay_espacio_memoria() {

	for(int i = 0; i < list_size(l_marcos_libres); i++){
		//pthread_mutex_lock(&mx_marcos_libres);
		if(list_get(l_marcos_libres, i))
			return 1;
		//pthread_mutex_unlock(&mx_marcos_libres);
	}
	log_info(logger, "   - MEMORIA LLENA");
	return 0;
}

int maximo_marcos_por_proceso(t_list* l_tp_1er_nivel) {
	int marcos_por_proceso = 0;

	void contar_marcos_por_tp_2do_nivel(t_tp_2do_nivel* tp_2do_nivel) {
		void contar_marcos(t_entrada_tp_2do_nivel* entrada_tp_2do_nivel) {
			// 3. Contar marco si esta presente
			if(entrada_tp_2do_nivel->bit_presencia)
				marcos_por_proceso++;
		}

		// 2. Iterar tabla de paginas de 2do nivel
		list_iterate(tp_2do_nivel->l_entradas_tp_2do_nivel, (void*) contar_marcos);
	}

	// 1. Iterar tabla de paginas de 1er nivel
	list_iterate(l_tp_1er_nivel, (void*) contar_marcos_por_tp_2do_nivel);

	// 4. Llego al maximo de marcos por proceso
	return marcos_por_proceso == config_memoria->marcos_por_proceso;
}

uint32_t obtener_indice_marco(t_entrada_tp_2do_nivel* entrada_tp_2do_nivel, t_list* l_tp_1er_nivel, t_peticion_memoria_indice* peticion_memoria_indice) {
	// 0. Asignacion ya realizada previamente
	if(entrada_tp_2do_nivel->bit_presencia) {
		log_info(logger, "   - MARCO ASIGNADO: #%05d", entrada_tp_2do_nivel->marco);
		return entrada_tp_2do_nivel->marco;
	}

	// 1. Asignacion (First-Fit)
	if(hay_espacio_memoria() && !maximo_marcos_por_proceso(l_tp_1er_nivel)) {
		asignar_nuevo_marco(peticion_memoria_indice->pid, entrada_tp_2do_nivel);
		log_info(logger, "   - [PF] NUEVO MARCO ASIGNADO: #%05d", entrada_tp_2do_nivel->marco);
		return entrada_tp_2do_nivel->marco;
	}

	// 2. Reemplazo (Clock / Clock-M)
	return algoritmo_reemplazo_pagina(entrada_tp_2do_nivel, l_tp_1er_nivel, peticion_memoria_indice);
}


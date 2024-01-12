#include <comunicacion/gestion-paginas/algoritmo-clock-m.h>

void cambiar_modo_busqueda() {
	switch(modo_busqueda) {
		case BUSQUEDA_BIT_USO:
			modo_busqueda = BUSQUEDA_BIT_MODIFICADO;
			break;
		case BUSQUEDA_BIT_MODIFICADO:
			modo_busqueda = BUSQUEDA_BIT_USO;
			break;
	}
}

uint32_t busqueda_bit_uso(t_entrada_tp_2do_nivel* entrada_tp_2do_nivel, int* puntero) {
	// Si PUM != 1-0-0, seguir recorriendo tabla de paginas
	if(!(!entrada_tp_2do_nivel->bit_uso && !entrada_tp_2do_nivel->bit_modificado)) {
		(*puntero)++;
		return 1;
	}

	return 0;
}

uint32_t busqueda_bit_modificado(t_entrada_tp_2do_nivel* entrada_tp_2do_nivel, int* puntero) {
	// Si PUM != 1-0-1, seguir recorriendo tabla de paginas
	if(!(!entrada_tp_2do_nivel->bit_uso && entrada_tp_2do_nivel->bit_modificado)) {
		// Cambiar Bit
		entrada_tp_2do_nivel->bit_uso = 0;

		// Seguir avanzando
		(*puntero)++;
		return 1;
	}

	return 0;
}

uint32_t avanzar_puntero_m(t_entrada_tp_2do_nivel* entrada_tp_2do_nivel, int* puntero) {
	int avanzar_puntero;

	switch(modo_busqueda) {
		case BUSQUEDA_BIT_USO:
			avanzar_puntero = busqueda_bit_uso(entrada_tp_2do_nivel, puntero);
			break;
		case BUSQUEDA_BIT_MODIFICADO:
			avanzar_puntero = busqueda_bit_modificado(entrada_tp_2do_nivel, puntero);
			break;
	}

	return avanzar_puntero;
}

uint32_t obtener_indice_marco_clock_m(int pid) {
	t_algoritmo_reemplazo* e_a_reemplazo = list_get(l_marcos_por_proceso, pid);

	// 0. Configuraciones iniciales del algoritmo
	int paginas_recorridas = 0;
	modo_busqueda = BUSQUEDA_BIT_USO;


	// 1. Obtener entrada de 2do nivel (pagina) con PUM = 100 y PUM = 101
	t_entrada_tp_2do_nivel* entrada_tp_2do_nivel;
	do {
		// 1. a. Restear puntero al inicio
		if(e_a_reemplazo->puntero >= list_size(e_a_reemplazo->l_marcos_asignados))
			e_a_reemplazo->puntero = 0;

		// 1. b. Obtener pagina a la que apunta el puntero
		entrada_tp_2do_nivel = list_get(e_a_reemplazo->l_marcos_asignados, e_a_reemplazo->puntero);

		// 1. c.
		if(paginas_recorridas >= list_size(e_a_reemplazo->l_marcos_asignados)) {
			paginas_recorridas = 0;
			cambiar_modo_busqueda();
		}

		paginas_recorridas++;
	} while(avanzar_puntero_m(entrada_tp_2do_nivel, &e_a_reemplazo->puntero));

	// 2. Avanzar puntero para siguiente reemplazo
	e_a_reemplazo->puntero++;

	mostrar_puntero(e_a_reemplazo);

	return entrada_tp_2do_nivel->marco;
}

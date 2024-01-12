#include <comunicacion/gestion-paginas/algoritmo-clock.h>

uint32_t avanzar_puntero(t_entrada_tp_2do_nivel* entrada_tp_2do_nivel, int* puntero) {
	// Si PU != 10, seguir recorriendo tabla de paginas
	if(entrada_tp_2do_nivel->bit_uso) {
		entrada_tp_2do_nivel->bit_uso = 0;
		(*puntero)++;
		return 1;
	}

	return 0;
}

uint32_t obtener_indice_marco_clock(int pid) {
	t_algoritmo_reemplazo* e_a_reemplazo = list_get(l_marcos_por_proceso, pid);

	// 1. Obtener entrada de 2do nivel (pagina) con PU = 10
	t_entrada_tp_2do_nivel* entrada_tp_2do_nivel;
	do {
		// 1. a. Restear puntero al inicio
		if(e_a_reemplazo->puntero >= list_size(e_a_reemplazo->l_marcos_asignados))
			e_a_reemplazo->puntero = 0;

		// 1. b. Obtener pagina a la que apunta el puntero
		entrada_tp_2do_nivel = list_get(e_a_reemplazo->l_marcos_asignados, e_a_reemplazo->puntero);
	} while(avanzar_puntero(entrada_tp_2do_nivel, &e_a_reemplazo->puntero));

	// 2. Avanzar puntero para siguiente reemplazo
	e_a_reemplazo->puntero++;

	mostrar_puntero(e_a_reemplazo);

	return entrada_tp_2do_nivel->marco;
}

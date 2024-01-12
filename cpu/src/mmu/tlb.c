#include <mmu/tlb.h>

void limpiar_tlb(){
	list_clean_and_destroy_elements(tlb, free);
}

void eliminar_entrada_invalida(t_entrada_tlb* entrada_tlb) {
	bool entrada_es_invalida(t_entrada_tlb* entrada_tlb_invalida) {
		// 2. Entrada invalida es la que tiene mismo marco, pero diferente pagina
		if(entrada_tlb_invalida->marco == entrada_tlb->marco && entrada_tlb_invalida->pagina != entrada_tlb->pagina) {
			log_info(logger, "    * [TLB] ENTRADA #%02d: [P #%05d | M #%05d] (INVALIDA)", list_size(tlb) - 1, entrada_tlb->pagina, entrada_tlb->marco);
			return true;
		}

		return false;
	}

	// 1. Buscar entrada invalida en la TLB
	list_remove_by_condition(tlb, (void*) entrada_es_invalida);
}

void reemplazar_entrada_tlb(t_entrada_tlb* entrada_tlb){
	t_entrada_tlb* entrada_tlb_eliminada = list_remove(tlb, 0);
	log_info(logger, "    * [TLB] ENTRADA #00: [P #%05d | M #%05d] (BORRADA)", entrada_tlb_eliminada->pagina, entrada_tlb_eliminada->marco);
	free(entrada_tlb_eliminada);

	list_add(tlb, entrada_tlb);
	log_info(logger, "    * [TLB] ENTRADA #%02d: [P #%05d | M #%05d] (AGREGADA)", list_size(tlb) - 1, entrada_tlb->pagina, entrada_tlb->marco);
}

void agregar_entrada_tlb(t_entrada_tlb* entrada_tlb) {
	if(list_size(tlb) < config_cpu->entradas_tlb) {
		list_add(tlb, entrada_tlb);
		log_info(logger, "    * [TLB] ENTRADA #%02d: [P #%05d | M #%05d] (AGREGADA)", list_size(tlb) - 1, entrada_tlb->pagina, entrada_tlb->marco);
	} else {
		reemplazar_entrada_tlb(entrada_tlb);
	}
}

void actualizar_tlb(t_entrada_tlb* entrada_tlb){
	bool compara_elementos(t_entrada_tlb* entrada_tlb_vieja){
		// 2. Buscar entrada referenciada en TLB
		return entrada_tlb_vieja->pagina == entrada_tlb->pagina;
	}

	// 1. Actualizar refrencia
	// 1. a. Buscar entrada referenciada
	list_remove_by_condition(tlb, (void*) compara_elementos);

	// 1. b. Mover al principio entrada referenciada
	list_add(tlb, entrada_tlb);
}

bool son_iguales_a_pagina(t_entrada_tlb* entrada_tlb, int numero_pagina){
	return entrada_tlb->pagina == numero_pagina;
}

int algoritmo_busqueda_lru(int numero_pagina) {
	int indice_marco = -1;

	bool comparar_entrada_con_pagina(t_entrada_tlb* entrada_tlb){
		// 3. Comparar pagina de entrada con pagina buscada
		return entrada_tlb->pagina == numero_pagina;
	}

	// 1. Revisar que la TLB no esta vacia
	if(!list_is_empty(tlb)) {
		// 2. Buscar en TLB
		// 2. a. Comparar pagina buscada, con entradas de TLB
		t_entrada_tlb* entrada_tlb = list_find(tlb, (void*) comparar_entrada_con_pagina);

		// 2. b. Revisar que se encontro una entrada
		if(entrada_tlb != NULL) {
			actualizar_tlb(entrada_tlb);

			indice_marco = entrada_tlb->marco;
		}
	}

	return indice_marco;
}

int algoritmo_busqueda_fifo(int numero_pagina) {
	int indice_marco = -1;

	bool comparar_entrada_con_pagina(t_entrada_tlb* entrada_tlb){
		// 3. Comparar pagina de entrada con pagina buscada
		return son_iguales_a_pagina(entrada_tlb, numero_pagina);
	}

	// 1. Revisar que la TLB no esta vacia
	if(!list_is_empty(tlb)) {
		// 2. Buscar en TLB
		// 2. a. Comparar pagina buscada, con entradas de TLB
		t_entrada_tlb* entrada_tlb = list_find(tlb, (void*) comparar_entrada_con_pagina);

		// 2. b. Revisar que se encontro una entrada
		if(entrada_tlb != NULL) {
			indice_marco = entrada_tlb->marco;
		}
	}

	return indice_marco;
}

int obtener_indice_marco_tlb(int numero_pagina) {
	int indice_marco = -1;

	switch(config_cpu->reemplazo_tlb) {
		case FIFO:
			indice_marco = algoritmo_busqueda_fifo(numero_pagina);
			break;
		case LRU:
			indice_marco = algoritmo_busqueda_lru(numero_pagina);
			break;
	}

	return indice_marco;
}

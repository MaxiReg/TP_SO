#include <swap/swap.h>

void crear_archivo_swap(t_pcb_reducido* pcb_reducido) {
	// 1. Determinar nombre y direccion del archivo SWAP
	char* direccion_swap = crear_direccion_swap(pcb_reducido->pid);

	// 2. Crear descriptor de archivo
	FILE* a_swap = fopen(direccion_swap, "w+");

	// 3. Inicializar archivo
	uint32_t a_escribir = 0;
	for(int n = 0; n < pcb_reducido->tamanio; n += sizeof(uint32_t))
		putw(a_escribir, a_swap);

	// 4. Cerrar descriptor de archivo
	fclose(a_swap);
	free(direccion_swap);
}

void suspender_proceso(t_pcb_reducido* pcb_reducido) {
	void recorrer_tp_1er_nivel(t_tp_2do_nivel* tp_2do_nivel) {
		void recorrer_tp_2do_nivel(t_entrada_tp_2do_nivel* entrada_tp_2do_nivel) {
			//log_info(logger, "   - Pag %d", entrada_tp_2do_nivel->index);

			// 3. Escribir en SWAP marcos modificados
			if(entrada_tp_2do_nivel->bit_modificado) {
				escribir_pagina_swap(pcb_reducido->pid, entrada_tp_2do_nivel->index, entrada_tp_2do_nivel->marco);
				//log_info(logger, "   - escribir_pagina_swap - Pag %d", entrada_tp_2do_nivel->index);
				//retardar_swap();
			}

			// 4. Actualizar lista de marcos libres
			if(entrada_tp_2do_nivel->bit_presencia) {
				log_info(logger, "     ~ MARCO LIBERADO: #%05d (PAGINA: %05d)", entrada_tp_2do_nivel->marco, entrada_tp_2do_nivel->index);
			}

			// 5. Actualizar pagina de tabla de paginas
			entrada_tp_2do_nivel->bit_presencia = 0;
			entrada_tp_2do_nivel->bit_uso = 0;
			entrada_tp_2do_nivel->bit_modificado = 0;
			//log_info(logger, "   - actualizar pagina\n");
		}

		// 2. Iterar tabla de paginas de 2do nivel
		list_iterate(tp_2do_nivel->l_entradas_tp_2do_nivel, (void*) recorrer_tp_2do_nivel);
	}

	// 1. Iterar tabla de paginas de 1er nivel
	t_list* l_tp_1er_nivel = list_get(l_tabla_paginas, pcb_reducido->pid);
	list_iterate(l_tp_1er_nivel, (void*) recorrer_tp_1er_nivel);

	log_info(logger, "   - TABLA DE PAGINAS ACTUALIZADA");
}

void reemplazar_pagina() {
	// 1. Escribir pagina (en marco) en swap
	if(peticion_swap->entrada_tp_2do_nivel_a_intercambiar->bit_modificado) {
		escribir_pagina_swap(peticion_swap->pid_reemplazo, peticion_swap->entrada_tp_2do_nivel_a_intercambiar->index, peticion_swap->entrada_tp_2do_nivel_a_intercambiar->marco);
		log_info(logger, "    ~ [PF] PAGINA SALIENTE: #%05d (SWAP ACTUALIZADO)", peticion_swap->entrada_tp_2do_nivel_a_intercambiar->index);
	} else {
		log_info(logger, "    ~ [PF] PAGINA SALIENTE: #%05d", peticion_swap->entrada_tp_2do_nivel_a_intercambiar->index);
	}

	// 2, Escribir pagina en memoria
	escribir_pagina_memoria(peticion_swap->pid_reemplazo, peticion_swap->entrada_tp_2do_nivel->index, peticion_swap->entrada_tp_2do_nivel->marco); // ----------------
	log_info(logger, "    ~ [PF] PAGINA ENTRANTE: #%05d", peticion_swap->entrada_tp_2do_nivel->index);
}

void traer_pagina(){
	// 2, Escribir pagina en memoria
	escribir_pagina_memoria(peticion_swap->pid_reemplazo, peticion_swap->numero_pagina, peticion_swap->entrada_tp_2do_nivel->marco); // ----------------
	log_info(logger, "    ~ [PF] PAGINA TRAIDA: #%05d", peticion_swap->numero_pagina);
}

void borrar_archivo_swap(int pid) {
	char* direccion_swap = crear_direccion_swap(pid);

	remove(direccion_swap);

	free(direccion_swap);
}

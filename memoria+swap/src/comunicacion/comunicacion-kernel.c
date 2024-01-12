#include <comunicacion/comunicacion-kernel.h>

void liberar_memoria_proceso(int tabla_paginas) {
	void resetear_tp_2do_nivel(t_tp_2do_nivel* tp_2do_nivel) {
		void resetear_entrada_tp_2do_nivel(t_entrada_tp_2do_nivel* entrada_tp_2do_nivel) {
			// 3. Marcar paginas y marcos como libres
			if(entrada_tp_2do_nivel->bit_presencia) {
				//pthread_mutex_lock(&mx_marcos_libres);
				liberar_marco(l_marcos_libres, entrada_tp_2do_nivel->marco);
				//pthread_mutex_unlock(&mx_marcos_libres);
			}
		}
		// 2. Iterar tabla de paginas de 2do nivel
		list_iterate(tp_2do_nivel->l_entradas_tp_2do_nivel, (void*) resetear_entrada_tp_2do_nivel);
	}

	t_list* l_tp_1er_nivel = list_get(l_tabla_paginas, tabla_paginas);

	// 1. Iterar tabla de paginas de 1er nivel
	list_iterate(l_tp_1er_nivel, (void*) resetear_tp_2do_nivel);
}

void crear_estructuras_algoritmo_reemplazo(int pid) {
	t_algoritmo_reemplazo* e_a_reemplazo = malloc(sizeof(t_algoritmo_reemplazo));

	e_a_reemplazo->puntero = 0;
	e_a_reemplazo->l_marcos_asignados = list_create();

	list_add(l_marcos_por_proceso, e_a_reemplazo);
}

t_entrada_tp_2do_nivel* crear_entrada_tp_2do_nivel(int index) {
	t_entrada_tp_2do_nivel* entrada_tp_2do_nivel = malloc(sizeof(t_entrada_tp_2do_nivel));

	entrada_tp_2do_nivel->index = index;
	entrada_tp_2do_nivel->marco = -1;
	entrada_tp_2do_nivel->bit_presencia = 0;
	entrada_tp_2do_nivel->bit_uso = 0;
	entrada_tp_2do_nivel->bit_modificado = 0;

	return entrada_tp_2do_nivel;
}

t_tp_2do_nivel* crear_tp_2do_nivel(int index) {
	t_tp_2do_nivel* tp_2do_nivel = malloc(sizeof(t_tp_2do_nivel));

	tp_2do_nivel->index = index;
	tp_2do_nivel->l_entradas_tp_2do_nivel = list_create();

	return tp_2do_nivel;
}

t_list* crear_tp_1er_nivel(int tamanio) {
	// 1. Crear tabla de paginas del proceso
	t_list* l_tp_1er_nivel = list_create();

	// 2. Crear tabla de paginas (de 2do nivel) del proceso
	int cantidad_paginas_a_crear = (int) ceil((double) tamanio / config_memoria->tamanio_pagina);
	int cantidad_tp_2do_nivel = (int) ceil((double) cantidad_paginas_a_crear / config_memoria->cantidad_entradas_tabla_paginas);

	for(int i = 0; i < cantidad_tp_2do_nivel; i++) {
		t_tp_2do_nivel* tp_2do_nivel = crear_tp_2do_nivel(i);

		// 3. Crear entradas tabla de paginas (de 2do nivel) del proceso
		int e = 0;
		while(e < config_memoria->cantidad_entradas_tabla_paginas && cantidad_paginas_a_crear) {
			int numero_pagina = i * config_memoria->cantidad_entradas_tabla_paginas + e;

			t_entrada_tp_2do_nivel* entrada_tp_2do_nivel = crear_entrada_tp_2do_nivel(numero_pagina);
			list_add(tp_2do_nivel->l_entradas_tp_2do_nivel, entrada_tp_2do_nivel);

			e++, cantidad_paginas_a_crear--;
		}

		list_add(l_tp_1er_nivel, tp_2do_nivel);
	}

	return l_tp_1er_nivel;
}

void inicializacion_proceso(t_pcb_reducido* pcb_reducido, int socket_servidor) {
	log_info(logger, "[============== INICIALIZACION [PID #%03d] ==============]", pcb_reducido->pid);

	// 1. Crear tabla de paginas del proceso
	t_list* l_tp_1er_nivel = crear_tp_1er_nivel(pcb_reducido->tamanio);

	// 1. b. Agregar tabla de paginas del proceso a la lista general
	list_add(l_tabla_paginas, l_tp_1er_nivel);
	log_info(logger, "   - TABLA DE PAGINAS CREADA");

	// 2. Crear archivo .swap del proceso
	realizar_peticion_swap_kernel(pcb_reducido);
	log_info(logger, "   - ARCHIVO CREADO");

	// 3. Crear estructura para algoritmo
	crear_estructuras_algoritmo_reemplazo(pcb_reducido->pid);

	// 4. Enviar numero de tabla de paginas al Kernel
	uint32_t numero_tabla_paginas = list_size(l_tabla_paginas) - 1;
	enviar_mensaje(numero_tabla_paginas, sizeof(uint32_t), socket_servidor);
	free(pcb_reducido);
	log_info(logger, "   - INDICE TABLA DE PAGINAS ENVIADO");
}

void suspension_proceso(t_pcb_reducido* pcb_reducido, int socket_servidor) {
	log_info(logger, "[================ SUSPENSION [PID #%03d] ================]", pcb_reducido->pid);

	// 1. Liberar espacio de memoria
	liberar_memoria_proceso(pcb_reducido->tabla_paginas);
	log_info(logger, "   - MEMORIA LIBERADA");

	// 2. Escribir archivo swap
	realizar_peticion_swap_kernel(pcb_reducido);

	// 3. Limpiar lista de marcos por proceso
	t_algoritmo_reemplazo* e_a_reemplazo = list_get(l_marcos_por_proceso, pcb_reducido->pid);
	list_clean(e_a_reemplazo->l_marcos_asignados);

	// 4. Enviar a Kernel confirmacion de suspension
	enviar_mensaje(pcb_reducido->pid, sizeof(uint32_t), socket_servidor);

	free(pcb_reducido);
}

void finalizacion_proceso(t_pcb_reducido* pcb_reducido, int socket_servidor) {
	log_info(logger, "[=============== FINALIZACION [PID #%03d] ===============]", pcb_reducido->pid);

	// 1. Liberar espacio de memoria
	liberar_memoria_proceso(pcb_reducido->tabla_paginas);
	log_info(logger, "   - MEMORIA LIBERADA");

	// 2. Eliminar archivo swap
	log_info(logger, "   - ARCHIVO BORRADO (´%d.swap´)", pcb_reducido->pid);

	realizar_peticion_swap_kernel(pcb_reducido);

	// 3. Enviar notificacion al Kernel
	enviar_mensaje((uint32_t) pcb_reducido->pid, sizeof(uint32_t), socket_servidor);

	free(pcb_reducido);
}

void comunicacion_kernel(int socket_servidor) {
	// Recibir noticacion para conectarse
	sem_wait(&sem_conexiones);
	sem_destroy(&sem_conexiones);

	int socket_kernel = aceptar_cliente(socket_servidor);
	log_info(logger, "[=================== HILO #3: KERNEL ===================]\n");

	int codigo_operacion;
	do {
		codigo_operacion = recibir_operacion(socket_kernel);

		retardar_memoria();

		t_pcb_reducido* pcb_reducido = recibir_paquete_pcb_reducido(socket_kernel);


		switch(pcb_reducido->estado) {
			case NEW:
				inicializacion_proceso(pcb_reducido, socket_kernel);
				break;
			case SUSPENDED_BLOCKED:
			case SUSPENDED_READY:
				suspension_proceso(pcb_reducido, socket_kernel);
				break;
			case EXIT_P:
				finalizacion_proceso(pcb_reducido, socket_kernel);
				break;
			/*case FIN:
				log_error(logger, "==== DESCONEXIÓN: CLIENTE DESCONECTADO =====");
				break;*/
			default:
				log_warning(logger, "[=============== OPERACIÓN DESCONOCIDA ==============]\n");
				break;
		}

		log_info(logger, "[=======================================================]\n");

		//free(pcb_reducido);
	} while(codigo_operacion != FIN);
}

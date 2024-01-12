#include <comunicacion/comunicacion-cpu.h>

uint32_t enviar_indice_marco(int socket_servidor) { //todo: retardo memoria
	log_info(logger, "[============ [HILO #2] ENVIAR INDICE MARCO ============]");

	// A1. Recibir informacion de peticion de CPU: indice tabla de 2do nivel
	t_peticion_memoria_indice* peticion_memoria_indice = recibir_peticion_memoria_indice(socket_servidor);
	log_info(logger, "   - PETICION RECIBIDA");

	// A2. Obtener tabla de 1er nivel
	t_list* l_tp_1er_nivel = list_get(l_tabla_paginas, peticion_memoria_indice->tabla_1er_nivel);

	// A3. Obtener tabla de 2do nivel
	t_tp_2do_nivel* tp_2do_nivel = list_get(l_tp_1er_nivel, peticion_memoria_indice->entrada_tabla_1er_nivel);

	// A4. Enviar indice tabla de 2do nivel
	enviar_mensaje(tp_2do_nivel->index, sizeof(uint32_t), socket_servidor);
	log_info(logger, "   - ENVIAR INDICE (T. 2DO NIVEL)");

	// B1. Recibir informacion de peticion de CPU: indice marco //TODO: EXPLOTA ACA!!!
	uint32_t entrada_tabla_2do_nivel = (uint32_t) recibir_mensaje(socket_servidor);

	// B1. b. Retardo por segunda peticion
	retardar_memoria();

	// B2. Obtener entrada de tabla de 2do nivel
	t_entrada_tp_2do_nivel* entrada_tp_2do_nivel = list_get(tp_2do_nivel->l_entradas_tp_2do_nivel, entrada_tabla_2do_nivel);

	// B3. Obtener indice marco (Algoritmos)
	uint32_t indice_marco = obtener_indice_marco(entrada_tp_2do_nivel, l_tp_1er_nivel, peticion_memoria_indice);

	// B4. Enviar indice marco
	enviar_mensaje(indice_marco, sizeof(uint32_t), socket_servidor);
	log_info(logger, "   - ENVIAR INDICE (MARCO)");

	log_info(logger, "[=======================================================]\n");

	int pid = peticion_memoria_indice->pid;
	free(peticion_memoria_indice);

	return pid;
}

void utilizar_pagina(int operacion, int pid, int pagina) {
	t_list* l_tp_1er_nivel = list_get(l_tabla_paginas, pid);

	t_tp_2do_nivel* tp_2do_nivel = list_get(l_tp_1er_nivel, (int) floor((double) pagina / config_memoria->cantidad_entradas_tabla_paginas));

	t_entrada_tp_2do_nivel* t_entrada_tp_2do_nivel = list_get(tp_2do_nivel->l_entradas_tp_2do_nivel, (int) floor((double) (pagina % config_memoria->cantidad_entradas_tabla_paginas)));

	if(operacion == WRITE)
		t_entrada_tp_2do_nivel->bit_modificado = 1;

	t_entrada_tp_2do_nivel->bit_uso = 1;
}

void obtener_valor_marco(int socket_cliente, int pid) {
	uint32_t valor_marco;
	log_info(logger, "[============== [HILO #2] LEER VALOR MARCO =============]");

	// 1. Recibir indice de marco y desplazamiento
	t_peticion_memoria_io* peticion_memoria_io = recibir_peticion_memoria_io(socket_cliente);
	log_info(logger, "   - PETICION RECIBIDA");

	// 2. Obtener valor leido
	valor_marco = leer_memoria(peticion_memoria_io->marco, peticion_memoria_io->desplazamiento);
	log_info(logger, "   - MARCO LEIDO (VALOR: %05d)", valor_marco);

	// 3. Enviar valor leido
	enviar_mensaje(valor_marco, sizeof(uint32_t), socket_cliente);
	log_info(logger, "   - ENVIAR VALOR");

	// 4. Actualizar (bits de) pagina
	utilizar_pagina(READ, pid, peticion_memoria_io->pagina);
	log_info(logger, "   - CAMBIAR BIT U.");

	// 5. Enviar resultado operacion
	enviar_mensaje(1, sizeof(uint32_t), socket_cliente);
	log_info(logger, "   - ENVIAR RESULTADO OPERACION");

	free(peticion_memoria_io);

	log_info(logger, "[=======================================================]\n");
}

void modificar_valor_marco(int socket_cliente, int pid) {
	uint32_t resultado_operacion;
	log_info(logger, "[============ [HILO #2] ESCRIBIR VALOR MARCO ===========]");

	// 1. Recibir indice de marco y desplazamiento
	t_peticion_memoria_io* peticion_memoria_io = recibir_peticion_memoria_io(socket_cliente);
	log_info(logger, "   - PETICION RECIBIDA");

	// 2. Escribir memoria
	resultado_operacion = escribir_memoria(peticion_memoria_io->marco, peticion_memoria_io->desplazamiento, peticion_memoria_io->a_escribir);
	log_info(logger, "   - MARCO ESCRITO");

	// 3. Actualizar (bits de) pagina
	utilizar_pagina(WRITE, pid, peticion_memoria_io->pagina);
	log_info(logger, "   - CAMBIAR BIT U. Y BIT M.");

	// 4. Enviar resultado operacion
	enviar_mensaje(resultado_operacion, sizeof(uint32_t), socket_cliente);
	log_info(logger, "   - ENVIAR RESULTADO OPERACION");

	free(peticion_memoria_io);
	log_info(logger, "[=======================================================]\n");
}

void comunicacion_cpu(int socket_servidor) {
	int socket_cpu = aceptar_cliente(socket_servidor);
	log_info(logger, "[===================== HILO #2: CPU ====================]\n");

	// 1. Notificar "conexion_kernel()" para que pueda conectarse
	sem_post(&sem_conexiones);

	int codigo_operacion;
	int pid;
	do {
		// 2. Recibir informacion
		codigo_operacion = recibir_operacion(socket_cpu);

		retardar_memoria();

		switch (codigo_operacion) {
			case INDICE_MARCO:
				pid = enviar_indice_marco(socket_cpu);
				break;
			case READ:
				obtener_valor_marco(socket_cpu, pid);
				break;
			case WRITE:
				modificar_valor_marco(socket_cpu, pid);
				break;
			/*case FIN:
				log_error(logger, "==== DESCONEXIÓN: CLIENTE DESCONECTADO =====");
				break;*/
			default:
				log_warning(logger, "[=============== OPERACIÓN DESCONOCIDA ==============]\n");
				break;
		}

	} while(codigo_operacion != FIN);
}


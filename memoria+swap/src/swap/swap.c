#include <swap/swap.h>

void retardar_swap() {
	int retardo_swap_microsegundos = config_memoria->retardo_swap * 1000;
	usleep(retardo_swap_microsegundos);
}

char* crear_direccion_swap(int pid) {
	char* direccion_swap = string_new();

	string_append_with_format(&direccion_swap, "%s/%s%s", config_memoria->path_swap, string_itoa(pid), ".swap"); //TODO CAMBIAR A .SWAP

	return direccion_swap;
}

void escribir_pagina_swap(int pid, int pagina, int marco) {
	// 1. Crear descriptor de archivo
	char* direccion_swap = crear_direccion_swap(pid);
	FILE* a_swap = fopen(direccion_swap, "r+");

	// 2. Leer marco
	int tamanio_marco = config_memoria->tamanio_pagina;
	void* valor_marco = malloc(tamanio_marco);

	int desplazamiento_marco = marco * tamanio_marco;
	//pthread_mutex_lock(&mx_memoria);
	memcpy(valor_marco, memoria + desplazamiento_marco, tamanio_marco);
	//pthread_mutex_unlock(&mx_memoria);

	// 3. Escribir archivo
	// 3. a. Ubicar puntero
	int desplazamiento_pagina = pagina * config_memoria->tamanio_pagina;
	fseek(a_swap, desplazamiento_pagina, SEEK_SET);

	// 3. b. Escribir
	fwrite(valor_marco, 1, tamanio_marco, a_swap);

	// 4. Cerrar descriptor de archivo y liberar elementos
	fclose(a_swap);
	free(valor_marco);
	free(direccion_swap);
}

void escribir_pagina_memoria(int pid, int pagina, int marco) {
	// 1. Crear descriptor de archivo
	char* direccion_swap = crear_direccion_swap(pid);
	FILE* a_swap = fopen(direccion_swap, "r+");

	// 2. Leer pagina
	// 2. a. Ubicar puntero
	int desplazamiento_pagina = pagina * config_memoria->tamanio_pagina;
	fseek(a_swap, desplazamiento_pagina, SEEK_SET);

	// 2. b. Leer pagina
	void* valor_pagina = malloc(config_memoria->tamanio_pagina);

	for(int d = 0; d < config_memoria->tamanio_pagina; d += sizeof(uint32_t)) {
		// Leer numero
		uint32_t n = getw(a_swap);

		// Escribir numero
		memcpy(valor_pagina + d, &n, sizeof(uint32_t));
	}

	// 3. Escribir memoria
	int tamanio_marco = config_memoria->tamanio_pagina;
	uint32_t desplazamiento_marco = marco * tamanio_marco;

	//pthread_mutex_lock(&mx_memoria);
	memcpy(memoria + desplazamiento_marco, valor_pagina, tamanio_marco);
	//pthread_mutex_unlock(&mx_memoria);

	// 4. Cerrar descriptor de archivo y liberar elementos
	fclose(a_swap);
	free(valor_pagina);
	free(direccion_swap);
}

void conexion_swap() {
	log_info(logger, "[==================== HILO #1: SWAP ====================]\n");

	int codigo_operacion;
	do {
		// 1. Esperar una peticion
	    sem_wait(&sem_realizar_peticion);

	    // 2. Simular retardo
		retardar_swap();

		// 3. Realizar peticion
		codigo_operacion = peticion_swap->codigo_peticion;
		switch(codigo_operacion) {
			case NEW:
				crear_archivo_swap(peticion_swap->pcb_reducido);
 				break;
			case I_O:
			case SUSPENDED_BLOCKED:
			case SUSPENDED_READY:
				suspender_proceso(peticion_swap->pcb_reducido);
				break;
			case REEMPLAZAR_PAGINA:
				reemplazar_pagina();
				break;
			case TRAER_PAGINA:
				traer_pagina();
				break;
			case EXIT_P:
				borrar_archivo_swap(peticion_swap->pcb_reducido->pid);
				break;
			/*case FIN:
				log_error(logger, "==== DESCONEXIÓN: CLIENTE DESCONECTADO =====");
				break;*/
			default:
				log_warning(logger, "=========== OPERACIÓN DESCONOCIDA ==========");
				break;
		}

		//4. Notificar a siguiente peticion
		sem_post(&sem_esperar_swap);
		sem_post(&sem_sincronizar_peticiones);
	} while(codigo_operacion != FIN);

}

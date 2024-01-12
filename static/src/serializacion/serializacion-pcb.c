#include <serializacion/serializacion-pcb.h>

void agregar_pcb_a_paquete(t_paquete* paquete, t_pcb* pcb) {
	// Reservar memoria: componentes de PCB
	uint32_t tamanio_pcb_sin_instrucciones = 2 * sizeof(uint8_t) + 1 * sizeof(uint16_t) + 3 * sizeof(uint32_t) + 4 * sizeof(int64_t);
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->tamanio + tamanio_pcb_sin_instrucciones);

	uint32_t desplazamiento = paquete->buffer->tamanio;

	/* ** Planificacion General ** */
	// Agregar: identificador del proceso (PID)
	memcpy(paquete->buffer->stream + desplazamiento, &(pcb->pid), sizeof(uint16_t));
	desplazamiento += sizeof(uint16_t);

	// Agregar: tamaño del proceso
	memcpy(paquete->buffer->stream + desplazamiento, &(pcb->tamanio), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	// Agregar: program counter
	memcpy(paquete->buffer->stream + desplazamiento, &(pcb->program_counter), sizeof(uint8_t));
	desplazamiento += sizeof(uint8_t);

	// Agregar: tabla de paginas del proceso
	memcpy(paquete->buffer->stream + desplazamiento, &(pcb->tabla_paginas), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	// Agregar: tiempo de bloqueo (solo IO)
	memcpy(paquete->buffer->stream + desplazamiento, &(pcb->tiempo_bloqueo), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	// Agregar: estado del proceso
	memcpy(paquete->buffer->stream + desplazamiento, &(pcb->estado), sizeof(uint8_t));
	desplazamiento += sizeof(uint8_t);

	/* **** Planificacion SRT **** */
	// Agregar: estimacion de rafaga del proceso
	memcpy(paquete->buffer->stream + desplazamiento, &(pcb->estimacion_rafaga), sizeof(int64_t));
	desplazamiento += sizeof(int64_t);

	// Agregar: estimacion de rafaga anterior del proceso
	memcpy(paquete->buffer->stream + desplazamiento, &(pcb->estimacion_rafaga_anterior), sizeof(int64_t));
	desplazamiento += sizeof(int64_t);

	// Agregar: tiempo que entro a ejecutar
	memcpy(paquete->buffer->stream + desplazamiento, &(pcb->tiempo_entro_a_ejecutar), sizeof(int64_t));
	desplazamiento += sizeof(int64_t);

	// Agregar: tiempo que salio de ejecutar
	memcpy(paquete->buffer->stream + desplazamiento, &(pcb->tiempo_salio_de_ejecutar), sizeof(int64_t));
	desplazamiento += sizeof(int64_t);

	/* ****** Instrucciones ****** */
	// Agregar: instrucciones del proceso
	int tamanio_l_instrucciones = 0;
	for(int i = 0; i < list_size(pcb->l_instrucciones); i++) {
		t_instruccion* instruccion = list_get(pcb->l_instrucciones, i);

		// Agregar: identificador de la instruccion
		tamanio_l_instrucciones += sizeof(uint8_t);
		paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->tamanio + tamanio_pcb_sin_instrucciones + tamanio_l_instrucciones);

		memcpy(paquete->buffer->stream + desplazamiento, &(instruccion->identificador), sizeof(uint8_t));
		desplazamiento += sizeof(uint8_t);

		// Agregar: primer parametro de la instruccion (si corresponde)
		if(instruccion->identificador == I_O || instruccion->identificador == READ || instruccion->identificador == WRITE || instruccion->identificador == COPY) {
			tamanio_l_instrucciones += sizeof(uint32_t);
			paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->tamanio + tamanio_pcb_sin_instrucciones + tamanio_l_instrucciones);

			memcpy(paquete->buffer->stream + desplazamiento, &(instruccion->parametros[0]), sizeof(uint32_t));
			desplazamiento += sizeof(uint32_t);
		}

		// Agregar: segundo parametro de la instruccion (si corresponde)
		if(instruccion->identificador == WRITE || instruccion->identificador == COPY) {
			tamanio_l_instrucciones += sizeof(uint32_t);
			paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->tamanio + tamanio_pcb_sin_instrucciones + tamanio_l_instrucciones);

			memcpy(paquete->buffer->stream + desplazamiento, &(instruccion->parametros[1]), sizeof(uint32_t));
			desplazamiento += sizeof(uint32_t);
		}
	}

	// Agrandar memoria: componentes de PCB
	paquete->buffer->tamanio += tamanio_pcb_sin_instrucciones + tamanio_l_instrucciones;
}

void agregar_pcb_reducido_a_paquete(t_paquete* paquete, t_pcb_reducido* pcb_reducido) {
	// Reservar memoria: componentes de PCB reducido
	uint32_t tamanio_pcb = 1 * sizeof(uint8_t) + 1 * sizeof(uint16_t) + 2 * sizeof(uint32_t);
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->tamanio + tamanio_pcb);

	uint32_t desplazamiento = paquete->buffer->tamanio;

	/* ** Planificacion General ** */
	// Agregar: identificador del proceso (PID)
	memcpy(paquete->buffer->stream + desplazamiento, &(pcb_reducido->pid), sizeof(uint16_t));
	desplazamiento += sizeof(uint16_t);

	// Agregar: tamaño del proceso
	memcpy(paquete->buffer->stream + desplazamiento, &(pcb_reducido->tamanio), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	// Agregar: tabla de paginas del proceso
	memcpy(paquete->buffer->stream + desplazamiento, &(pcb_reducido->tabla_paginas), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	// Agregar: estado del proceso
	memcpy(paquete->buffer->stream + desplazamiento, &(pcb_reducido->estado), sizeof(uint8_t));
	desplazamiento += sizeof(uint8_t);

	// Agrandar memoria: componentes de PCB
	paquete->buffer->tamanio += tamanio_pcb;
}

void enviar_pcb_reducido(t_pcb* pcb, int socket_cliente) {
	t_paquete* paquete = crear_paquete(PCB_REDUCIDO);

	t_pcb_reducido* pcb_reducido = malloc(sizeof(t_pcb_reducido));
	pcb_reducido->pid = pcb->pid;
	pcb_reducido->tamanio = pcb->tamanio;
	pcb_reducido->tabla_paginas = pcb->tabla_paginas;
	pcb_reducido->estado = pcb->estado;

	agregar_pcb_reducido_a_paquete(paquete, pcb_reducido);

	enviar_paquete(paquete, socket_cliente);
}

t_pcb* recibir_paquete_pcb(int socket_cliente) {
	t_pcb* pcb = malloc(sizeof(t_pcb));

	uint8_t identificador_instruccion;
	uint32_t tamanio_buffer, parametro0, parametro1;
	uint32_t desplazamiento = 0;

	void* buffer = recibir_buffer(&tamanio_buffer, socket_cliente);

	/* ** Planificacion General ** */
	// De-serializar: identificador del proceso
	memcpy(&(pcb->pid), buffer + desplazamiento, sizeof(uint16_t));
	desplazamiento += sizeof(uint16_t);

	// De-serializar: tamaño del proceso
	memcpy(&(pcb->tamanio), buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	// De-serializar: program counter
	memcpy(&(pcb->program_counter), buffer + desplazamiento, sizeof(uint8_t));
	desplazamiento += sizeof(uint8_t);

	// De-serializar: tabla de paginas del proceso
	memcpy(&(pcb->tabla_paginas), buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	// De-serializar: tiempo de bloqueo (solo IO)
	memcpy(&(pcb->tiempo_bloqueo), buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	// De-serializar: estado del proceso
	memcpy(&(pcb->estado), buffer + desplazamiento, sizeof(uint8_t));
	desplazamiento += sizeof(uint8_t);

	/* **** Planificacion SRT **** */
	// De-serializar: estimacion de rafaga del proceso
	memcpy(&(pcb->estimacion_rafaga), buffer + desplazamiento, sizeof(int64_t));
	desplazamiento += sizeof(int64_t);

	// De-serializar: estimacion de rafaga anterior del proceso
	memcpy(&(pcb->estimacion_rafaga_anterior), buffer + desplazamiento, sizeof(int64_t));
	desplazamiento += sizeof(int64_t);

	// De-serializar: tiempo que entro a ejecutar
	memcpy(&(pcb->tiempo_entro_a_ejecutar), buffer + desplazamiento, sizeof(int64_t));
	desplazamiento += sizeof(int64_t);

	// De-serializar: tiempo que salio de ejecutar
	memcpy(&(pcb->tiempo_salio_de_ejecutar), buffer + desplazamiento, sizeof(int64_t));
	desplazamiento += sizeof(int64_t);

	/* ****** Instrucciones ****** */
	// De-serializar: instrucciones del proceso
	t_list* l_instrucciones = list_create();

	while(desplazamiento < tamanio_buffer) {
		t_instruccion* instruccion = malloc(sizeof(t_instruccion));

		// De-serializar: identificador de la instruccion
		memcpy(&identificador_instruccion, buffer + desplazamiento, sizeof(uint8_t));
		desplazamiento += sizeof(uint8_t);
		instruccion->identificador = identificador_instruccion;

		// De-serializar: primer parametro de la instruccion (si corresponde)
		if(identificador_instruccion == I_O || identificador_instruccion == READ || identificador_instruccion == WRITE || identificador_instruccion == COPY) {
			memcpy(&parametro0, buffer + desplazamiento, sizeof(uint32_t));
			desplazamiento += sizeof(uint32_t);
			instruccion->parametros[0] = parametro0;
		}

		// De-serializar: segundo parametro de la instruccion (si corresponde)
		if(identificador_instruccion == WRITE || identificador_instruccion == COPY) {
			memcpy(&parametro1, buffer + desplazamiento, sizeof(uint32_t));
			desplazamiento += sizeof(uint32_t);
			instruccion->parametros[1] = parametro1;
		}

		// Agregar instruccion a lista
		list_add(l_instrucciones, instruccion);
	}

	pcb->l_instrucciones = l_instrucciones;

	free(buffer);

	return pcb;
}

t_pcb_reducido* recibir_paquete_pcb_reducido(int socket_cliente) {
	t_pcb_reducido* pcb_reducido = malloc(sizeof(t_pcb_reducido));

	uint32_t tamanio_buffer, desplazamiento = 0;

	void* buffer = recibir_buffer(&tamanio_buffer, socket_cliente);

	/* ** Planificacion General ** */
	// De-serializar: identificador del proceso
	memcpy(&(pcb_reducido->pid), buffer + desplazamiento, sizeof(uint16_t));
	desplazamiento += sizeof(uint16_t);

	// De-serializar: tamaño del proceso
	memcpy(&(pcb_reducido->tamanio), buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	// De-serializar: tabla de paginas del proceso
	memcpy(&(pcb_reducido->tabla_paginas), buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	// De-serializar: estadi del proceso
	memcpy(&(pcb_reducido->estado), buffer + desplazamiento, sizeof(uint8_t));
	desplazamiento += sizeof(uint8_t);

	free(buffer);

	return pcb_reducido;
}

char* convertir_a_horario(time_t time) {
	// Obtener horario actual
	struct tm ttm;
	ttm = * localtime(&time);

	// Convertir horario a cadena
	char* horario = string_new();
	string_append_with_format(&horario, "%02d:%02d:%02d", ttm.tm_hour, ttm.tm_min, ttm.tm_sec);

	return horario;
}

void imprimir_pcb_completo(t_log* logger, t_pcb* pcb) {
	imprimir_pcb_incompleto(logger, pcb);
	imprimir_instrucciones(logger, (void*) pcb->l_instrucciones);
}

void imprimir_pcb_incompleto(t_log* logger, t_pcb* pcb) {
	log_info(logger, "[**************** INFO. PCB [PID #%05d] ****************]", pcb->pid);
	log_info(logger, "   * TAMAÑO ....................... %05d", pcb->tamanio);
	log_info(logger, "   * PROGRAM COUNTER .............. %05d", pcb->program_counter);
	log_info(logger, "   * TABLA DE PAGINAS ............. %05d", pcb->tabla_paginas);

	if(pcb->tiempo_bloqueo > 0)
		log_info(logger, "   * TIEMPO BLOQUEO (I/O) ......... %05d", pcb->tiempo_bloqueo);

	log_info(logger, "   * ESTADO PROCESO ............... %05d", pcb->estado);

	log_info(logger, "   * EST. RAFAGA .................. %05lld", pcb->estimacion_rafaga);
	log_info(logger, "   * EST. RAFAGA ANTERIOR ......... %05lld", pcb->estimacion_rafaga_anterior);
	log_info(logger, "   * TIE. ENT. EJEC. .............. %s", convertir_a_horario(pcb->tiempo_entro_a_ejecutar));
	log_info(logger, "   * TIE. SAL. EJEC. .............. %s", convertir_a_horario(pcb->tiempo_salio_de_ejecutar));
	log_info(logger, "[*******************************************************]\n");
}

void imprimir_pcb_mini(t_log* logger, t_pcb* pcb) {
	log_info(logger, "[**************** INFO. PCB [PID #%05d] ****************]", pcb->pid);
	log_info(logger, "   * PROGRAM COUNTER .............. %05d", pcb->program_counter);

	if(pcb->tiempo_bloqueo > 0)
		log_info(logger, "   * TIEMPO BLOQUEO (I/O) ......... %05d", pcb->tiempo_bloqueo);

	log_info(logger, "   * ESTADO PROCESO ............... %05d", pcb->estado);

	log_info(logger, "   * CANTIDAD INSTRUCCIONES: %05d", list_size(pcb->l_instrucciones));

	log_info(logger, "[*******************************************************]\n");
}

void liberar_pcb(t_pcb* pcb) {
	list_destroy_and_destroy_elements(pcb->l_instrucciones, free);
	free(pcb);
}

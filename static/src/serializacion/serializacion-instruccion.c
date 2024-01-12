#include <serializacion/serializacion-instruccion.h>

void agregar_tamanio_proceso_a_paquete(t_paquete* paquete, uint32_t tamanio_proceso) {
	// Reservar memoria: tamaño del proceso
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->tamanio + sizeof(uint32_t));

	uint32_t desplazamiento = paquete->buffer->tamanio;

	// Agregar: tamaño de proceso
	memcpy(paquete->buffer->stream + desplazamiento, &tamanio_proceso, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	// Agrandar tamaño: tamaño del proceso
	paquete->buffer->tamanio += sizeof(uint32_t);
}

void agregar_instruccion_a_paquete(t_paquete* paquete, t_instruccion* instruccion) {
	int cantidad_parametros = cantidad_parametros_instruccion(instruccion->identificador);

	// Reservar memoria: componentes de una instruccion (codigo de operacion y parametro/s)
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->tamanio + sizeof(uint8_t) + cantidad_parametros * sizeof(uint32_t));

	uint32_t desplazamiento = paquete->buffer->tamanio;

	// Agregar: identificador de la instruccion
	memcpy(paquete->buffer->stream + desplazamiento, &(instruccion->identificador), sizeof(uint8_t));
	desplazamiento += sizeof(uint8_t);

	// Agregar: primer parametro de la instruccion (si corresponde)
	if(cantidad_parametros >= 1) {
		memcpy(paquete->buffer->stream + desplazamiento, &(instruccion->parametros[0]), sizeof(uint32_t));
		desplazamiento += sizeof(uint32_t);
	}

	// Agregar: segundo parametro de la instruccion (si corresponde)
	if(cantidad_parametros == 2) {
		memcpy(paquete->buffer->stream + desplazamiento, &(instruccion->parametros[1]), sizeof(uint32_t));
		desplazamiento += sizeof(uint32_t);
	}

	// Agrandar tamaño: tamaño del stream + tamaño del int que contiene el tamaño del stream
	paquete->buffer->tamanio += sizeof(uint8_t) + cantidad_parametros * sizeof(uint32_t);
}

uint32_t recibir_tamanio_proceso(int socket_cliente) {
	uint32_t tamanio_proceso;

	recv(socket_cliente, &tamanio_proceso, sizeof(uint32_t), MSG_WAITALL);

	return tamanio_proceso;
}

t_list* recibir_paquete_instrucciones(int socket_cliente) {
	t_list* l_instrucciones = list_create();

	uint8_t identificador_instruccion;
	uint32_t tamanio_buffer, parametro0, parametro1;
	uint32_t tamanio_proceso;
	uint32_t desplazamiento = 0;

	// Obtener buffer
	void* buffer = recibir_buffer(&tamanio_buffer, socket_cliente);

	// Obtener tamaño del proceso
	memcpy(&tamanio_proceso, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	list_add(l_instrucciones, (void*) tamanio_proceso);

	// De-serializar: buffer en elementos
	while(desplazamiento < tamanio_buffer) {
		t_instruccion* instruccion = malloc(sizeof(t_instruccion));

		// De-serializar: identificador de la instruccion
		memcpy(&identificador_instruccion, buffer + desplazamiento, sizeof(uint8_t));
		desplazamiento += sizeof(uint8_t);
		instruccion->identificador = identificador_instruccion;

		int cantidad_parametros = cantidad_parametros_instruccion(instruccion->identificador);

		// De-serializar: primer parametro de la instruccion (si corresponde)
		if(cantidad_parametros >= 1) {
			memcpy(&parametro0, buffer + desplazamiento, sizeof(uint32_t));
			desplazamiento += sizeof(uint32_t);
			instruccion->parametros[0] = parametro0;
		}

		// De-serializar: segundo parametro de la instruccion (si corresponde)
		if(cantidad_parametros == 2) {
			memcpy(&parametro1, buffer + desplazamiento, sizeof(uint32_t));
			desplazamiento += sizeof(uint32_t);
			instruccion->parametros[1] = parametro1;
		}

		// Agregar instruccion a lista
		list_add(l_instrucciones, instruccion);
	}

	free(buffer);

	return l_instrucciones;
}

int cantidad_parametros_instruccion(e_id_operacion identificador_operacion) {
	int cantidad_parametros;

	switch(identificador_operacion) {
		case NO_OP:
		case EXIT_I:
			cantidad_parametros = 0;
			break;
		case I_O:
		case READ:
			cantidad_parametros = 1;
			break;
		case WRITE:
		case COPY:
			cantidad_parametros = 2;
			break;
	}

	return cantidad_parametros;
}

void imprimir_instrucciones(t_log* logger, t_list* l_instrucciones) {
	void imprimir_instruccion(t_instruccion* instruccion) {
		if(cantidad_parametros_instruccion(instruccion->identificador) == 0) {
			log_info(logger, "   * ID #%d", instruccion->identificador);

		} else if(cantidad_parametros_instruccion(instruccion->identificador) == 1) {
			log_info(logger, "   * ID #%d - P1:%05d", instruccion->identificador, instruccion->parametros[0]);

		} else if(cantidad_parametros_instruccion(instruccion->identificador) == 2) {
			log_info(logger, "   * ID #%d - P1:%05d - P2:%05d", instruccion->identificador, instruccion->parametros[0], instruccion->parametros[1]);
		}
	}

	log_info(logger, "[******************** INSTRUCCIONES ********************]");

	list_iterate(l_instrucciones, (void*) imprimir_instruccion);
	log_info(logger, "   * CANTIDAD INSTRUCCIONES: %05d", list_size(l_instrucciones));

	log_info(logger, "[*******************************************************]\n");
}

void enviar_paquete_instrucciones(int conexion, t_list* l_instrucciones, uint32_t tamanio_proceso) {
	t_paquete* paquete = crear_paquete(INSTRUCCIONES);

	agregar_tamanio_proceso_a_paquete(paquete, tamanio_proceso);

	for(int i = 0; i < list_size(l_instrucciones); i++) {
		t_instruccion* instruccion = list_get(l_instrucciones, i);

		agregar_instruccion_a_paquete(paquete, instruccion);
	}

	enviar_paquete(paquete, conexion);
	eliminar_paquete(paquete);
}

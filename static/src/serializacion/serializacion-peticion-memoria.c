#include <serializacion/serializacion-peticion-memoria.h>

void agregar_peticion_memoria_indice_a_paquete(t_paquete* paquete, t_peticion_memoria_indice* peticion_memoria_indice) {
	// Reservar memoria: componentes de peticion
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->tamanio + 5 * sizeof(uint32_t));

	uint32_t desplazamiento = paquete->buffer->tamanio;

	// Agregar: Entrada de tabla de 1er nivel
	memcpy(paquete->buffer->stream + desplazamiento, &(peticion_memoria_indice->entrada_tabla_1er_nivel), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	// Agregar: Numero pagina
	memcpy(paquete->buffer->stream + desplazamiento, &(peticion_memoria_indice->numero_pagina), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	// Agregar: PID
	memcpy(paquete->buffer->stream + desplazamiento, &(peticion_memoria_indice->pid), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	// Agregar: Indice de tabla de 1er nivel
	memcpy(paquete->buffer->stream + desplazamiento, &(peticion_memoria_indice->tabla_1er_nivel), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	// Agregar: Tamanio proceso
	memcpy(paquete->buffer->stream + desplazamiento, &(peticion_memoria_indice->tamanio_proceso), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	// Agrandar tamaño: tamaño del stream + tamaño del int que contiene el tamaño del stream
	paquete->buffer->tamanio += 5 * sizeof(uint32_t);
}

void agregar_peticion_memoria_io_a_paquete(t_paquete* paquete, t_peticion_memoria_io* peticion_memoria_io) {
	// Reservar memoria: componentes de peticion
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->tamanio + 4 * sizeof(uint32_t));

	uint32_t desplazamiento = paquete->buffer->tamanio;

	// Agregar: Numero de pagina
	memcpy(paquete->buffer->stream + desplazamiento, &(peticion_memoria_io->pagina), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	// Agregar: Indice marco
	memcpy(paquete->buffer->stream + desplazamiento, &(peticion_memoria_io->marco), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	// Agregar: Desplazamiento
	memcpy(paquete->buffer->stream + desplazamiento, &(peticion_memoria_io->desplazamiento), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	// Agregar: Valor a escribir
	memcpy(paquete->buffer->stream + desplazamiento, &(peticion_memoria_io->a_escribir), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	// Agrandar tamaño: tamaño del stream + tamaño del int que contiene el tamaño del stream
	paquete->buffer->tamanio += 4 * sizeof(uint32_t);
}

t_peticion_memoria_indice* recibir_peticion_memoria_indice(int socket_cliente) {
	t_peticion_memoria_indice* peticion_memoria_indice = malloc(sizeof(t_peticion_memoria_indice));

	uint32_t tamanio_buffer, desplazamiento = 0;

	// Obtener: buffer
	void* buffer = recibir_buffer(&tamanio_buffer, socket_cliente);

	// Obtener: Entrada de tabla de 1er nivel
	memcpy(&(peticion_memoria_indice->entrada_tabla_1er_nivel), buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	// Obtener: Numero pagina
	memcpy(&(peticion_memoria_indice->numero_pagina), buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	// Obtener: Entrada de tabla de 1er nivel
	memcpy(&(peticion_memoria_indice->pid), buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	// Obtener: Indice de tabla de 1er nivel
	memcpy(&(peticion_memoria_indice->tabla_1er_nivel), buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	// Obtener: Tamanio proceso
	memcpy(&(peticion_memoria_indice->tamanio_proceso), buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	free(buffer);

	return peticion_memoria_indice;
}

t_peticion_memoria_io* recibir_peticion_memoria_io(int socket_cliente) {
	t_peticion_memoria_io* peticion_memoria_io = malloc(sizeof(t_peticion_memoria_io));

	uint32_t tamanio_buffer, desplazamiento = 0;

	// Obtener: buffer
	void* buffer = recibir_buffer(&tamanio_buffer, socket_cliente);

	// Obtener: Numero de pagina
	memcpy(&(peticion_memoria_io->pagina), buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	// Obtener: Indice marco
	memcpy(&(peticion_memoria_io->marco), buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	// Obtener: Desplazamiento
	memcpy(&(peticion_memoria_io->desplazamiento), buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	// Obtener: Valor a escribir
	memcpy(&(peticion_memoria_io->a_escribir), buffer + desplazamiento, sizeof(uint32_t));

	free(buffer);

	return peticion_memoria_io;
}

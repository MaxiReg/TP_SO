#include <serializacion/serializacion-handshake.h>

void enviar_paquete_handshake(t_config_handshake* config_handshake, int conexion) {
	t_paquete* paquete = crear_paquete(HANDSHAKE);

	// Reservar memoria: componentes de handshake
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->tamanio + 2 * sizeof(uint32_t));

	uint32_t desplazamiento = paquete->buffer->tamanio;

	// Agregar: cantidad de entradas tabla de paginas
	memcpy(paquete->buffer->stream + desplazamiento, &(config_handshake->cantidad_entradas_tabla_paginas), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	// Agregar: tamaño de pagina
	memcpy(paquete->buffer->stream + desplazamiento, &(config_handshake->tamanio_pagina), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	// Agrandar memoria: tamaño del handshake
	paquete->buffer->tamanio += 2 * sizeof(uint32_t);

	enviar_paquete(paquete, conexion);
	eliminar_paquete(paquete);
}

t_config_handshake* recibir_paquete_handshake(int socket_cliente) {
	t_config_handshake* config_handshake = malloc(sizeof(t_config_handshake));

	uint32_t tamanio_buffer, desplazamiento = 0;

	void* buffer = recibir_buffer(&tamanio_buffer, socket_cliente);

	// De-serializar: cantidad de entradas tabla de paginas
	memcpy(&(config_handshake->cantidad_entradas_tabla_paginas), buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	// De-serializar: tamaño de pagina
	memcpy(&(config_handshake->tamanio_pagina) , buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	free(buffer);

	return config_handshake;
}

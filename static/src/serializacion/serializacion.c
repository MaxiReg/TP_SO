#include <serializacion/serializacion.h>

t_paquete* crear_paquete(e_codigo_operacion codigo_operacion) {
	t_paquete* paquete = malloc(sizeof(t_paquete));

	// Inicializar paquete
	paquete->codigo_operacion = codigo_operacion;

	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->tamanio = 0;
	paquete->buffer->stream = NULL;

	return paquete;
}

void agregar_elemento_a_paquete(t_paquete* paquete, void* stream, uint32_t tamanio_stream) {
	// Reservar memoria: tamaño de stream (uint32_t) y stream
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->tamanio + tamanio_stream + sizeof(uint32_t));

	uint32_t desplazamiento = paquete->buffer->tamanio;

	// Agregar: tamanio del stream
	memcpy(paquete->buffer->stream + desplazamiento, &tamanio_stream, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	// Agregar: stream
	memcpy(paquete->buffer->stream + desplazamiento, stream, tamanio_stream);

	// Agrandar tamaño: tamaño de stream (uint32_t) y stream
	paquete->buffer->tamanio += tamanio_stream + sizeof(uint32_t);
}

void* serializar_paquete(t_paquete* paquete, uint32_t tamanio_paquete) {
	// Reservar memoria: tamaño de paquete (codigo de operacion + tamaño de stream + tamaños de elementos + elementos)
	void* stream_serializacion = malloc(tamanio_paquete);

	uint32_t desplazamiento = 0;

	// Serializar: codigo del operacion
	memcpy(stream_serializacion + desplazamiento, &(paquete->codigo_operacion), sizeof(uint8_t));
	desplazamiento += sizeof(uint8_t);

	// Serializar: tamanio del stream
	memcpy(stream_serializacion + desplazamiento, &(paquete->buffer->tamanio), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	// Serializar: stream
	memcpy(stream_serializacion + desplazamiento, paquete->buffer->stream, paquete->buffer->tamanio);
	desplazamiento += paquete->buffer->tamanio; //TODO: ¿Es necesaria esta linea?

	return stream_serializacion;
}

void enviar_paquete(t_paquete* paquete, int socket_cliente) {
	// Reservar memoria: tamaño de paquete (codigo de operacion + tamaño de stream + tamaños de elementos + elementos)
	uint32_t tamanio_paquete = paquete->buffer->tamanio + sizeof(uint8_t) + sizeof(uint32_t);

	void* a_enviar = serializar_paquete(paquete, tamanio_paquete);

	send(socket_cliente, a_enviar, tamanio_paquete, 0);

	free(a_enviar);
}

void enviar_mensaje(uint32_t mensaje, uint32_t tamanio, int socket_cliente) {
	send(socket_cliente, &mensaje, tamanio, 0);

}

void eliminar_paquete(t_paquete* paquete) {
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

void* recibir_buffer(uint32_t* tamanio, int socket_cliente) {
	void* buffer;

	// Esperar: tamaño del buffer
	recv(socket_cliente, tamanio, sizeof(uint32_t), MSG_WAITALL);
	buffer = malloc(*tamanio);

	// Esperar: buffer
	recv(socket_cliente, buffer, *tamanio, MSG_WAITALL);

	return buffer;
}

t_list* recibir_paquete(int socket_cliente) {
	t_list* l_elementos = list_create();

	uint32_t desplazamiento = 0;

	// Obtener buffer
	uint32_t tamanio_buffer;
	void* buffer = recibir_buffer(&tamanio_buffer, socket_cliente);

	// De-serializar: buffer en elementos
	uint32_t tamanio_elemento;
	while(desplazamiento < tamanio_buffer) {
		// De-serializar: tamanio del elemento
		memcpy(&tamanio_elemento, buffer + desplazamiento, sizeof(uint32_t));
		desplazamiento += sizeof(uint32_t);

		// Reservar memoria para el elemento a copiar
		char* valor = malloc(tamanio_elemento);

		// De-serializar: elemento
		memcpy(valor, buffer + desplazamiento, tamanio_elemento);
		desplazamiento += tamanio_elemento;

		// Agregar elemento a lista
		list_add(l_elementos, valor);
	}

	free(buffer);

	return l_elementos;
}

int recibir_operacion(int socket_cliente) {
	uint8_t codigo_operacion;

	if(recv(socket_cliente, &codigo_operacion, sizeof(uint8_t), MSG_WAITALL) > 0) {
		return codigo_operacion;
	} else {
		// TODO: Cerrar conexión ante operación invalida????? RECV DEVUELVE -1 ANTE ERROR
		close(socket_cliente);
		return FIN;
	}
}

void* recibir_mensaje(int socket_cliente) { //TODO: CAMBIAR TIPO DE FUNCIONA A uint32_t??
	uint32_t mensaje;

	recv(socket_cliente, &mensaje, sizeof(uint32_t), MSG_WAITALL);

	return (void*) mensaje;
}

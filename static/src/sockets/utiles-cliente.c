#include <sockets/utiles-cliente.h>

int crear_conexion(char* ip, char* puerto) {
	struct addrinfo protocolo_servidor, * info_servidor;

	// Parsear direcciones
	memset(&protocolo_servidor, 0, sizeof(protocolo_servidor));
	protocolo_servidor.ai_family = AF_UNSPEC;
	protocolo_servidor.ai_socktype = SOCK_STREAM;
	protocolo_servidor.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &protocolo_servidor, &info_servidor);

	// Crear socket
	int socket_cliente = 0;
	socket_cliente = socket(info_servidor->ai_family,
							info_servidor->ai_socktype,
							info_servidor->ai_protocol);

	// Marcar socket como reusable
	const int habilitar = 1;
	setsockopt(socket_cliente, SOL_SOCKET, SO_REUSEADDR, &habilitar, sizeof(int));

	// Marcar puerto como reusable
	setsockopt(socket_cliente, SOL_SOCKET, SO_REUSEPORT, &habilitar, sizeof(int));

	// Conectar socket
	connect(socket_cliente, info_servidor->ai_addr, info_servidor->ai_addrlen);

	// Liberar estructura
	freeaddrinfo(info_servidor);

	return socket_cliente;
}

void terminar_conexion(int conexion) {
	close(conexion);
}

#include <sockets/utiles-servidor.h>

int iniciar_servidor(char* ip, char* puerto) {
	struct addrinfo protocolo_servidor, * info_servidor;

	// Parsear direcciones
	memset(&protocolo_servidor, 0, sizeof(protocolo_servidor));
	protocolo_servidor.ai_family = AF_UNSPEC;
	protocolo_servidor.ai_socktype = SOCK_STREAM;
	protocolo_servidor.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &protocolo_servidor, &info_servidor);

	// Crear socket de escucha del servidor
	int socket_servidor = socket(info_servidor->ai_family,
								 info_servidor->ai_socktype,
								 info_servidor->ai_protocol);

	// Marcar socket como reusable
	const int habilitar = 1;
	setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEADDR, &habilitar, sizeof(int));

	// Marcar puerto como reusable
	setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEPORT, &habilitar, sizeof(int));

	// Asociar socket a un puerto
	bind(socket_servidor, info_servidor->ai_addr, info_servidor->ai_addrlen);

	// Escuchar conexiones entrantes
	listen(socket_servidor, SOMAXCONN);

	// Liberar estructura
	freeaddrinfo(info_servidor);

	return socket_servidor;
}

int aceptar_cliente(int socket_servidor) {
	//struct sockaddr_in dir_cliente;
	//socklen_t tam_direccion = sizeof(struct sockaddr_in);

	int socket_cliente = accept(socket_servidor, NULL, NULL);

	return socket_cliente;
}

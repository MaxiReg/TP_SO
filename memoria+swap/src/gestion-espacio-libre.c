#include <gestion-espacio-libre.h>

void ocupar_marco(t_list* l_marcos_libres, int indice) {
	uint32_t valor_ocupado = 0;
	//pthread_mutex_lock(&mx_marcos_libres);
	list_replace(l_marcos_libres, indice, (void*) valor_ocupado);
	//pthread_mutex_unlock(&mx_marcos_libres);
}

void liberar_marco(t_list* l_marcos_libres, int indice) {
	uint32_t valor_libre = 1;
	//pthread_mutex_lock(&mx_marcos_libres);
	list_replace(l_marcos_libres, indice, (void*) valor_libre);
	//pthread_mutex_unlock(&mx_marcos_libres);
}


t_list* crear_l_marcos_libres(int tamanio_memoria, int tamanio_pagina) {
	uint32_t valor_libre = 1;

	int cantidad_marcos = tamanio_memoria / tamanio_pagina;

	t_list* lista = list_create();

	for(int b = 0; b < cantidad_marcos; b++) {
		list_add(lista, (void*) valor_libre);
	}

	return lista;
}

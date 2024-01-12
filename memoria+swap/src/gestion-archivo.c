#include <gestion-archivo.h>

//void crear_archivo(char* direccion_archivo, int tamanio_archivo) {
//	// 1. Crear descriptor de archivo
//	FILE* a_swap = fopen(direccion_archivo, "w+");
//
//	// 2. Inicializar archivo
//	uint32_t a_escribir = 2000;
//	for(int n = 0; n < tamanio_archivo; n += sizeof(uint32_t)) {
//		putw(a_escribir, a_swap);
//		a_escribir++;
//	}
//
//	// 3. Cerrar descriptor de archivo
//	fclose(a_swap);
//}
//
//void mostrar_archivo(char* direccion_archivo) {
//	// 1. Crear descriptor de archivo
//	FILE* a_swap = fopen(direccion_archivo, "r");
//
//	// 2. Leer archivo
//	printf("\n--------------------------------------------------------------------------------------------------- ARCHIVO ---------------------------------------------------------------------------------------------------\n");
//
//	uint32_t n;
//	uint32_t c = 0;
//	printf("PAG 000: ");
//	while((n = getw(a_swap)) != EOF) {
//		if(c % TAMANIO_PAGINA == 0 && c != 0) {
//			if(c % (TAMANIO_MARCO * 6) == 0)
//					printf("\n");
//
//			printf("PAG %03d: %05d ", (c / TAMANIO_PAGINA), n);
//		} else {
//			printf("%05d ", n);
//		}
//		c += 4;
//	}
//
//	printf("\n");
//
//
//	// 3. Cerrar descriptor de archivo
//	fclose(a_swap);
//}
//
//void mostrar_memoria() {
//	printf("\n--------------------------------------------------------------------------------------------------- MEMORIA --------------------------------------------------------------------------------------------------- \n");
//	uint32_t leido;
//	printf("MAR 000: ");
//	for(int d = 0; d < TAMANIO_MEMORIA; d += sizeof(uint32_t)) {
//		memcpy(&leido, memoria_test + d, sizeof(uint32_t));
//
//		if(d % TAMANIO_MARCO == 0 && d != 0) {
//			if(d % (TAMANIO_MARCO * 6) == 0)
//				printf("\n");
//
//			printf("MAR %03d: %05d ", (d / TAMANIO_MARCO), leido);
//
//		} else {
//			printf("%05d ", leido);
//		}
//
//	}
//	printf("\n");
//}
//
//
//void modificar_archivo(char* direccion_archivo, int pagina, int marco) {
//	// 1. Crear descriptor de archivo
//	FILE* a_swap = fopen(direccion_archivo, "r+");
//
//	// 2. Leer marco
//	void* valor_marco = malloc(TAMANIO_MARCO);
//
//	int desplazamiento_marco = marco * TAMANIO_MARCO;
//	memcpy(valor_marco, memoria_test + desplazamiento_marco, TAMANIO_MARCO);
//	//printf("\nMAR %d: %d\n", marco, *((uint32_t*) valor_marco));
//
//	// 3. Escribir archivo
//	// 3. a. Ubicar puntero
//	int desplazamiento_pagina = pagina * TAMANIO_PAGINA;
//	fseek(a_swap, desplazamiento_pagina, SEEK_SET);
//
//	// 3. b. Escribir
//	fwrite(valor_marco, 1, TAMANIO_MARCO, a_swap);
//	free(valor_marco);
//
//	// X. Cerrar descriptor de archivo
//	fclose(a_swap);
//}
//
//void modificar_memoria(char* direccion_archivo, int pagina, int marco) {
//	// 1. Crear descriptor de archivo
//	FILE* a_swap = fopen(direccion_archivo, "r+");
//
//	// 2. Leer pagina
//	// 2. a. Ubicar puntero
//	int desplazamiento_pagina = pagina * TAMANIO_PAGINA;
//	fseek(a_swap, desplazamiento_pagina, SEEK_SET);
//
//	// 2. b. Leer pagina
//	void* valor_pagina = malloc(TAMANIO_PAGINA);
//	uint32_t n;
//	for(int d = 0; d < TAMANIO_PAGINA; d += sizeof(uint32_t)) {
//		// Leer numero
//		n = getw(a_swap);
//
//		// Escribir numero
//		memcpy(valor_pagina + d, &n, sizeof(uint32_t));
//	}
//
//	// 3. Escribir memoria
//	uint32_t desplazamiento_marco = marco * TAMANIO_MARCO;
//
//	// Escribir valor en memoria
//	memcpy(memoria_test + desplazamiento_marco, valor_pagina, TAMANIO_MARCO);
//
//	// 4. Cerrar descriptor de archivo
//	fclose(a_swap);
//}
//
//void eliminar_archivo() {
//
//}

//int main() {
//	// 1. a. Crear memoria
//	memoria_test = malloc(TAMANIO_MEMORIA);
//
//	uint32_t a_escribir = 0;
//	for(int d = 0; d < TAMANIO_MEMORIA; d += sizeof(uint32_t)) {
//		memcpy(memoria_test + d, &(a_escribir), sizeof(uint32_t));
//		a_escribir++;
//	}
//
//	// 1. b. Leer memoria
//	mostrar_memoria();
//
//	// 2. a. Crear archivo
//	char* direccion_archivo = "/home/utnso/swap/1.swap";
//	crear_archivo(direccion_archivo, 128);
//
//	// 2. b. Leer archivo
//	mostrar_archivo(direccion_archivo);
//
//	// 3. a. Escribir memoria en archivo
//	//printf("\n[========================================================= MODIFICADO =========================================================]");
//
//	//modificar_archivo(direccion_archivo, 2, 3, 16);
//
//	// 3. b. Leer archivo modificado
//	//mostrar_archivo(direccion_archivo);
//
//	// 4. a. Escribir archivo en memoria
//	//modificar_memoria(direccion_archivo, 5, 16, 16);
//
//	//4. b. Leer memoria
//	//mostrar_memoria();
//
//	modificar_memoria(direccion_archivo, 0, 0);
//	modificar_memoria(direccion_archivo, 1, 255);
//	modificar_memoria(direccion_archivo, 2, 130);
//
//	modificar_archivo(direccion_archivo, 5, 0);
//	modificar_archivo(direccion_archivo, 6, 255);
//	modificar_archivo(direccion_archivo, 7, 130);
//
//	printf("\n[================================================================================================== RESULTADOS ==================================================================================================]");
//	mostrar_archivo(direccion_archivo);
//	mostrar_memoria();
//
//	return EXIT_SUCCESS;
//}

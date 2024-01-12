#include <mmu/crear-direccion.h>

int calcular_numero_pagina(int direccion_logica) {
	return (int) floor((double) direccion_logica / config_cpu->tamanio_pagina);
}

int calcular_entrada_tabla_1er_nivel(int numero_pagina) {
	return (int) floor((double) numero_pagina / config_cpu->cantidad_entradas_tabla_paginas);
}

int calcular_entrada_tabla_2do_nivel(int numero_pagina) {
	return numero_pagina % config_cpu->cantidad_entradas_tabla_paginas;
}

int calcular_desplazamiento(int direccion_logica, int numero_pagina) {
	return direccion_logica - numero_pagina * config_cpu->tamanio_pagina;
}

t_direccion_logica* crear_direccion_logica(int direccion_logica) {
	t_direccion_logica* d_logica = malloc(sizeof(t_direccion_logica));

	d_logica->numero_pagina = calcular_numero_pagina(direccion_logica);
	d_logica->entrada_tabla_1er_nivel = calcular_entrada_tabla_1er_nivel(d_logica->numero_pagina);
	d_logica->entrada_tabla_2do_nivel = calcular_entrada_tabla_2do_nivel(d_logica->numero_pagina);
	d_logica->desplazamiento = calcular_desplazamiento(direccion_logica, d_logica->numero_pagina);

	return d_logica;
}

t_direccion_fisica* crear_direccion_fisica(int marco, int desplazamiento) {
	t_direccion_fisica* d_fisica = malloc(sizeof(t_direccion_fisica));

	d_fisica->marco = marco;
	d_fisica->desplazamiento = desplazamiento;

	return d_fisica;
}

#include <ciclo-instruccion/ejecucion-instruccion.h>

void ejecutar_noop(int retardo_noop) {
	int retardo_noop_microsegundos = retardo_noop * 1000;
	usleep(retardo_noop_microsegundos);
}

void devolver_paquete_pcb(t_pcb* pcb, int conexion) {
	t_instruccion* instruccion = list_get(pcb->l_instrucciones, pcb->program_counter - 1);

	pcb->tiempo_bloqueo = 0;
	pcb->estimacion_rafaga_anterior = pcb->estimacion_rafaga;

	switch(instruccion->identificador) {
		case I_O:
			pcb->tiempo_bloqueo = instruccion->parametros[0];
			pcb->estado = BLOCKED;
			break;
		case EXIT_I:
			pcb->estado = EXIT_P;
			break;
		default: // Interrupcion
			pcb->estado = READY;
			break;
	}

	t_paquete* paquete = crear_paquete(PCB);

	agregar_pcb_a_paquete(paquete, pcb);

	enviar_paquete(paquete, conexion);
	eliminar_paquete(paquete);
}

int ejecutar_read(int direccion_logica, t_pcb* pcb, int socket_cliente) {
	// 1. Calcular direccion logica
	t_direccion_logica* d_logica = crear_direccion_logica(direccion_logica);

	// 2. Obtener direccion fisica
	t_direccion_fisica* direccion_fisica = obtener_direccion_fisica(d_logica, pcb, socket_cliente);
	log_info(logger, "    ~ [MMU] M: #%05d - D: #%05d", direccion_fisica->marco, direccion_fisica->desplazamiento);

	// 3. Leer valor en el marco
	int valor_marco = obtener_valor_marco(direccion_fisica, d_logica->numero_pagina, socket_cliente);
	log_info(logger, "    ~ [MEM] VALOR: %d", valor_marco);

	free(d_logica);
	free(direccion_fisica);

	return valor_marco;
}

void ejecutar_write(int direccion_logica, int a_escribir, t_pcb* pcb, int socket_cliente) {
	// 1. Calcular direccion logica
	t_direccion_logica* d_logica = crear_direccion_logica(direccion_logica);

	// 2. Obtener direccion fisica
	t_direccion_fisica* direccion_fisica = obtener_direccion_fisica(d_logica, pcb, socket_cliente);
	log_info(logger, "    ~ [MMU] M: #%05d - D: #%05d", direccion_fisica->marco, direccion_fisica->desplazamiento);

	// 3. Escribir valor en el marco
	int resultado_operacion = enviar_valor_a_escribir(direccion_fisica, d_logica->numero_pagina, a_escribir, socket_cliente);

	if(resultado_operacion)
		log_info(logger, "    ~ [MEM] ESCRITURA EXITOSA");
	else
		log_info(logger, "    ~ [MEM] ESCRITURA FALLIDA");

	free(d_logica);
	free(direccion_fisica);
}

void ejecutar_copy(int direccion_logica_destino, int direccion_logica_origen, t_pcb* pcb, int socket_cliente) {
	int a_escribir = ejecutar_read(direccion_logica_origen, pcb, socket_cliente);
	ejecutar_write(direccion_logica_destino, a_escribir, pcb, socket_cliente);
}


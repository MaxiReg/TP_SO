#include <ciclo-instruccion/ciclo-instruccion.h>

void escuchar_interrupciones() {
	// Conectar con Interrupt
	int socket_servidor_interrupt = iniciar_servidor(config_cpu->ip, config_cpu->puerto_escucha_interrupt);
	int socket_cliente = aceptar_cliente(socket_servidor_interrupt);

	log_info(logger, "[=================== HILO #2: INTERRUPT ================]\n");

	while(1) {
		uint8_t interrupcion;
		recv(socket_cliente, &interrupcion, sizeof(uint8_t), 0);

	    pthread_mutex_lock(&mx_interrupcion);
	    hay_interrupcion = interrupcion;
	    pthread_mutex_unlock(&mx_interrupcion);

	    if(hay_interrupcion) { // TODO; PREGUNTAR POR QUE
	    	log_info(logger, "[===================== INTERRUPCION ====================]\n");
		}
	}
}

int procesar_instruccion(t_pcb* pcb, int socket_cliente) {
	int identificador_instruccion = 0;
	// 1. Fetch
	t_instruccion* instruccion = list_get(pcb->l_instrucciones, pcb->program_counter);

	// X. Siguiente instruccion
	pcb->program_counter++;

	switch(instruccion->identificador) { // 2. Decode
		case NO_OP:
			log_info(logger, "   > [H #1] NO_OP ... %dms", config_cpu->retardo_noop);
			ejecutar_noop(config_cpu->retardo_noop); // 3. Execute (para todos los ejecutar_...())
			break;
		case I_O:
			log_info(logger, "   > [H #1] I/O ..... %dms", instruccion->parametros[0]);
			identificador_instruccion = I_O;
			break;
		case READ:
			printf("\n");
			log_info(logger, "   > [H #1] READ .... D: #%05d", instruccion->parametros[0]);
			ejecutar_read(instruccion->parametros[0], pcb, socket_cliente);
			printf("\n");
			break;
		case WRITE:
			printf("\n");
			log_info(logger, "   > [H #1] WRITE ... DD: #%05d <- V: %05d", instruccion->parametros[0], instruccion->parametros[1]);
			ejecutar_write(instruccion->parametros[0], instruccion->parametros[1], pcb, socket_cliente);
			printf("\n");
			break;
		case COPY:
			printf("\n");
			log_info(logger, "   > [H #1] COPY .... DD: #%05d | DO: #%05d", instruccion->parametros[0], instruccion->parametros[1]);
			ejecutar_copy(instruccion->parametros[0], instruccion->parametros[1], pcb, socket_cliente); //3. b. Fetch Operands
			printf("\n");
			break;
		case EXIT_I:
			log_info(logger, "   > [H #1] EXIT");
			identificador_instruccion = EXIT_I;
			break;
	}

	return identificador_instruccion;
}

void ciclo_instruccion() {
	// 0. Conexiones con modulos
	// 0. a. Conectar con Memoria
	int socket_cliente_memoria = crear_conexion(config_cpu->ip, config_cpu->puerto_memoria);

	// 0. b. Conectar con Dispatch
	int socket_servidor_ciclo = iniciar_servidor(config_cpu->ip, config_cpu->puerto_escucha_dispatch);
	int socket_cliente = aceptar_cliente(socket_servidor_ciclo);

	log_info(logger, "[================== HILO #1: DISPATCH ==================]\n");

	// 1. Recibir primer PCB
	t_pcb* pcb;

	int operacion = recibir_operacion(socket_cliente);
	if(operacion == PCB) {
		pcb = recibir_paquete_pcb(socket_cliente);
		log_info(logger, "[============= [H #1] RECIBIR PCB [PID #%03d] ===========]", pcb->pid);
	}

	// 2. Crear TLB general
	tlb = list_create();

	// 3. Ciclo de instruccion
	int devolver_pcb = 0;
	while(1) {
		// 3. a. Procesar siguiente instruccion
		devolver_pcb = procesar_instruccion(pcb, socket_cliente_memoria);

		// 3. b. Devolver PCB en caso de interrupcion / bloqueo / finalizacion
		pthread_mutex_lock(&mx_interrupcion);
		if(hay_interrupcion || devolver_pcb) {
			// Devolver PCB
			devolver_paquete_pcb(pcb, socket_cliente);

			// Resetear estructuras administrativas
			hay_interrupcion = 0;

			devolver_pcb = 0;

		    limpiar_tlb(tlb);
			log_info(logger, "   * [TLB] VACIADA");

			log_info(logger, "[===================== PCB DEVUELTO ====================]\n");

			// 3. c.  Recibir siguiente PCB
			operacion = recibir_operacion(socket_cliente);
			if(operacion == PCB) {
				liberar_pcb(pcb); // TODO: TENER CUIDADin
				pcb = recibir_paquete_pcb(socket_cliente);
				log_info(logger, "[============= [H #1] RECIBIR PCB [PID #%03d] ===========]", pcb->pid);
			}
		}
	    pthread_mutex_unlock(&mx_interrupcion);
	}

    list_destroy_and_destroy_elements(tlb, free);
	liberar_pcb(pcb);
}

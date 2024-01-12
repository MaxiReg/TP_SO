#include <kernel.h>

int main(void) {
	iniciar_configuracion();

	iniciar_listas();

	iniciar_semaforos();

	iniciar_planificacion();

	int socket_servidor = generar_conexion();

	if(!socket_servidor){
		destruir_listas();
		destruir_semaforos();
	}

	socket_memoria = crear_conexion(ip_Memoria, puerto_Memoria);

	while(server_escuchar("KERNEL",socket_servidor));


	liberar_pcb(procesoAEjecutar);
	destruir_listas();
	destruir_semaforos();

	terminar_programa(socket_servidor, config);

	log_destroy(args->log);
	free(args);
	return EXIT_SUCCESS;
}


void iniciar_configuracion(){

	logger = iniciar_logger();
	config = iniciar_config(logger);

	ip_Memoria = obtener_de_config(config, "IP_MEMORIA");
	puerto_Memoria = obtener_de_config(config, "PUERTO_MEMORIA");

	ip_cpu = obtener_de_config(config, "IP_CPU");
	puerto_cpu_interrupt = obtener_de_config(config, "PUERTO_CPU_INTERRUPT");
	puerto_cpu_dispatch = obtener_de_config(config, "PUERTO_CPU_DISPATCH");

	puerto_escucha = obtener_de_config(config, "PUERTO_ESCUCHA");

	algoritmoPlanificacion = obtener_algoritmo();

	estimacion_inicial = obtener_int_de_config(config,"ESTIMACION_INICIAL");

	alfa = obtener_float_de_config(config,"ALFA");

	grado_multiprogramacion = obtener_int_de_config(config,"GRADO_MULTIPROGRAMACION");

	tiempo_maximo_bloqueado = obtener_int_de_config(config,"TIEMPO_MAXIMO_BLOQUEADO");

	pidKernel = 0;

	log_info(logger, "[************* CONFIGURACIONES MEMORIA+SWAP ************]");
	log_info(logger, "   * IP MEMORIA ................... %s", ip_Memoria);
	log_info(logger, "   * PUERTO MEMORIA ............... %s", puerto_Memoria);
	log_info(logger, "   * IP CPU ....................... %s", ip_cpu);
	log_info(logger, "   * PUERTO CPU INTERRUPT ......... %s", puerto_cpu_interrupt);
	log_info(logger, "   * PUERTO CPU DISPATCH .......... %s", puerto_cpu_dispatch);
	log_info(logger, "   * PUERTO ESCUCHA ............... %s", puerto_escucha);

	if(algoritmoPlanificacion == FIFO)
		log_info(logger, "   * ALG. PLANIFICACION ........... FIFO");
	else
		log_info(logger, "   * ALG. PLANIFICACION ........... SRT");

	log_info(logger, "   * ESTIMACION INICIAL ........... %dms", estimacion_inicial);
	log_info(logger, "   * ALFA ......................... %.2f", alfa);
	log_info(logger, "   * GRADO MULTIPROGRAMACION ...... %d", grado_multiprogramacion);
	log_info(logger, "   * TIEMPO MAXIMO BLOQUEADO ...... %dms", tiempo_maximo_bloqueado);
	log_info(logger, "[*******************************************************]\n");


	if(grado_multiprogramacion <= 0){

			perror("Puede que el grado de multiprocesamiento sea mayor al de multiprogramacion, o que alguno de los dos sea menor o igual a cero, revise el archivo de configuracion del Kernel para verificar que eso no suceda.\n");
			abort();
		}

	if(algoritmoPlanificacion != SRT && algoritmoPlanificacion != FIFO){

			perror("El algoritmo de planificacion ingresado no esta disponible en este sistema o esta mal escrito, revise el archivo de configuracion del Kernel para verificar que eso no suceda.\n");
			abort();
		}

	if(alfa < 0 || alfa > 1){

			perror("El alfa debe estar acotado entre 0 y 1, revise el archivo de configuracion del Kernel para verificar que ese sea el caso.\n");
			abort();
		}

}

// CREAR PCB DENTRO DE ESTA FUNCION
// Crear struct con 2 campos (pcb y procesar_proceso)

void procesar_conexion_consola(void* void_args) {

    t_procesar_conexion_args* args = (t_procesar_conexion_args*) void_args;
    t_log* logger = args->log;
    int cliente_socket = args->socket_cliente;
    char* server_name = args->server_name;

		int codigo_operacion;
		do { // Cambiar para que termine cuando el codigo de operación sea: "FIN"
			codigo_operacion = recibir_operacion(cliente_socket);

			switch (codigo_operacion) {
				case INSTRUCCIONES:;
					t_list* l_instrucciones = recibir_paquete_instrucciones(cliente_socket);
					int tamanio_proceso = (int) list_remove(l_instrucciones, 0);

				    t_pcb* pcb = malloc(sizeof(t_pcb));

				    pcb->pid = pidKernel;

				    pthread_mutex_lock(&mutexVectorConsolas);
				    pidKernel++;
				    list_add(vectorSocketsConsolas, (void*) cliente_socket);
				    pthread_mutex_unlock(&mutexVectorConsolas);

				    pcb->l_instrucciones = l_instrucciones;
				    pcb->tamanio = tamanio_proceso;
					pcb->estado = NEW;
					pcb->estimacion_rafaga = estimacion_inicial;
					pcb->tiempo_entro_a_ejecutar = 0;
					pcb->tiempo_salio_de_ejecutar = 0;
					pcb->tabla_paginas = 0;
					pcb->tiempo_bloqueo = 0;
					pcb->program_counter = 0;
					pcb->estimacion_rafaga_anterior = estimacion_inicial;

					agregarANew(pcb);

					log_info(logger, "[==================== INFO. PROCESO ====================]");
					log_info(logger, "   * TAMAÑO PROCESO: %db", tamanio_proceso);
					//imprimir_instrucciones(logger, l_instrucciones);
					break;
				case FIN:
					log_warning(logger, "[================= CLIENTE DESCONECTADO ================]");
					break;
				default:
					log_warning(logger,"=========== OPERACIÓN DESCONOCIDA ==========");
					break;
			}
		} while(codigo_operacion != FIN);
}

int server_escuchar(char* server_name, int server_socket) {
    int cliente_socket = aceptar_cliente(server_socket);

	if (cliente_socket != -1) {
		log_info(logger, "[=================== CLIENTE CONECTADO =================]");

	   pthread_t hilo;
	   args = malloc(sizeof(t_procesar_conexion_args));
	   args->log = logger;
	   args->socket_cliente = cliente_socket;
	   args->server_name = server_name;

	   pthread_create(&hilo, NULL, (void*) procesar_conexion_consola, (void*) args);
	   pthread_detach(hilo);


		return 1;
	}
    return 1;
}

int generar_conexion(){
	int socket_servidor = iniciar_servidor("0.0.0.0", puerto_escucha);
return socket_servidor;

}


void iniciar_listas(){

	listaSemaforos = list_create();
	colaNew = queue_create();
	listaReady = list_create();
	listaExe = list_create();
	listaBlock = list_create();
	listaExit = list_create();
	listaBlockSuspended = list_create();
	colaReadySuspended = queue_create();
	vectorSocketsConsolas = list_create();
	archivos_creados = list_create();

}

void destruir_listas(){


	liberarListaDeSemaforos();
	destruirColaYElementos(colaNew);
	destruirListaYElementos(listaReady);
	destruirListaYElementos(listaExe);
	destruirListaYElementos(listaBlock);
	destruirListaYElementos(listaExit);
	destruirListaYElementos(listaBlockSuspended);
	destruirColaYElementos(colaReadySuspended);

	log_info(logger,"Se Destruyeron las listas");

}

void liberarListaDeSemaforos(){

	for(int i=0; i < list_size(listaSemaforos); i++){

		t_semaforo* semaforoAux = list_get(listaSemaforos, i);
		queue_destroy(semaforoAux->listaDeEspera);
		pthread_mutex_destroy(&(semaforoAux->mutexSemaforo));
		free(semaforoAux);
	}
}

void destruirListaYElementos(t_list* unaLista){
    list_destroy_and_destroy_elements(unaLista,free);
}

void destruirColaYElementos(t_queue* unaCola){
    queue_destroy_and_destroy_elements(unaCola, free);
}


void iniciar_semaforos(){

	pthread_mutex_init(&mutexBlockSuspended, NULL);
	pthread_mutex_init(&mutexReadySuspended, NULL);
	pthread_mutex_init(&mutexNew, NULL);
	pthread_mutex_init(&mutexReady, NULL);
	pthread_mutex_init(&mutexBlock, NULL);
	pthread_mutex_init(&mutexExe, NULL);
	pthread_mutex_init(&mutexExit, NULL);
	pthread_mutex_init(&mutexVectorConsolas,NULL);
	pthread_mutex_init(&semaforoPcbAEjecutar,NULL);
	pthread_mutex_init(&gradomulti,NULL);


	//Semaforo para controlar la cantidad de procesos cargados en memoria
	sem_init(&multiprogramacion , 0 , grado_multiprogramacion);
	sem_init(&gradoMultiProcesamiento,0,1);
	//Semaforo binario para el orden de ejecucion del planificador largo
	sem_init(&largoPlazo, 0, 0); //TODO: verificar si es mejor iniciarlo en 1
	sem_init(&medianoPlazo, 0, 0);
	sem_init(&cortoPlazo, 0, 0);
	sem_init(&sendInstrucion,0,0);

	sem_init(&contadorReady, 0, 0);
	sem_init(&contadorExe,0,0);
	sem_init(&contadorBlock,0,0);
	sem_init(&contadorReadySuspended,0,0);
	sem_init(&semProcesoEsperandoDispositvoIO,0,0);
	sem_init(&entroReady,0,0);
}

void iniciar_planificacion(){

	/*CREAMOS HILOS*/
	pthread_create(&hilo_Dispatch,NULL,(void*) ciclo_instruccion,NULL);
	pthread_create(&hilo_Interrup,NULL,(void*) enviar_interrupcion,NULL);
	pthread_create(&hiloNewReady, NULL, (void*)hiloNew_Ready, NULL);
	pthread_create(&hiloReady_Exec, NULL, (void*)hiloReady_Exe, NULL);
	pthread_create(&hiloSuspendedAReady, NULL, (void*)hilo_suspendedAReady, NULL);
	pthread_create(&hiloAtencionIO, NULL, (void*)hilo_paralelo_atencion_IO , NULL);

	/*DETACH HILOS*/
	pthread_detach(hilo_Dispatch);
	pthread_detach(hiloNewReady);
	pthread_detach(hiloReady_Exec);
	pthread_detach(hiloSuspendedAReady);
	pthread_detach(hiloAtencionIO);
	pthread_detach(hilo_Interrup);

}

void destruir_semaforos(){

	pthread_mutex_destroy(&mutexBlockSuspended);
	pthread_mutex_destroy(&mutexReadySuspended);
	pthread_mutex_destroy(&mutexNew);
	pthread_mutex_destroy(&mutexReady);
	pthread_mutex_destroy(&mutexBlock);
	pthread_mutex_destroy(&mutexExe);
	pthread_mutex_destroy(&mutexExit);
	pthread_mutex_destroy(&semaforoPcbAEjecutar);

	sem_destroy(&multiprogramacion);
	sem_destroy(&largoPlazo); //TODO: verificar si es mejor iniciarlo en 1
	sem_destroy(&medianoPlazo);
	sem_destroy(&cortoPlazo);
	sem_destroy(&sendInstrucion);
	sem_destroy(&contadorReady);
	sem_destroy(&contadorExe);
	sem_destroy(&contadorBlock);
	sem_destroy(&contadorReadySuspended);
	sem_destroy(&semProcesoEsperandoDispositvoIO);
	sem_destroy(&gradoMultiProcesamiento);
	sem_destroy(&entroReady);
}



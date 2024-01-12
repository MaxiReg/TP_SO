#include <comunicacionCPU.h>

void cargar_paquete_pcb(int conexion, t_pcb* proceso){
	t_paquete* paquete = crear_paquete(PCB);

	agregar_pcb_a_paquete(paquete, proceso);

	enviar_paquete(paquete, conexion);
	eliminar_paquete(paquete);
}

void ciclo_instruccion(){
	log_info(logger, "[============== HILO #6: CICLO INSTRUCCION =============]\n");

	int socket_servidor = crear_conexion(ip_cpu, puerto_cpu_dispatch);
	t_pcb* pcb;

	while(1) {
		sem_wait(&sendInstrucion);

		// 1. Enviar PCB a CPU
		cargar_paquete_pcb(socket_servidor, procesoAEjecutar);

		// 2. Esperar a que CPU devuelva el PCB
		int operacion = recibir_operacion(socket_servidor);

		if(operacion == PCB) {
			// Recibir PCB
			pcb = recibir_paquete_pcb(socket_servidor);

			printf("\n");
			log_info(logger, "[============= [H #6] RECIBIR PCB [PID #%03d] ===========]\n", pcb->pid);

			// Timestamp proceso salio de EXEC
			time_t tiempo_salida = time(NULL);
			sacarDeExe(pcb);

			int64_t tiempo_ejecutado = (int64_t) tiempo_salida - (procesoAEjecutar->tiempo_entro_a_ejecutar);

			switch(pcb->estado) {
				case EXIT_P:

					sem_post(&multiprogramacion);

					// Comunicacion de memoria
					enviar_pcb_reducido(pcb, socket_memoria); //TODO: FALTA RECIBIR MENSAJE DE MEMORIA
					uint32_t pid_eliminado = (uint32_t) recibir_mensaje(socket_memoria);
					log_info(logger, "   - [PID #%03d] ESPACIO LIBERADO MEMORIA", pid_eliminado);

					enviar_mensaje(1, sizeof(uint32_t), (int) list_get(vectorSocketsConsolas, pcb->pid));
					printf("\n");
					log_info(logger, "[========== [H #6] TERMINAR PROCESO [PID #%03d] =========]", pcb->pid);

					liberar_pcb(pcb);
					break;
				case BLOCKED:
					pcb->estimacion_rafaga = obtener_proxima_estimacion(pcb);
					pcb->estimacion_rafaga_anterior = pcb->estimacion_rafaga;
					agregarABlocked(pcb);

					crear_hilo_chequeo_tiempo_maximo((void*)pcb);
					sem_post(&semProcesoEsperandoDispositvoIO);
					break;
				case READY:
					pcb->estimacion_rafaga = procesoAEjecutar->estimacion_rafaga - (tiempo_ejecutado * 1000);
					agregarAReady(pcb);
					break;
			}

		}
	}

}

void enviar_interrupcion() {
	int socket_servidor = crear_conexion(ip_cpu, puerto_cpu_interrupt);
	log_info(logger, "[============= HILO #5: ENVIAR INTERRUPCION ============]\n");

	while(1){
		sem_wait(&entroReady);

		if(replanificar()){
			log_warning(logger, "REPLANIFIQUE");
			uint8_t interrupcion = 1;
			send(socket_servidor , &interrupcion , sizeof(uint8_t) , 0);
		}
	}
}

bool replanificar(){
	time_t tiempoActual = time(NULL);
	//Me quedo con el pcb que tenga la estimacion mas chica de los que estan en Ready
	pthread_mutex_lock(&mutexReady);
	int tamanioListaReady = list_size(listaReady);
	pthread_mutex_unlock(&mutexReady);

	t_pcb* pcbReady;

	if(tamanioListaReady > 0 ){

		pcbReady = procesos_en_ready();
		int64_t estimacion_ready = pcbReady->estimacion_rafaga;

		int64_t tiempo_ejecutado = (int64_t) tiempoActual - (procesoAEjecutar->tiempo_entro_a_ejecutar);
		int64_t estimacionProcesoEnEjecucion = procesoAEjecutar->estimacion_rafaga - tiempo_ejecutado*1000;


		if(estimacion_ready < estimacionProcesoEnEjecucion) {
			return true; //Hay que replanificar
		} else {
			log_warning(logger, "NO HAY REPLANIFICACION");
			return false; //No hay que replanificar
		}
	} else {
		return false;
	}

}

t_pcb* procesos_en_ready() {
	t_pcb* procesoPlanificado;
	int i, indexARemover;
	int64_t menorEstimacion;

	pthread_mutex_lock(&mutexReady);
	t_pcb* procesoAux = list_get(listaReady,0);
	pthread_mutex_unlock(&mutexReady);

	indexARemover = 0;
	menorEstimacion = procesoAux->estimacion_rafaga;

	//itero por la lista de Ready
	pthread_mutex_lock(&mutexReady);

	for(i=1;i<list_size(listaReady);i++){
		procesoAux = list_get(listaReady,i);

		if(menorEstimacion > procesoAux->estimacion_rafaga){
			menorEstimacion = procesoAux->estimacion_rafaga;
			indexARemover = i;
		}
	}

	procesoPlanificado = list_get(listaReady, indexARemover);
	pthread_mutex_unlock(&mutexReady);

	return procesoPlanificado;
}

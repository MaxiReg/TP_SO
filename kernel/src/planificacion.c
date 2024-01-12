#include <planificacion.h>

// FUNCIONES VARIAS
int tamanioDeLista(t_list* lista){
	return list_size(lista);
}

void verificarGradoMulti(){
	int tamanioReady = tamanioDeLista(listaReady);
	int tamanioExe = tamanioDeLista(listaExe);
	int tamanioBlock = tamanioDeLista(listaBlock);

	int tamTotal = tamanioReady + tamanioExe + tamanioBlock;

	pthread_mutex_lock(&gradomulti);
	if(tamTotal >= grado_multiprogramacion)
		log_warning(logger, "[===== GRADO MULTIPROGRAMACION MAXIMO ALCANZADO =====]");
	pthread_mutex_unlock(&gradomulti);

}

t_algoritmo_planificacion obtener_algoritmo(){

	 t_algoritmo_planificacion switcher;
	 char* algoritmo = obtener_de_config(config, "ALGORITMO_PLANIFICACION");
	    //FIFO
	    if (strcmp(algoritmo, "FIFO") == 0)
	        switcher = FIFO;

	    //SJF CON DESALOJO
	    if (strcmp(algoritmo, "SRT") == 0)
	        switcher = SRT;

	    return switcher;
}

t_pcb* obtenerSiguienteFIFO(){
	pthread_mutex_lock(&mutexReady);
	t_pcb* siguientePcb = list_get(listaReady,0);
	list_remove(listaReady, 0);
	pthread_mutex_unlock(&mutexReady);

	return siguientePcb;
}

t_pcb* obtenerSiguienteSJF(){
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

	//list_iterate(listaReady, obtener_proxima_estimacion);

	for(i=1;i<list_size(listaReady);i++){
		procesoAux = list_get(listaReady,i);

		if(menorEstimacion > procesoAux->estimacion_rafaga){
			menorEstimacion = procesoAux->estimacion_rafaga;
			indexARemover = i;
		}
	}

	procesoPlanificado = list_get(listaReady, indexARemover);

	list_remove(listaReady,indexARemover);

	pthread_mutex_unlock(&mutexReady);

	return procesoPlanificado;
}

t_pcb* obtenerSiguienteDeReady(){
	sem_wait(&contadorReady);
	int tamanioDeListaReady = tamanioDeLista(listaReady);

	t_pcb* pcbPlanificado;

	if(tamanioDeListaReady > 0){
		// SWITCH para los distintos algoritmos que llama a una funcion para cada uno
		switch(algoritmoPlanificacion){
			//CASO FIFO
			case FIFO:
				pcbPlanificado = obtenerSiguienteFIFO();
				break;
			case SRT:
				pcbPlanificado = obtenerSiguienteSJF();
				break;
		}
	}

	log_info(logger, "   > [PID #%03d] SALE DE COLA (READY)", pcbPlanificado->pid);

	return pcbPlanificado;
}

// SEMAFOROS LISTAS Y COLAS
void agregarANew(t_pcb* pcb){
	pthread_mutex_lock(&mutexNew);
	queue_push(colaNew,pcb);
	log_info(logger, "   > [PID #%03d] ENTRA A COLA (NEW)", pcb->pid);
	pthread_mutex_unlock(&mutexNew);

	list_add(archivos_creados, 0);

	sem_post(&largoPlazo);
}

t_pcb* sacarDeNew(){
	pthread_mutex_lock(&mutexNew);

	t_pcb* pcb = queue_pop(colaNew);
	log_info(logger, "   > [PID #%03d] SALE DE COLA (NEW)", pcb->pid);

	pthread_mutex_unlock(&mutexNew);


	return pcb;
}


void agregarAReady(t_pcb* pcbReady){

	if(pcbReady -> estado != BLOCKED && pcbReady -> estado != READY && pcbReady -> estado != EXEC){
		verificarGradoMulti();
		sem_wait(&multiprogramacion);
	}

	pthread_mutex_lock(&mutexReady);

	list_add(listaReady, pcbReady);

	int creado = (int) list_get(archivos_creados, pcbReady->pid);
	if(!creado){
		pcbReady->estado = NEW; //TODO: Cambiar, es para que no rompa memoria
		enviar_pcb_reducido(pcbReady, socket_memoria);
		pcbReady->tabla_paginas = (uint32_t) recibir_mensaje(socket_memoria);
		log_info(logger, "   - [PID #%03d] ESTRUCTURAS DE MEMORIA CREADAS", pcbReady->pid);
		list_replace_and_destroy_element(archivos_creados, pcbReady->pid, (void*) 1, free);
	}

	pcbReady->estado = READY;
	log_info(logger, "   > [PID #%03d] ENTRA A COLA (READY)", pcbReady->pid);

	pthread_mutex_unlock(&mutexReady);


	if (algoritmoPlanificacion == SRT && procesoAEjecutar != NULL && pcbReady != procesoAEjecutar && list_size(listaExe) > 0) {
		sem_post(&entroReady);
	}

	sem_post(&contadorReady);
	sem_post(&cortoPlazo);

}

// TRANSICIONES POR HILOS

// Hilo que mueve los procesos de new a ready	-	CASO FIFO
void hiloNew_Ready(){
	log_info(logger, "[============ HILO #4: TRANSICION NEW/READY ============]\n");

	while(1){

		sem_wait(&largoPlazo); //Trabo la ejecucion del hilo hasta que se agregue algo a new
		if(queue_size(colaReadySuspended) != 0){
			t_pcb* pcbSuspendedReady = sacarDeReadySuspended();
			agregarAReady(pcbSuspendedReady);

		} else {
			t_pcb* pcbReady = sacarDeNew();
			agregarAReady(pcbReady);
		}
	}
}


// Hilo que mueve los procesos de Ready a Exe	-	CASO FIFO / SJF CD
void hiloReady_Exe(){
	log_info(logger, "[============ HILO #2: TRANSICION READY/EXE ============]\n");

	while(1){

		sem_wait(&cortoPlazo);
		sem_wait(&gradoMultiProcesamiento);
		pthread_mutex_lock(&semaforoPcbAEjecutar);
		procesoAEjecutar = obtenerSiguienteDeReady();

		//Elegimos el siguiente proceso segun el algoritmo
		pthread_mutex_unlock(&semaforoPcbAEjecutar);

		if(procesoAEjecutar != NULL) {
			pthread_mutex_lock(&mutexExe);
			list_add(listaExe, procesoAEjecutar);
			sem_post(&contadorExe);

			if(algoritmoPlanificacion == FIFO){
				log_info(logger, "   > [PID #%03d] ENTRA A COLA (EXEC)", procesoAEjecutar->pid);
			} else if(algoritmoPlanificacion == SRT) {
				log_info(logger, "   > [PID #%03d] ENTRA A COLA (EXEC) (EST.: %lldms)", procesoAEjecutar->pid, procesoAEjecutar->estimacion_rafaga);
			}

			procesoAEjecutar->estado = EXEC;

			time_t tiempoEntroEjecutar = time(NULL);
			procesoAEjecutar->tiempo_entro_a_ejecutar = (int64_t) tiempoEntroEjecutar;

			pthread_mutex_unlock(&mutexExe);

			sem_post(&sendInstrucion);

		} else {
			sem_post(&cortoPlazo);
		}

	}
}


void sacarDeExe(t_pcb* pcb){
	time_t tiempoSalioEjecutar = time(NULL);

	bool tienenMismoPID(void* elemento) {
		if(pcb-> pid == ((t_pcb *) elemento)-> pid)
			return true;
		else
			return false;
	}

	sem_wait(&contadorExe);
	pthread_mutex_lock(&mutexExe);
	list_remove_by_condition(listaExe, tienenMismoPID);
	log_info(logger, "   > [PID #%03d] SALE DE COLA (EXEC)", pcb->pid);

	pcb->tiempo_salio_de_ejecutar = (uint32_t) tiempoSalioEjecutar;

	pthread_mutex_unlock(&mutexExe);
	sem_post(&gradoMultiProcesamiento);
}

void agregarABlocked(t_pcb* pcb_blocked){
	pthread_mutex_lock(&mutexBlock);

	list_add(listaBlock, pcb_blocked);
	pcb_blocked->estado = BLOCKED;

	log_info(logger, "   > [PID #%03d] ENTRA A COLA (BLOCKED)", pcb_blocked->pid);

	pthread_mutex_unlock(&mutexBlock);
	sem_post(&contadorBlock);
}


void sacarDeBlock(t_pcb* pcb){
	sem_wait(&contadorBlock);

	bool tienenMismoPID(void* elemento){
		if(pcb->pid == ((t_pcb *) elemento)->pid)
			return true;
		else
			return false;
	}

	pthread_mutex_lock(&mutexBlock);

	list_remove_by_condition(listaBlock, tienenMismoPID);

	log_info(logger, "   > [PID #%03d] SALE DE COLA (BLOCKED)", pcb->pid);

	pthread_mutex_unlock(&mutexBlock);
}

void agregarABlockSuspended(t_pcb* pcbSuspended){

	pthread_mutex_lock(&mutexBlockSuspended);

	pcbSuspended->estado = SUSPENDED_BLOCKED;

	log_info(logger, "   > [PID #%03d] ENTRA A COLA (BLOCKED-SUSPENDED)", pcbSuspended->pid);

	// ENVIAR PCB A SUSPENDER
	enviar_pcb_reducido(pcbSuspended, socket_memoria);
	uint32_t pid_suspendido = (uint32_t) recibir_mensaje(socket_memoria);
	log_info(logger, "   - [PID #%03d] MENSAJE DE SUSPENSION ENVIADO A MEMORIA", pid_suspendido);

	pthread_mutex_unlock(&mutexBlockSuspended);

	sem_post(&multiprogramacion);

}

void sacarDeBlockSuspended(t_pcb* pcbBlockedSuspended){

	bool tienenMismoPID(void* elemento){

	if(pcbBlockedSuspended->pid == ((t_pcb *) elemento)->pid)
		return true;
	else
		return false;
	}

	pthread_mutex_lock(&mutexBlockSuspended);

	list_remove_by_condition(listaBlockSuspended, tienenMismoPID);
	log_info(logger, "   > [PID #%03d] SALE DE COLA (BLOCKED-SUSPENDED)", pcbBlockedSuspended->pid);

	pthread_mutex_unlock(&mutexBlockSuspended);

	agregarAReadySuspended(pcbBlockedSuspended);
}

void agregarAReadySuspended(t_pcb* pcbReadySuspended){
	pthread_mutex_lock(&mutexReadySuspended);

	queue_push(colaReadySuspended, pcbReadySuspended);
	pcbReadySuspended->estado = SUSPENDED_READY;

	log_info(logger, "   > [PID #%03d] ENTRA A COLA (READY-SUSPENDED)", pcbReadySuspended->pid);

	pthread_mutex_unlock(&mutexReadySuspended);

	sem_post(&medianoPlazo);
	sem_post(&contadorReadySuspended);

}

t_pcb* sacarDeReadySuspended(){
	sem_wait(&contadorReadySuspended);

	pthread_mutex_lock(&mutexReadySuspended);

	t_pcb* pcbSuspendedReady = queue_pop(colaReadySuspended);

	log_info(logger, "   > [PID #%03d] SALE DE COLA (READY-SUSPENDED)", pcbSuspendedReady->pid);

	pthread_mutex_unlock(&mutexReadySuspended);

	return pcbSuspendedReady;
}

void hilo_suspendedAReady(){
	log_info(logger, "[========== HILO #3: TRANSICION S-READY/READY ==========]\n");

	while(1){
		sem_wait(&medianoPlazo);

		if(queue_size(colaReadySuspended) == 0){
			sem_post(&largoPlazo);
		} else {
			t_pcb* pcbSuspended = sacarDeReadySuspended();
			agregarAReady(pcbSuspended);
		}
	}
}

//void execABlocked(t_pcb* procesoDevuelto) {
//	sacarDeExe(procesoDevuelto);
//	agregarABlocked(procesoDevuelto);
//
//	if (algoritmoPlanificacion == SRT){
//		procesoDevuelto->estimacion_rafaga_anterior = procesoDevuelto->estimacion_rafaga;
//		procesoDevuelto->estimacion_rafaga = obtener_proxima_estimacion(procesoDevuelto);
//	}
//
//	crear_hilo_chequeo_tiempo_maximo((void*) procesoDevuelto);
//	sem_post(&semProcesoEsperandoDispositvoIO);
//}

void crear_hilo_chequeo_tiempo_maximo(void* punteroPcb) {
	pthread_t hiloChequeoTiempoMaximo;
	pthread_create(&hiloChequeoTiempoMaximo, NULL, (void*) procesar_chequeo_tiempo_maximo, punteroPcb);
	pthread_detach(hiloChequeoTiempoMaximo);
}

void procesar_chequeo_tiempo_maximo(void* punteroAlPcb) {
	t_pcb* pcb = (t_pcb*) punteroAlPcb;

	//Lo bloqueamos el tiempo maximo que puede estar bloqueado
	usleep(tiempo_maximo_bloqueado*1000);

	if(pcb->estado == BLOCKED){
		agregarABlockSuspended(pcb);
	}
}

//Lo llamo en execABlocked con el sem_post
void hilo_paralelo_atencion_IO() {
	log_info(logger, "[============ HILO #1: PROCESAMIENTO BLOQUEO ===========]\n");
	while(1) {
		sem_wait(&semProcesoEsperandoDispositvoIO);

		t_pcb* pcb_IO = obtener_primero_de_blocked(); // TODO: ACA ESTA EL ERROR

		log_info(logger, "   > [PID #%03d] ENTRA A COLA [I/O] (%05dms)", pcb_IO->pid, pcb_IO->tiempo_bloqueo);

		usleep((pcb_IO->tiempo_bloqueo)*1000);

		//log_info(logger,"[I/O] El proceso de pid: %d termino su entrada salida de: %d segundos",pcb_IO->pid,(pcb_IO->tiempo_bloqueo/1000));

		if(pcb_IO->estado == BLOCKED) {
			sacarDeBlock(pcb_IO);
			agregarAReady(pcb_IO);
		} else if(pcb_IO->estado == SUSPENDED_BLOCKED) {
			sacarDeBlockSuspended(pcb_IO);
		}
	}

}

t_pcb* obtener_primero_de_blocked() {
	pthread_mutex_lock(&mutexBlock);
	t_pcb* siguientePcb = list_remove(listaBlock, 0);

	//imprimir_pcb_incompleto(logger,siguientePcb);
	pthread_mutex_unlock(&mutexBlock);

	return siguientePcb;
}

int64_t obtener_proxima_estimacion(t_pcb* pcb) {
	int64_t tiempo_ejecutado = ((pcb->tiempo_salio_de_ejecutar) - (pcb->tiempo_entro_a_ejecutar))*1000;
	return (alfa * tiempo_ejecutado) + ((1 - alfa) * pcb->estimacion_rafaga_anterior);
}

/*uint32_t tiempoEjecutado(t_pcb* pcb){
	uint32_t tiempo_ejecutado;
	return  tiempo_ejecutado = (pcb->tiempo_salio_de_ejecutar) - (pcb->tiempo_entro_a_ejecutar);
}*/

/* ALGORITMOS */

// usar "liberar_pcb(t_pcb* pcb)" CUANDO HAGAMOS EL EXIT ASI NO TENEMOS QUILOMBO CON VALGRIND

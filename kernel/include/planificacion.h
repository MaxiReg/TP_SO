#ifndef INCLUDE_PLANIFICACION_H_
#define INCLUDE_PLANIFICACION_H_
	/* ******* BIBLIOTECA ******* */
	// ESTANDAR
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>

	#include <sockets/config-log.h>
	#include <kernel.h>
	#include <semaphore.h>
	#include <pthread.h>

	// COMMONS
	#include <commons/log.h>
	#include <commons/config.h>

	//DECLARACION DE SEMAFOROS
	sem_t multiprogramacion;
	sem_t largoPlazo;
	sem_t medianoPlazo;
	sem_t cortoPlazo;
	sem_t contadorReady;
	sem_t sendInstrucion;
	sem_t contadorExe;
	sem_t contadorBlock;
	sem_t contadorReadySuspended;
	sem_t semProcesoEsperandoDispositvoIO;
	sem_t gradoMultiProcesamiento;
	sem_t entroReady;

	typedef enum{
        FIFO,
        SRT
    } t_algoritmo_planificacion;

    // FUNCIONES VARIAS
	t_algoritmo_planificacion obtener_algoritmo();
	t_algoritmo_planificacion algoritmoPlanificacion;
	t_pcb* obtenerSiguienteDeReady();

	// TRANSICIONES
	void hiloNew_Ready();
	void hiloReady_Exe();
	void execABlocked();
	void hilo_suspendedAReady();
	void hilo_paralelo_atencion_IO();
	void enviar_interrupcion();

	//uint32_t tiempoEjecutado(t_pcb* pcb);
#endif /* INCLUDE_PLANIFICACION_H_ */

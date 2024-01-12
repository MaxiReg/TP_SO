#ifndef INCLUDE_KERNELH
#define INCLUDE_KERNELH

 	/* ******* BIBLIOTECA ******* */
	// ESTÁNDAR
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>

	// MISCELÁNEO
	#include <netdb.h>
	#include <signal.h>
	#include <sys/socket.h>
	#include <unistd.h>

	// COMMONS
	#include <commons/log.h>
	#include <commons/config.h>
	#include <commons/collections/list.h>
	#include <commons/collections/queue.h>
	#include <semaphore.h>
	#include<commons/string.h>

	// COMPARTIDOS
	#include <serializacion/serializacion.h>
 	#include <serializacion/serializacion-instruccion.h>
	#include <serializacion/serializacion-pcb.h>
	#include <sockets/config-log.h>
	#include <planificacion.h>
	#include <comunicacionCPU.h>

	#include <pthread.h>

	//TIEMPO
	#include <sys/time.h>

	typedef struct {
        t_log* log;
        int socket_cliente;
        char* server_name;
    } t_procesar_conexion_args;

    t_procesar_conexion_args* args;

    //ACAAAAAAAAAAAAAAAAAAAAA
    int pidKernel;

    typedef struct{
    	char* nombre;
    	int valor;
    	t_queue* listaDeEspera;
    	pthread_mutex_t mutexSemaforo;
    }t_semaforo;



    void iterador_instruccion(t_instruccion* instruccion);

/* GLOBALES */

    t_pcb* procesoAEjecutar;
    t_list* vectorSocketsConsolas;

/* Variable de Config */
    int socket_memoria;

    t_log* logger;
    t_config* config;
    char* ip_Memoria;
    char* puerto_Memoria;
    char* ip_cpu;
    char* puerto_cpu_interrupt;
    char* puerto_cpu_dispatch;
    char* puerto_escucha;
    float alfa;
    int estimacion_inicial;
    int grado_multiprogramacion;
    int tiempo_maximo_bloqueado;

/* Listas - Colas */
    t_list* listaSemaforos;
    t_queue* colaNew;
    t_list* listaReady;
    t_list* listaExe;
    t_list* listaBlock;
    t_list* listaExit;
    t_list* listaBlockSuspended;
    t_queue* colaReadySuspended;
    t_list* archivos_creados;

/* MUTEX */

    pthread_mutex_t mutexNew;
    pthread_mutex_t mutexReady;
    pthread_mutex_t mutexBlock;
    pthread_mutex_t mutexExe;
    pthread_mutex_t mutexExit;
    pthread_mutex_t mutexBlockSuspended;
    pthread_mutex_t mutexReadySuspended;
    pthread_mutex_t mutexVectorConsolas;
    pthread_mutex_t semaforoPcbAEjecutar;
    pthread_mutex_t gradomulti;

// HILOS

    pthread_t hilo_Dispatch;
    pthread_t hilo_Interrup;

    pthread_t hiloNewReady;
    pthread_t hiloReady_Exec;

    /**/

    pthread_t hiloSuspendedAReady;
    pthread_t hiloAtencionIO;

/* Tareas Iniciales */
    void iniciar_configuracion();
    void iniciar_listas();
    void iniciar_semaforos();
    void iniciar_planificacion();

    void destruir_listas();
    void destruir_semaforos();


// COMUNICACION

    int server_escuchar(char* server_name, int server_socket);
    void procesar_conexion(void* void_args,t_pcb* pcb);

// FUNCIONES

    int aceptar_cliente(int server_socket);
    int iniciar_servidor(char* ip, char* puerto_escucha);
    int generar_conexion();

    void liberarListaDeSemaforos();
    void destruirListaYElementos(t_list* unaLista);
    void destruirColaYElementos(t_queue* unaCola);

    void agregarANew(t_pcb* pcb);
    t_pcb* sacarDeNew();

    void agregarAReady(t_pcb* pcbReady);
    void sacarDeExe(t_pcb* pcb);

    void agregarAReadySuspended(t_pcb* pcbReadySuspended);
    t_pcb* sacarDeReadySuspended();

    void agregarABlocked(t_pcb* pcb_blocked);
    void sacarDeBlock(t_pcb* pcb);

    void agregarABlockSuspended(t_pcb* pcbSuspended);
    void sacarDeBlockSuspended(t_pcb* pcbBlockedSuspended);

    void execABlocked(t_pcb* procesoDevuelto);

    void crear_hilo_chequeo_tiempo_maximo(void* pcb);
    void procesar_chequeo_tiempo_maximo(void* pcb);
    void crear_hilo_pcb_suspended_blocked(t_pcb* pcb);

    t_pcb* procesos_en_ready();
    bool replanificar();
    t_pcb* obtener_primero_de_blocked();
    int64_t obtener_proxima_estimacion(t_pcb* pcb);



// ESTIMACION





#endif /* MAINH */

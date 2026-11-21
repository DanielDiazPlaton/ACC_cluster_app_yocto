//Ira el middelware
#include "middleware.h"
#include "can_bus.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>



void *Ostask_app(void *arg)
{
    char *input_arg = (char *) arg;

    while(1)
    {
    printf("\nAqui se ejecutan las funcionalidades del cluster: %s\n",input_arg);
    sleep(1);
    }

    return NULL;
}

void *Ostask_comm(void *arg)
{
    char *input_arg = (char *)arg;
    uint32_t test_can_id = 0x123;
    int can_fd = -1;

    printf("[BSW-COMM] Iniciando hilo: %s\n", input_arg);

    // Espera activa hasta que systemd-networkd levante can0
    while ((can_fd = can_bus_init("can0")) < 0)
    {
        fprintf(stderr, "[BSW-COMM] Esperando interfaz can0...\n");
        sleep(2);
    }

    // Bucle infinito: transmite tramas CAN cada 500 ms sin detenerse nunca
    while (1)
    {
        can_bus_send_test_frame(can_fd, test_can_id);
        usleep(500000);
    }

    // Inalcanzable en ejecución normal; satisface la firma void* de pthread
    can_bus_close(can_fd);
    return NULL;
}


void Ostask_create()
{
    pthread_t app_thread;
    static char *pApp = "Funciones";

    pthread_t CAN_thread;
    static char *pCan = "Comunicacion CAN";

    if (pthread_create(&app_thread, NULL, Ostask_app, (void *)pApp) != 0)
    {
        perror("[BSW] Error creando Ostask_app");
        exit(EXIT_FAILURE);
    }

    if (pthread_create(&CAN_thread, NULL, Ostask_comm, (void *)pCan) != 0)
    {
        perror("[BSW] Error creando Ostask_comm");
        exit(EXIT_FAILURE);
    }

    // Bloquea el hilo main() indefinidamente mientras ambos hilos corren
    pthread_join(app_thread, NULL);
    pthread_join(CAN_thread, NULL);
}





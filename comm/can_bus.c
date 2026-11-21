#include "can_bus.h"

#include "can_bus.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>

int can_bus_init(const char *ifname) {
    int s;
    struct sockaddr_can addr;
    struct ifreq ifr;

    // 1. Crear el socket RAW para protocolo CAN
    if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
        perror("[CAN] Error al crear socket");
        return -1;
    }

    // 2. Obtener el índice numérico de la interfaz (ej. can0)
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, ifname, IFNAMSIZ - 1);
    if (ioctl(s, SIOCGIFINDEX, &ifr) < 0) {
        perror("[CAN] Error en ioctl (¿Está la interfaz can0 habilitada?)");
        close(s);
        return -1;
    }

    // 3. Vincular (bind) el socket a la interfaz de red CAN
    memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("[CAN] Error en bind");
        close(s);
        return -1;
    }

    printf("[CAN] Interfaz %s inicializada correctamente (Socket FD: %d)\n", ifname, s);
    return s;
}

int can_bus_send_test_frame(int socket_fd, uint32_t can_id) {
    struct can_frame frame;
    static uint8_t counter = 0;

    if (socket_fd < 0) {
        fprintf(stderr, "[CAN] Socket inválido para transmisión.\n");
        return -1;
    }

    // Estructurar la trama CAN
    frame.can_id = can_id;
    frame.can_dlc = 8; // 8 bytes de payload

    // Carga util (Payload) de prueba: Fácilmente identificable en analizador
    frame.data[0] = 0xAA;        // Byte de sincronía / firma
    frame.data[1] = 0x55;
    frame.data[2] = counter++;   // Contador incremental de tramas
    frame.data[3] = 0x01;        // Estado de prueba (ej. Key-ON)
    frame.data[4] = 0x00;
    frame.data[5] = 0xDE;        // Mnemónico 0xDEADBEEF para depuración
    frame.data[6] = 0xAD;
    frame.data[7] = 0xBE;

    // Enviar trama
    ssize_t nbytes = write(socket_fd, &frame, sizeof(struct can_frame));

    if (nbytes < 0) {
        perror("[CAN] Error al transmitir trama");
        return -1;
    }

    if (nbytes < (ssize_t)sizeof(struct can_frame)) {
        fprintf(stderr, "[CAN] Error: Trama enviada de forma incompleta.\n");
        return -1;
    }

    printf("[CAN] Trama enviada -> ID: 0x%03X | DLC: %d | Data: %02X %02X %02X %02X %02X %02X %02X %02X\n",
           frame.can_id, frame.can_dlc,
           frame.data[0], frame.data[1], frame.data[2], frame.data[3],
           frame.data[4], frame.data[5], frame.data[6], frame.data[7]);

    return (int)nbytes;
}

void can_bus_close(int socket_fd) {
    if (socket_fd >= 0) {
        close(socket_fd);
        printf("[CAN] Socket cerrado.\n");
    }
}







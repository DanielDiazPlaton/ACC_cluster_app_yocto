//El header de can_bus.c
//aqui estara el header den functionality.c
#ifndef CAN_BUS_H
#define CAN_BUS_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Inicializa la interfaz SocketCAN (ej. "can0").
 * @param ifname Nombre de la interfaz CAN en Linux ("can0", "vcan0", etc.)
 * @return Descriptor del socket (fd > 0) si tiene éxito, o -1 en caso de error.
 */
int can_bus_init(const char *ifname);

/**
 * @brief Envía una trama de prueba con datos simulados.
 * @param socket_fd Socket retornado por can_bus_init.
 * @param can_id Identificador estándar o extendido (ej. 0x123).
 * @return Número de bytes transmitidos o -1 si hubo error.
 */
int can_bus_send_test_frame(int socket_fd, uint32_t can_id);

/**
 * @brief Cierra el socket CAN activado.
 * @param socket_fd Descriptor del socket.
 */
void can_bus_close(int socket_fd);

#endif // CAN_BUS.H
#ifndef _TLS_H_
#define _TLS_H_

#include <libserialport.h>
#include <string.h>

// Function to get a list of connected ESP devices
int get_esp_devices(struct sp_port ***port_list);

// Function to send command to ESP via serial
int send_esp_command(const char *port_name, const char *command);

#endif

#ifndef _SONALKUNJ_WIFI_H_
#define _SONALKUNJ_WIFI_H_

#include "SonalKunj_uart.h"

void wifiInit(void);
void wifiUpdateDatabase(unsigned char temp, unsigned char humid, float co2);
unsigned char wifiGetCommand(void);

#endif
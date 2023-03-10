#include "SonalKunj_wifi.h"

/*
 * This function start the wifi connection.
 */
void wifiInit(void)
{
	uart_2_Init();
	uart_2_Transmit(0x01);
}

void wifiUpdateDatabase(unsigned char temp, unsigned char humid, float co2)
{
	uart_2_Transmit(0x02);
	uart_2_Transmit(temp/10 + '0');
	uart_2_Transmit(temp%10 + '0');
	uart_2_Transmit(humid/10 + '0');
	uart_2_Transmit(humid%10 + '0');
	uart_2_Transmit(co2/10 + '0');
	uart_2_Transmit('.');
	uart_2_Transmit((unsigned char)co2%10 + '0');
	uart_2_Transmit(0x03);
}

unsigned char wifiGetCommand(void)
{
	return uart_2_Receive();
}
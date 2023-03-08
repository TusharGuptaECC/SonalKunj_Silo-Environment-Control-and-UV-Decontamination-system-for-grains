#define ONBOARD_LEDS_H
#define NEXTION_H
#define DELAY_H
#define ONBOARD_SWITCH_H
#define ADC_H
#define PWM_H
#define WATCHDOG_H
#define SYSTICK_H
#define SLEEP_H
#define WIFI_H

#include "configuration.h"

// temp , humid, co2_conc, uv time
float settings[4] = {16,50,0.5,30};

#define PIOSC 0	/*Precision internal oscillator*/
#define MOSC 1	/*External main oscillator*/
#define clk_portb 1
#define dht_pin 2	// portf
#define heating_unit_pin 6 // port a
#define fan_pin 7 // port a
#define UV_pin 5 // port e

//---------------------------------------------------------------
#define AUTOMATIC_MODE 0
#define MANUAL_MODE 1
#define WIFI_CONTROL_MODE 2
//---------------------------------------------------------------

volatile uint32_t present_time = 0;
volatile uint32_t past_time = 0;
uint32_t dht_data;
volatile int dht_count = 0;
volatile float co2_data = 0;
/*
 * system_state = 0 -> MANUAL MODE
 * system_state = 1 -> AUTOMATIC MODE
 * system_state = 2 -> WIFI_CONTROL MODE
 */
volatile unsigned char system_state = 1;//AUTOMATIC_MODE;

void clockInit(unsigned char clk_src);
void dhtInit(void);
void dhtReadData(void);
void filoLidOpen(void);
void filoLidClose(void);
void heatingUnitInit(void);
void fanInit(void);
void UVInit(void);
void heatingUnitCtrl(unsigned char status);
void fanCtrl(unsigned char status);
void UVCtrl(unsigned char status);
unsigned char idealTemp(unsigned char dht_temp_data);
unsigned char idealHumid(unsigned char dht_humid_data);
unsigned char idealCO2(float co2_conc);
void prepareForSleep(void);
void automaticMode(void);
void manualMode(void);
void wifiControlMode(void);

void TIMER1A_Handler(void)
{
	//led(red_led, ON);
	if (TIMER1->MIS & (1 << 2))
	{
		TIMER1->ICR |= (1 << 2);
		past_time = present_time;
		present_time = ((TIMER1->TAR & 0x0000FFFF));
		dht_count = dht_count + 1;
	}
}

void ADC0SS0_Handler(void)
{
	if (ADC0->ISC & (1 << 0))
	{
		ADC0->ISC |= (1 << 0);
		co2_data = ((8*(ADC0->SSFIFO0 & 0x00000FFF))/10000.00);
	}
}

void WTIMER5A_Handler(void)
{
	if (WTIMER5->MIS & (1 << 0))
	{
		//led(green_led, ON);
		//system_state = 1;
		heatingUnitCtrl(OFF);
		fanCtrl(OFF);
		WTIMER5->ICR |= (1 << 0);
	}
}

void GPIOF_Handler(void)
{
	if (GPIOF->MIS & (1 << 4))
	{
		system_state = 1;
		GPIOF->ICR |= (1 << 4);
	}
}

void SysTick_Handler(void)
{
	if (SysTick->CTRL & (1 << 16))
  {
    system_state = 1;
    SysTick->CTRL = 0;  // disable the timer
  }
}

int main(void)
{
	//unsigned char response;
	clockInit(MOSC);
	//gpioPinInit(GPIO, OUTPUT, blue_led);
	gpioPinInit(GPIO, OUTPUT, green_led);
	gpioPinInit(GPIO, OUTPUT, red_led);
	nextionInit();
	heatingUnitInit();
	fanInit();
	UVInit();
	//initWatchDog1();
	wifiInit();
	
	while (1)
	{
		if (system_state == AUTOMATIC_MODE)
		{
			automaticMode();
		}
		else if (system_state == MANUAL_MODE)
		{
			manualMode();
		}
		else
		{
			wifiControlMode();
		}
	}
	return 0;
}

void clockInit(unsigned char clk_src)
{
	// RCC default value is 0x078E.3AD1
	uint32_t i;
	if (clk_src == MOSC)
	{
		SYSCTL->RCC = 0x000E3D40;
	}
	if (clk_src == PIOSC)
	{
		SYSCTL->RCC = 0x078E3AD1;
	}
  for (i = 0; i < 10000; i++);
	RCGCGPIO_Reg |= ((1 << clk_portf) | (1 << clk_portb) | CLOCK_TO_GPIO_E);  // clock for PORTF
	// SYSCTL->RCGC0 |= (1 << clk_portf);	// legacy register not used here.
	SystemCoreClockUpdate();
}

void dhtInit(void)
{
	gpioPinInit(GPIO, OUTPUT, dht_pin);
	led(dht_pin, ON);
	delayMS(1000);
	led(dht_pin, OFF);
	delayMS(25);
	led(dht_pin, ON);
	delayUS(5);
	led(dht_pin, OFF);
	gpioPinInit(GPIO, INPUT, dht_pin);
	inputCaptureInit(dht_pin);
}

void dhtReadData(void)
{
	while(dht_count < 31)
	{
		if(dht_count < 3)
		{
			//led(green_led, ON);
			// do nothing
			{}	// avoid compiler optimization
		}
		else if (present_time - past_time > 1600)//past_time - present_time > 1600 ) // 100us
		{
			dht_data |= (1 << (31 - dht_count + 3));
		}
		else
		{
			dht_data &= ~(1 << (31 - dht_count + 3));
		}
		
	}
	while(dht_count < 42)
	{
		{}// do nothing
		
	}
	TIMER1->CTL &= ~(1 << TAEN_bit); // off the timer
	// reset the timer
	resetTimer1();
}

void filoLidOpen(void)
{
	pwmBegin(20, 6);
	delayMS(40);
	pwmStop();
	delayMS(40);
}
	
void filoLidClose(void)
{
	pwmBegin(20, 13);
	delayMS(40);
	pwmStop();
	delayMS(40);
}	

// heating unit is connected to PA6
void heatingUnitInit(void)
{
	SYSCTL->RCGCGPIO |= (1 << 0);
	GPIOA->AFSEL &= ~(1 << heating_unit_pin);
	GPIOA->DR2R |= (1 << heating_unit_pin);
	GPIOA->DIR |= (1 << heating_unit_pin);
	GPIOA->DEN |= (1 << heating_unit_pin);
}

void fanInit(void)
{
	SYSCTL->RCGCGPIO |= (1 << 0);
	GPIOA->AFSEL &= ~(1 << fan_pin);
	GPIOA->DR2R |= (1 << fan_pin);
	GPIOA->DIR |= (1 << fan_pin);
	GPIOA->DEN |= (1 << fan_pin);
}

void UVInit(void)
{
	SYSCTL->RCGCGPIO |= (1 << 4);
	GPIOE->AFSEL &= ~(1 << UV_pin);
	GPIOE->DR2R |= (1 << UV_pin);
	GPIOE->DIR |= (1 << UV_pin);
	GPIOE->DEN |= (1 << UV_pin);
}

void heatingUnitCtrl(unsigned char status)
{
	if (status)
  {
		GPIOA->DATA |= (1 <<  heating_unit_pin);
	}
	else
	{
		GPIOA->DATA &= ~(1 << heating_unit_pin);
	}
}

void fanCtrl(unsigned char status)
{
	if (status)
  {
		GPIOA->DATA |= (1 <<  fan_pin);
	}
	else
	{
		GPIOA->DATA &= ~(1 << fan_pin);
	}
}

void UVCtrl(unsigned char status)
{
	if (status)
  {
		GPIOE->DATA |= (1 <<  UV_pin);
	}
	else
	{
		GPIOE->DATA &= ~(1 << UV_pin);
	}
}

unsigned char idealTemp(unsigned char dht_temp_data)
{
	if (dht_temp_data > settings[0])
	{
		return 2;
	}
	else if (dht_temp_data < settings[0])
	{
		return 1;
	}
	else 
	{
		return 0;
	}
}

unsigned char idealHumid(unsigned char dht_humid_data)
{
	if (dht_humid_data > settings[1])
	{
		return 2;
	}
	else if (dht_humid_data < settings[0])
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

unsigned char idealCO2(float co2_conc)
{
	if (co2_conc > settings[2])
	{
		return 2;
	}
	else if (co2_conc < settings[0])
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void prepareForSleep(void)
{
	// prepare timer for wake-up
	SYSCTL->RCGCWTIMER |= (1 << 5); 
	onBoardSwitchInit(switch_one, rising_edge_mode_switch);
	WTIMER5->CTL &= ~(1 << 0);// disable the timer
	WTIMER5->CFG = 0;//64 bit mode
	WTIMER5->TAMR |= 0x1;// one shot mode
	WTIMER5->TBILR = 0x2;
	WTIMER5->TAILR = 0x3C346000;// one minute timeout
	
	WTIMER5->IMR |= (1 << 0);// time-out
	NVIC_SetPriority(WTIMER5A_IRQn, 0);
	NVIC_EnableIRQ(WTIMER5A_IRQn);
	WTIMER5->CTL |= (1 << 0);
}

void automaticMode(void)
{
	led(red_led, ON);
	dhtInit();
	dhtReadData();
	adcInitAndReadData();
	if (idealTemp(((dht_data >> 8) & 0x000000FF)) == 1)// temp is less than ideal temp
	{
		heatingUnitCtrl(ON);
	}
	if (idealHumid(((dht_data >> 24) & 0x000000FF)) == 2)// humid is more than ideal value
	{
		fanCtrl(ON);
	}
	if (idealCO2(co2_data))
	{
		fanCtrl(ON);
		UVCtrl(ON);
		delayMS((uint32_t)settings[3]);
		UVCtrl(OFF);
	}
	prepareForSleep();
	goToSleep(SLEEP_WAKEUP_WFI, SLEEP_MODE_DEEP);
	led(red_led, OFF);
	wifiUpdateDatabase(((dht_data >> 24) & 0x000000FF), ((dht_data >> 8) & 0x000000FF), 2.5);//co2_data
}

void manualMode(void)
{
	unsigned char response = 0;
// led(green_led, ON);
// nextionInit();
	while (1)
	{
		response = nextionRead_1();
		if (response == 2)
		{
			led(green_led, OFF);
			dhtInit();
			dhtReadData();
			adcInitAndReadData();
			nextionSendSensorData(((dht_data >> 24) & 0x000000FF), ((dht_data >> 8) & 0x000000FF), 2.5);
		}
		else if (response == 3)
		{
			while (1)
			{
				response = nextionRead_1();
				if (response == 6)
				{
					heatingUnitCtrl(ON);		
				}
				if (response == 7)
				{
					heatingUnitCtrl(OFF);
				}
				if (response == 8)
				{
					fanCtrl(ON);
				}
				if (response == 9)
				{
					fanCtrl(OFF);
				}
				if (response == 10)
				{
					UVCtrl(ON);
				}
				if (response == 11)
				{
					UVCtrl(OFF);
				}
				if (response == 12)
				{
					filoLidOpen();
				}
				if (response == 13)
				{
					filoLidClose();
				}
				if (response == 0x15)
				{
					break;
				}
			}
		}
		else if (response == 4)
		{
			nextionDisplayIdealData();
			while (1)
			{
				response = nextionRead_2();
				if (response == 200)
				{
					saveSettingsFromNextion();
					//led(red_led, OFF);
					break;
				}
			}
		}
		else//if (nextionRead_1() == 0)
		{
			led(green_led, OFF);
		}
	}
}

void wifiControlMode(void)
{
	unsigned char response = 0;
// led(green_led, ON);
// nextionInit();
	while (1)
	{
		response = uart_2_Receive();
		if (response == 103)
		{
			led(green_led, OFF);
			dhtInit();
			dhtReadData();
			adcInitAndReadData();
			//nextionSendSensorData(((dht_data >> 24) & 0x000000FF), ((dht_data >> 8) & 0x000000FF), 2.5);
			wifiUpdateDatabase(((dht_data >> 24) & 0x000000FF), ((dht_data >> 8) & 0x000000FF), 2.5);
		}
		else if (response == 104)
		{
			while (1)
			{
				response = nextionRead_1();
				if (response == 6)
				{
					heatingUnitCtrl(ON);		
				}
				if (response == 7)
				{
					heatingUnitCtrl(OFF);
				}
				if (response == 8)
				{
					fanCtrl(ON);
				}
				if (response == 9)
				{
					fanCtrl(OFF);
				}
				if (response == 10)
				{
					UVCtrl(ON);
				}
				if (response == 11)
				{
					UVCtrl(OFF);
				}
				if (response == 12)
				{
					filoLidOpen();
				}
				if (response == 13)
				{
					filoLidClose();
				}
				if (response == 0x15)
				{
					break;
				}
			}
		}
		else if (response == 105)
		{
			nextionDisplayIdealData();
			while (1)
			{
				response = nextionRead_2();
				if (response == 200)
				{
					saveSettingsFromNextion();
					//led(red_led, OFF);
					break;
				}
			}
		}
		else//if (nextionRead_1() == 0)
		{
			led(green_led, OFF);
		}
	}
}

#ifndef	__CONFIG_H
#define	__CONFIG_H

#define MAIN_Fosc		24000000L	//定义主时钟
#include "STC8xxxx.H"
#include "I2C.h"
#include "adc.h"
#include "mode.h"
#include "ina219.h"
#include "usart.h"
#include "timer.h"
#include "gpio.h"

extern u32 GPIO_Value;
extern u8 WATER_Number;
void  delay_ms(u8 ms);

#endif
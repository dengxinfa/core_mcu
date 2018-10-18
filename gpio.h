#ifndef __GPIO_H_
#define __GPIO_H_
#include "config.h"

#define TEST_GPIO_UP 0
#define TEST_GPIO_DOWN 1
#define TEST_GPIO_WATER 2

void GPIO_Record_UP(void);
void GPIO_Record_DOWN(void);
void GPIO_Record_WATER(void);
void SET_GPIO_Value(void);
void GET_GPIO_Value(void);
void SET_GPIO_Next(void);
void SEND_Hex_Record(void);

#endif
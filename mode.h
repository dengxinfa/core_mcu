#ifndef _MODE_H_
#define _MODE_H_

#include	"config.h"

#define ADC_STATUS 0
#define ADC_END 1
#define RECV_CMD  2
#define VC_TEST 3
#define GPIO_TEST 4
#define GPIO_NEXT 5
#define GPIO_END 6


u8 get_mode(u8 mode);
void set_mode(u8 mode);
void reset_mode(u8 mode);

#endif

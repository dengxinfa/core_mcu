#ifndef	__ADC_H
#define	__ADC_H

#define CHN_V3_3 2
#define CHN_V2_5 3
#define CHN_V2_0 7

#include "config.h"

typedef struct{
	u16 V3_3;
	u16 V2_5;
	u16 V2_0;
}ADC_v;

void ENABLE_ADC_Convert(void);
void ADC_Channel_Select(u8 channel);
void ADC_Init(void);
u16 ADC_Filter(void);
void ADC_Send_Current(void);

#endif

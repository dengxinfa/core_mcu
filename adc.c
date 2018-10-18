#include "adc.h"

ADC_v ADC;
u16 ADC_Buff[6];

void ENABLE_ADC_Convert(void)
{
	ADC_CONTR |= 0x40;
}

void ADC_Channel_Select(u8 channel)
{
	ADC_CONTR |= channel;
}

void ADC_Init(void)
{
	P1M0 = 0x00;
	P1M1 = 0x8c;      //p1.2、p1.3、p1.7高阻输入
	
	ADCCFG = 0x0f;    //设置ADC时钟为系统时钟2/16/16
	ADC_CONTR = 0x80;//开启ADC转化电源
	EADC   = 1;       //是能ADC中断
}

void ADC_Voltage_To_ASCII(u16 voltage_data)
{
	u8 buff[7];
	buff[0] = voltage_data/1000 + '0';
	buff[1] = voltage_data%1000/100 + '0';
	buff[2] = voltage_data%100/10 + '0';
	buff[3] = voltage_data%10 + '0';
	buff[4] =  'm';
	buff[5] =  'V';
	buff[6] =  0;
	PrintString1(buff);
}

void ADC_Send_Current(void)
{
	PrintString1("3.3V_2.5V_2.0V_Voltage: ");
	ADC_Voltage_To_ASCII(ADC.V3_3);
	PrintString1(" ");
	ADC_Voltage_To_ASCII(ADC.V2_5);
	PrintString1(" ");
	ADC_Voltage_To_ASCII(ADC.V2_0);
	PrintString1("\r\n");
}

u16 ADC_Filter(void)    //中值滤波
{
	u8 i;
	u16 temp;
	for(i=1; i<6; i++)    //切换通道后第一次转换结果丢弃. 避免采样电容的残存电压影响.
		if(ADC_Buff[i] > ADC_Buff[i+1])
		{
			temp = ADC_Buff[i];
			ADC_Buff[i] = ADC_Buff[i+1];
			ADC_Buff[i+1] = temp;
		}			
	return ADC_Buff[3];
}

void ADC_Isr() interrupt 5 using 1
{
	static i = 0,j = 0;
	ADC_CONTR &= ~0x20;
		
	if(i++ >= 6)
	{
		if(++j == 1)     //读取3.3V
		{
			ADC.V3_3 = ADC_Filter();
			ADC_Channel_Select(CHN_V2_5);
		}
		else if(j == 2) //读取2.5V
		{
			ADC.V2_5 = ADC_Filter();
			ADC_Channel_Select(CHN_V2_0);
		}
		else            //读取2.0V
		{
			ADC.V2_0 = ADC_Filter();
			ADC_Channel_Select(CHN_V3_3);
			i = 0;
			j = 0;
			set_mode(ADC_STATUS);
			set_mode(ADC_END);
		}
	}
	else
		ADC_Buff[i] = ADC_RES * 256 + ADC_RESL;
		
	if(!get_mode(ADC_STATUS))
		ENABLE_ADC_Convert();
	else
		reset_mode(ADC_STATUS);
}
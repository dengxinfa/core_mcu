#include "gpio.h"

u32 GPIO_Value,GPIO_Record;
u8 WATER_Number;

void GPIO_Set_Read(void)
{
	P0M1  = 0xff;
	P0M0  = 0x00;
	P2M1  = 0xff;
	P2M0  = 0x00;
	P3M1 |= 0xfc;
	P0M0 &= 0x03;
	P4M1 |= 0x0e;
	P4M0 &= 0xf1;
	P5M1 |= 0x03;
	P5M0 &= 0xfc;
}

void GPIO_Set_Write(void)
{
	P0M1  = 0x00;
	P0M0  = 0xff;
	P2M1  = 0x00;
	P2M0  = 0xff;
	P3M1 &= 0x03;
	P0M0 |= 0xfc;
	P4M1 &= 0xf1;
	P4M0 |= 0x0e;
	P5M1 &= 0xfc;
	P5M0 |= 0x03;
}


void GET_GPIO_Value(void)
{
	GPIO_Set_Read();
	GPIO_Value = 0;
	GPIO_Value |= ((u32)P0);
	GPIO_Value |= ((u32)P2 << 8);
	GPIO_Value |= ((u32)(P3 >> 2) << 16);
	GPIO_Value |= ((u32)(P4 << 5) << 17);
	GPIO_Value |= ((u32)(P5 << 6) << 19);
}


void SET_GPIO_Value(void)
{
	GPIO_Set_Write();
	P0 = (u8)GPIO_Value;
	P2 = (u8)(GPIO_Value >> 8);
	P3 |= (u8)((GPIO_Value >> 16) & 0x3f);
	P4 |= (u8)((GPIO_Value >> 22) & 0x07);
	P5 |= (u8)(GPIO_Value >> 25);
}


void SET_GPIO_Next(void)
{
	PrintString1("next\r\n");
}

void GPIO_Record_UP(void)
{
	GPIO_Record |= ~GPIO_Value;
}

void GPIO_Record_DOWN(void)
{
	GPIO_Record |= GPIO_Value;
}

void GPIO_Record_WATER(void)
{
	GPIO_Record |= ((~GPIO_Value) & (1<<WATER_Number));	
	GPIO_Record |= (GPIO_Value & (~(1<<WATER_Number)));	
}

void SEND_Hex_Record(void)
{
	u8 i,j=0,buff[10];
	buff[0] = '0';
	buff[1] = 'x';
	for(i=2; i<10; i++)
	{
		j = (u8)(GPIO_Record >> ((i-2)*4));
		j |= 0x0f;
		if(j >=10)
			buff[i] = j - 10 + 'A';
		else
			buff[i] = j + '0';
	}
	PrintString1("GPIO RESULT: ");
	PrintString1(buff);
	PrintString1("\r\n");
}

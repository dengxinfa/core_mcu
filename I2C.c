
/*---------------------------------------------------------------------*/
/* --- STC MCU International Limited ----------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.GXWMCU.com --------------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* 如果要在程序中使用此代码,请在程序中注明使用了宏晶科技的资料及程序   */
/*---------------------------------------------------------------------*/


//编写 Coody  2005-4-12

#include	"I2C.h"

sbit	SDA	= P1^3;	//定义SDA  PIN5
sbit	SCL	= P1^4;	//定义SCL  PIN6
u8 flag;

/****************************/
void	I2C_Delay(void)	//for normal MCS51,	delay (2 * dly + 4) T, for STC12Cxxxx delay (4 * dly + 10) T
{
	u8	dly;
	dly = MAIN_Fosc / 2000000UL;		//按2us计算
	while(--dly)	;
}


/****************************/
void I2C_Start(void)               //start the I2C, SDA High-to-low when SCL is high
{
	SDA = 1;
	I2C_Delay();
	SCL = 1;
	I2C_Delay();
	SDA = 0;
	I2C_Delay();
	SCL = 0;
	I2C_Delay();
}		


void I2C_Stop(void)					//STOP the I2C, SDA Low-to-high when SCL is high
{
	SDA = 0;
	I2C_Delay();
	SCL = 1;
	I2C_Delay();
	SDA = 1;
	I2C_Delay();
}

void S_ACK(void)              //Send ACK (LOW)
{
	SDA = 0;
	I2C_Delay();
	SCL = 1;
	I2C_Delay();
	SCL = 0;
	I2C_Delay();
}

void S_NoACK(void)           //Send No ACK (High)
{
	SDA = 1;
	I2C_Delay();
	SCL = 1;
	I2C_Delay();
	SCL = 0;
	I2C_Delay();
}
		
void I2C_Check_ACK(void)         //Check ACK, If flag=0, then right, if flag=1, then error
{
	SDA = 1;
	I2C_Delay();
	SCL = 1;
	I2C_Delay();
	flag  = SDA;
	SCL = 0;
	I2C_Delay();
}

/****************************/
void I2C_WriteAbyte(u8 dat)		//write a byte to I2C
{
	u8 i;
	i = 8;
	do
	{
		if(dat & 0x80)	SDA = 1;
		else			SDA	= 0;
		dat <<= 1;
		I2C_Delay();
		SCL = 1;
		I2C_Delay();
		SCL = 0;
		I2C_Delay();
	}
	while(--i);
}

/****************************/
u8 I2C_ReadAbyte(void)			//read A byte from I2C
{
	u8 i,dat;
	i = 8;
	SDA = 1;
	do
	{
		SCL = 1;
		I2C_Delay();
		dat <<= 1;
		if(SDA)		dat++;
		SCL  = 0;
		I2C_Delay();
	}
	while(--i);
	return(dat);
}

/****************************/
void WriteNbyte(u8 device_addr,u8 addr, u8 *p, char number)			/*	WordAddress,First Data Address,Byte lenth	*/
                         									//flag=0,right, flag=1,error
{
	I2C_Start();
	I2C_WriteAbyte(device_addr);
	I2C_Check_ACK();
	if(!flag)
	{
		I2C_WriteAbyte(addr);
		I2C_Check_ACK();
		if(!flag)
		{
			do
			{
				I2C_WriteAbyte(*p);		p++;
				I2C_Check_ACK();
				if(flag)
					break;
			}
			while(--number);
		}
	}
	I2C_Stop();
}


/****************************/
void ReadNbyte(u8 device_addr, u8 addr, u8 *p, char number)				/*	WordAddress,First Data Address,Byte lenth	*/
                         									//flag=0,right, flag=1,error
{
	I2C_Start();
	I2C_WriteAbyte(device_addr);
	I2C_Check_ACK();
	if(!flag)
	{
		I2C_WriteAbyte(addr);
		I2C_Check_ACK();
		if(!flag)
		{
			I2C_Start();
			I2C_WriteAbyte(device_addr+1);
			I2C_Check_ACK();
			if(!flag)
			{
				do
				{
					*p = I2C_ReadAbyte();	p++;
					if(number != 1)		S_ACK();	//send ACK
				}
				while(--number);
				S_NoACK();			//send no ACK
			}
		}
	}
	I2C_Stop();
}


/****************************/
void ina219_Write_Register(u8 device_addr, u8 reg, u16 dat)
{
    unsigned char val[2];
    
    val[0] = (unsigned char)(dat >> 8);
    val[1] = (unsigned char)(dat & 0xFF);
    WriteNbyte(device_addr, reg, val, 2);
}

void ina219_Read_Register(u8 device_addr, u8 reg, s16 *dat)
{
    //printf("read reg == %d\r\n",reg);
  unsigned char val[2];
  
  ReadNbyte(device_addr, reg, val, 2);
  *dat = ((unsigned int)(val[0]) << 8) + val[1];
  
    //printf("data1 == %x\r\n",val[0]);
    //printf("data2 == %x\r\n",val[1]);
    
}


void WriteNbyte_Init(u8 device_addr,u8 addr, u8 *p, char number)			/*	WordAddress,First Data Address,Byte lenth	*/
                         									//flag=0,right, flag=1,error
{
	I2C_Start();
	I2C_WriteAbyte(device_addr);
	I2C_Check_ACK();
	if(!flag)
	{
		I2C_WriteAbyte(addr);
		I2C_Check_ACK();
		if(!flag)
		{
			do
			{
				I2C_WriteAbyte(*p);		p++;
				I2C_Check_ACK();
				if(flag)
					break;
			}
			while(--number);
		}
	}
	I2C_Stop();
}

void ina219_Write_Register_Init(u8 device_addr, u8 reg, u16 dat)
{
    unsigned char val[2];
    
    val[0] = (unsigned char)(dat >> 8);
    val[1] = (unsigned char)(dat & 0xFF);
    WriteNbyte_Init(device_addr, reg, val, 2);
}
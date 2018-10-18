
/*------------------------------------------------------------------*/
/* --- STC MCU International Limited -------------------------------*/
/* --- STC 1T Series MCU RC Demo -----------------------------------*/
/* --- Mobile: (86)13922805190 -------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ---------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ---------------------*/
/* --- Web: www.GXWMCU.com -----------------------------------------*/
/* --- QQ:  800003751 ----------------------------------------------*/
/* If you want to use the program or the program referenced in the  */
/* article, please specify in which data and procedures from STC    */
/*------------------------------------------------------------------*/

/*************	功能说明	**************

本文件为STC8xxx系列的定时器初始化和中断程序,用户可以在这个文件中修改自己需要的中断程序.


******************************************/


#include	"timer.h"

u16 time_count;

/********************* Timer0中断函数************************/
void timer0_int (void) interrupt TIMER0_VECTOR
{
	//如不是自动重装模式, 请自行添加装初值的代码
	if(time_count++ > 65535)
		time_count = 0;
}

/********************* Timer1中断函数************************/
void timer1_int (void) interrupt TIMER1_VECTOR
{
	//如不是自动重装模式, 请自行添加装初值的代码
	if(get_mode(ADC_END))
	{
		reset_mode(ADC_END);
		ADC_Send_Current();
		INA_Process();
		INA_Send_Current();
		reset_mode(VC_TEST);
	}
}

/********************* Timer2中断函数************************/
void timer2_int (void) interrupt TIMER2_VECTOR
{
	P22 = ~P22;
}

/********************* Timer3中断函数************************/
void timer3_int (void) interrupt TIMER3_VECTOR
{
	P23 = ~P23;
}

/********************* Timer4中断函数************************/
void timer4_int (void) interrupt TIMER4_VECTOR
{
	P24 = ~P24;
}

//========================================================================
// 函数: u8	Timer_Inilize(u8 TIM, TIM_InitTypeDef *TIMx)
// 描述: 定时器初始化程序.
// 参数: TIMx: 结构参数,请参考timer.h里的定义.
//       特别说明:	TIM_ValueMode=COUNTER时,   TIM_Value的值为对外计数的初值, 1T模式.
//					TIM_ValueMode=TIME_us时,   TIM_Value的值为定时us数, 实际装入时是65536减定时值, 自动判断12T或1T模式.
//					TIM_ValueMode=SYS_CLOCK时, TIM_Value的值为定时系统时钟数, 实际装入时是65536减定时值, 自动判断12T或1T模式.
// 返回: 0: 初始化成功,    1: 定时器序号过大错误,   2: 值过大错误.
// 版本: V1.0, 2018-3-22
//========================================================================
u8	Timer_Inilize(u8 TIM, TIM_InitTypeDef *TIMx)
{
	u8	div12;
	if(TIM > Timer4)	return 1;	//定时器序号过大错误

	if(TIMx->TIM_ValueMode == COUNTER)	//重装值为对外计数初值, 固定为1T计数模式
	{
		if(TIMx->TIM_Value >= 65536UL)	return 2;	//值过大, 返回错误
		div12 = 0;		//1T mode
	}
	else	//重装值为us或系统时钟数
	{
		if(TIMx->TIM_ValueMode == TIME_us)		//重装值为us
			TIMx->TIM_Value = (u32)(((float)MAIN_Fosc * (float)TIMx->TIM_Value)/1000000UL);	//重装的是时间(us), 计算所需要的系统时钟数.

		if(TIMx->TIM_Value >= (65536UL * 12))	return 2;	//值过大, 返回错误
		if(TIMx->TIM_Value < 65536UL)	div12 = 0;		//1T mode
		else
		{
			div12 = 1;	//12T mode
			TIMx->TIM_Value = TIMx->TIM_Value / 12;
		}
	}

	if(TIM == Timer0)
	{
		if(TIMx->TIM_Mode >  TIM_16BitAutoReloadNoMask)	return 3;	//模式错误
		TR0 = 0;	//停止计数
		ET0 = 0;	//禁止中断
		PT0 = 0;	//低优先级中断
		TMOD &= 0xf0;	//定时模式, 16位自动重装
		AUXR &= ~0x80;	//12T模式, 
		INT_CLKO &= ~0x01;	//不输出时钟
		if(TIMx->TIM_Interrupt == ENABLE)		ET0 = 1;	//允许中断
		if(TIMx->TIM_Polity == PolityHigh)		PT0 = 1;	//高优先级中断
		if(TIMx->TIM_ClkOut == ENABLE)	INT_CLKO |=  0x01;	//输出时钟
		TMOD |= TIMx->TIM_Mode;	//工作模式,0: 16位自动重装, 1: 16位定时/计数, 2: 8位自动重装, 3: 16位自动重装, 不可屏蔽中断
		if(TIMx->TIM_ValueMode == COUNTER)		//对外计数或分频
		{
			TMOD |=  0x04;	//对外计数或分频
			if((TIMx->TIM_Mode == TIM_8BitAutoReload) && (TIMx->TIM_Value >= 256))	return 2;	//值过大, 返回错误  8位重装模式
		}
		if(TIMx->TIM_Mode == TIM_8BitAutoReload)	//定时器8位重装模式
		{
			if((div12 == 1) || (TIMx->TIM_Value >= (256 * 12)))	return 2;	//12T模式, 值过大, 返回错误
			else if(TIMx->TIM_Value > 256)	TIMx->TIM_Value = TIMx->TIM_Value / 12;	//12T
			else AUXR |=  0x80;	//定时器, 1T模式
			if(TIMx->TIM_ValueMode != COUNTER)	TIMx->TIM_Value = 256 - TIMx->TIM_Value;	//定时模式
			TH0 = (u8)(TIMx->TIM_Value);
			TL0 = (u8)(TIMx->TIM_Value);
		}
		else	//定时器16位模式
		{
			if(div12 == 0)		AUXR |=  0x80;	//定时器, 1T模式	
			if(TIMx->TIM_ValueMode != COUNTER)	TIMx->TIM_Value = 65536UL - TIMx->TIM_Value;	//定时模式
			TH0 = (u8)(TIMx->TIM_Value >> 8);
			TL0 = (u8)(TIMx->TIM_Value);
		}
		if(TIMx->TIM_Run == ENABLE)	TR0 = 1;	//开始运行
		return	0;		//成功
	}

	if(TIM == Timer1)
	{
		if(TIMx->TIM_Mode >= TIM_16BitAutoReloadNoMask)	return 2;	//错误
		TR1 = 0;	//停止计数
		ET1 = 0;	//禁止中断
		PT1 = 0;	//低优先级中断
		TMOD &=  0x0f;	//定时模式, 16位自动重装
		AUXR &= ~0x40;	//12T模式, 
		INT_CLKO &= ~0x02;	//不输出时钟
		if(TIMx->TIM_Interrupt == ENABLE)		ET1 = 1;	//允许中断
		if(TIMx->TIM_Polity == PolityHigh)		PT1 = 1;	//高优先级中断
		if(TIMx->TIM_ClkOut == ENABLE)	INT_CLKO |=  0x02;	//输出时钟
		TMOD |= (TIMx->TIM_Mode << 4);	//工作模式,0: 16位自动重装, 1: 16位定时/计数, 2: 8位自动重装
		if(TIMx->TIM_ValueMode == COUNTER)		//对外计数或分频
		{
			TMOD |=  0x40;	//对外计数或分频
			if((TIMx->TIM_Mode == TIM_8BitAutoReload) && (TIMx->TIM_Value >= 256))	return 2;	//值过大, 返回错误  8位重装模式
		}
		if(TIMx->TIM_Mode == TIM_8BitAutoReload)	//定时器8位重装模式
		{
			if((div12 == 1) || (TIMx->TIM_Value >= (256 * 12)))	return 2;	//12T模式, 值过大, 返回错误
			else if(TIMx->TIM_Value > 256)	TIMx->TIM_Value = TIMx->TIM_Value / 12;	//12T
			else AUXR |=  0x40;	//定时器, 1T模式
			if(TIMx->TIM_ValueMode != COUNTER)	TIMx->TIM_Value = 256 - TIMx->TIM_Value;	//定时模式
			TH1 = (u8)(TIMx->TIM_Value);
			TL1 = (u8)(TIMx->TIM_Value);
		}
		else	//定时器16位模式
		{
			if(div12 == 0)		AUXR |=  0x40;	//定时器, 1T模式	
			if(TIMx->TIM_ValueMode != COUNTER)	TIMx->TIM_Value = 65536UL - TIMx->TIM_Value;	//定时模式
			TH1 = (u8)(TIMx->TIM_Value >> 8);
			TL1 = (u8)(TIMx->TIM_Value);
		}

		if(TIMx->TIM_Run == ENABLE)	TR1 = 1;	//开始运行
		return	0;		//成功
	}

	if(TIM == Timer2)		//Timer2,固定为16位自动重装, 中断无优先级
	{
		AUXR &= ~0x1c;		//停止计数, 定时模式, 12T模式
		IE2  &= ~(1<<2);	//禁止中断
		INT_CLKO &= ~0x04;	//不输出时钟
		if(TIMx->TIM_Interrupt == ENABLE)	IE2  |=  (1<<2);	//允许中断
		if(TIMx->TIM_ClkOut == ENABLE)	INT_CLKO |=  0x04;		//输出时钟
		if(TIMx->TIM_ValueMode == COUNTER)	AUXR |=  (1<<3);	//对外计数或分频
		if(div12 == 0)						AUXR |=  (1<<2);	//1T模式
		if(TIMx->TIM_ValueMode != COUNTER)	TIMx->TIM_Value = 65536UL - TIMx->TIM_Value;	//定时模式
		TH2 = (u8)(TIMx->TIM_Value >> 8);
		TL2 = (u8)(TIMx->TIM_Value);
		if(TIMx->TIM_Run == ENABLE)	AUXR |=  (1<<4);	//开始运行
		return	0;		//成功
	}

	if(TIM == Timer3)		//Timer3,固定为16位自动重装, 中断无优先级
	{
		T4T3M &= 0xf0;		//停止计数, 定时模式, 12T模式, 不输出时钟
		IE2  &= ~(1<<5);	//禁止中断
		if(TIMx->TIM_Interrupt == ENABLE)	IE2   |=  (1<<5);	//允许中断
		if(TIMx->TIM_ClkOut == ENABLE)		T4T3M |=  1;		//输出时钟
		if(TIMx->TIM_ValueMode == COUNTER)	T4T3M |=  (1<<2);	//对外计数或分频
		if(div12 == 0)						T4T3M |=  (1<<1);	//1T模式
		if(TIMx->TIM_ValueMode != COUNTER)	TIMx->TIM_Value = 65536UL - TIMx->TIM_Value;	//定时模式
		TH3 = (u8)(TIMx->TIM_Value >> 8);
		TL3 = (u8)(TIMx->TIM_Value);
		if(TIMx->TIM_Run == ENABLE)	T4T3M |=  (1<<3);	//开始运行
		return	0;		//成功
	}

	if(TIM == Timer4)		//Timer4,固定为16位自动重装, 中断无优先级
	{
		T4T3M &= 0x0f;		//停止计数, 定时模式, 12T模式, 不输出时钟
		IE2  &= ~(1<<6);	//禁止中断
		if(TIMx->TIM_Interrupt == ENABLE)	IE2   |=  (1<<6);	//允许中断
		if(TIMx->TIM_ClkOut == ENABLE)		T4T3M |=  (1<<4);	//输出时钟
		if(TIMx->TIM_ValueMode == COUNTER)	T4T3M |=  (1<<6);	//对外计数或分频
		if(div12 == 0)						T4T3M |=  (1<<5);	//1T模式
		if(TIMx->TIM_ValueMode != COUNTER)	TIMx->TIM_Value = 65536UL - TIMx->TIM_Value;	//定时模式
		TH4 = (u8)(TIMx->TIM_Value >> 8);
		TL4 = (u8)(TIMx->TIM_Value);
		if(TIMx->TIM_Run == ENABLE)	T4T3M |=  (1<<7);	//开始运行
		return	0;		//成功
	}

	return 2;	//错误
}


u16 get_time(void)
{
	return time_count;
}

u8 timeout_nms(u16 timeout, u16 starttime)
{
	if(time_count < starttime)
		if((time_count + 65536 - starttime) > timeout)
			return 1;
		else
			return 0;
	else
		if((time_count - starttime) > timeout)
			return 1;
		else
			return 0;
}

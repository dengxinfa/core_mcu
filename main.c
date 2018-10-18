#include "config.h"

void  delay_ms(u8 ms)
{
	u16 i;
	do
	{
		i = MAIN_Fosc / 10000;
		while(--i)	;
	}while(--ms);
}

void TIM_config(void)
{
	TIM_InitTypeDef		TIM_InitStructure;					//结构定义
	TIM_InitStructure.TIM_Mode      = TIM_16BitAutoReload;	//指定工作模式,   TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
	TIM_InitStructure.TIM_Polity    = PolityLow;			//指定中断优先级, PolityHigh,PolityLow
	TIM_InitStructure.TIM_Interrupt = ENABLE;				//中断是否允许,   ENABLE或DISABLE
	TIM_InitStructure.TIM_ClkOut    = ENABLE;				//是否输出高速脉冲, ENABLE或DISABLE
	TIM_InitStructure.TIM_ValueMode = SYS_CLOCK;			//初值模式, TIME_us: 重装值为时间, 单位为us.   SYS_CLOCK: 重装值为系统时钟数.	COUNTER: 对外计数初值(固定为1T模式).
	TIM_InitStructure.TIM_Value     = MAIN_Fosc / 1000;	//重装值或初值,
	TIM_InitStructure.TIM_Run       = ENABLE;				//是否初始化后启动定时器, ENABLE或DISABLE
	Timer_Inilize(Timer0,&TIM_InitStructure);				//初始化Timer0	  Timer0,Timer1,Timer2,Timer3,Timer4	
	Timer_Inilize(Timer1,&TIM_InitStructure);
}

void	UART_config(void)
{
	COMx_InitDefine		COMx_InitStructure;					//结构定义
	COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;		//模式,       UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
	COMx_InitStructure.UART_BRT_Use   = BRT_Timer2;			//使用波特率,   BRT_Timer1, BRT_Timer2 (注意: 串口2固定使用BRT_Timer2)
	COMx_InitStructure.UART_BaudRate  = 115200ul;			//波特率, 一般 110 ~ 115200
	COMx_InitStructure.UART_RxEnable  = ENABLE;				//接收允许,   ENABLE或DISABLE
	COMx_InitStructure.BaudRateDouble = DISABLE;			//波特率加倍, ENABLE或DISABLE
	COMx_InitStructure.UART_Interrupt = ENABLE;				//中断允许,   ENABLE或DISABLE
	COMx_InitStructure.UART_Polity    = PolityLow;			//中断优先级, PolityLow,PolityHigh
	COMx_InitStructure.UART_P_SW      = UART1_SW_P30_P31;	//切换端口,   UART1_SW_P30_P31,UART1_SW_P36_P37,UART1_SW_P16_P17(必须使用内部时钟)
	COMx_InitStructure.UART_RXD_TXD_Short = DISABLE;		//内部短路RXD与TXD, 做中继, ENABLE,DISABLE
	USART_Configuration(USART1, &COMx_InitStructure);		//初始化串口1 USART1,USART2

	COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;		//模式,       UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
	COMx_InitStructure.UART_BaudRate  = 115200ul;			//波特率,     110 ~ 115200
	COMx_InitStructure.UART_RxEnable  = ENABLE;				//接收允许,   ENABLE或DISABLE
	COMx_InitStructure.UART_Interrupt = ENABLE;				//中断允许,   ENABLE或DISABLE
	COMx_InitStructure.UART_Polity    = PolityLow;			//中断优先级, PolityLow,PolityHigh
	COMx_InitStructure.UART_P_SW      = UART2_SW_P10_P11;	//切换端口,   UART2_SW_P10_P11,UART2_SW_P46_P47
	USART_Configuration(USART2, &COMx_InitStructure);		//初始化串口2 USART1,USART2

}

void main(void)
{
	u16 time_vc_test,time_gpio_test;
	u8 gpio_test_status = 0;
	ADC_Init();
	INA219_Init();
	TIM_config();
	UART_config();
	EA = 1;
	while(1)
	{
		if(get_mode(VC_TEST))
		{
			time_vc_test = get_time();
			ENABLE_ADC_Convert();
			delay_ms(100);
			while(get_mode(VC_TEST) || timeout_nms(3000,time_vc_test));				
		}	
		if(get_mode(GPIO_TEST))
		{
			time_gpio_test = get_time();
			while(!timeout_nms(3000,time_gpio_test))
			{
				if(get_mode(GPIO_NEXT))
				{
					switch (gpio_test_status)
					{
						case TEST_GPIO_UP: GET_GPIO_Value();GPIO_Record_UP();break;
					
						case TEST_GPIO_DOWN:GET_GPIO_Value();GPIO_Record_DOWN();break;
					
						default:GET_GPIO_Value();GPIO_Record_WATER();break;
					}
					SET_GPIO_Next();
					reset_mode(GPIO_NEXT);
					gpio_test_status++;
				}
				
				if(get_mode(GPIO_END))
				{
					SEND_Hex_Record();
					reset_mode(GPIO_END);
					goto END;
				}
			}
		}	
	END:
		gpio_test_status = 0;
		delay_ms(1);
	}
}
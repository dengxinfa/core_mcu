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
	TIM_InitTypeDef		TIM_InitStructure;					//�ṹ����
	TIM_InitStructure.TIM_Mode      = TIM_16BitAutoReload;	//ָ������ģʽ,   TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
	TIM_InitStructure.TIM_Polity    = PolityLow;			//ָ���ж����ȼ�, PolityHigh,PolityLow
	TIM_InitStructure.TIM_Interrupt = ENABLE;				//�ж��Ƿ�����,   ENABLE��DISABLE
	TIM_InitStructure.TIM_ClkOut    = ENABLE;				//�Ƿ������������, ENABLE��DISABLE
	TIM_InitStructure.TIM_ValueMode = SYS_CLOCK;			//��ֵģʽ, TIME_us: ��װֵΪʱ��, ��λΪus.   SYS_CLOCK: ��װֵΪϵͳʱ����.	COUNTER: ���������ֵ(�̶�Ϊ1Tģʽ).
	TIM_InitStructure.TIM_Value     = MAIN_Fosc / 1000;	//��װֵ���ֵ,
	TIM_InitStructure.TIM_Run       = ENABLE;				//�Ƿ��ʼ����������ʱ��, ENABLE��DISABLE
	Timer_Inilize(Timer0,&TIM_InitStructure);				//��ʼ��Timer0	  Timer0,Timer1,Timer2,Timer3,Timer4	
	Timer_Inilize(Timer1,&TIM_InitStructure);
}

void	UART_config(void)
{
	COMx_InitDefine		COMx_InitStructure;					//�ṹ����
	COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;		//ģʽ,       UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
	COMx_InitStructure.UART_BRT_Use   = BRT_Timer2;			//ʹ�ò�����,   BRT_Timer1, BRT_Timer2 (ע��: ����2�̶�ʹ��BRT_Timer2)
	COMx_InitStructure.UART_BaudRate  = 115200ul;			//������, һ�� 110 ~ 115200
	COMx_InitStructure.UART_RxEnable  = ENABLE;				//��������,   ENABLE��DISABLE
	COMx_InitStructure.BaudRateDouble = DISABLE;			//�����ʼӱ�, ENABLE��DISABLE
	COMx_InitStructure.UART_Interrupt = ENABLE;				//�ж�����,   ENABLE��DISABLE
	COMx_InitStructure.UART_Polity    = PolityLow;			//�ж����ȼ�, PolityLow,PolityHigh
	COMx_InitStructure.UART_P_SW      = UART1_SW_P30_P31;	//�л��˿�,   UART1_SW_P30_P31,UART1_SW_P36_P37,UART1_SW_P16_P17(����ʹ���ڲ�ʱ��)
	COMx_InitStructure.UART_RXD_TXD_Short = DISABLE;		//�ڲ���·RXD��TXD, ���м�, ENABLE,DISABLE
	USART_Configuration(USART1, &COMx_InitStructure);		//��ʼ������1 USART1,USART2

	COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;		//ģʽ,       UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
	COMx_InitStructure.UART_BaudRate  = 115200ul;			//������,     110 ~ 115200
	COMx_InitStructure.UART_RxEnable  = ENABLE;				//��������,   ENABLE��DISABLE
	COMx_InitStructure.UART_Interrupt = ENABLE;				//�ж�����,   ENABLE��DISABLE
	COMx_InitStructure.UART_Polity    = PolityLow;			//�ж����ȼ�, PolityLow,PolityHigh
	COMx_InitStructure.UART_P_SW      = UART2_SW_P10_P11;	//�л��˿�,   UART2_SW_P10_P11,UART2_SW_P46_P47
	USART_Configuration(USART2, &COMx_InitStructure);		//��ʼ������2 USART1,USART2

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

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

/*************	����˵��	**************

���ļ�ΪSTC8xxxϵ�еĶ�ʱ����ʼ�����жϳ���,�û�����������ļ����޸��Լ���Ҫ���жϳ���.


******************************************/


#include	"timer.h"

u16 time_count;

/********************* Timer0�жϺ���************************/
void timer0_int (void) interrupt TIMER0_VECTOR
{
	//�粻���Զ���װģʽ, ���������װ��ֵ�Ĵ���
	if(time_count++ > 65535)
		time_count = 0;
}

/********************* Timer1�жϺ���************************/
void timer1_int (void) interrupt TIMER1_VECTOR
{
	//�粻���Զ���װģʽ, ���������װ��ֵ�Ĵ���
	if(get_mode(ADC_END))
	{
		reset_mode(ADC_END);
		ADC_Send_Current();
		INA_Process();
		INA_Send_Current();
		reset_mode(VC_TEST);
	}
}

/********************* Timer2�жϺ���************************/
void timer2_int (void) interrupt TIMER2_VECTOR
{
	P22 = ~P22;
}

/********************* Timer3�жϺ���************************/
void timer3_int (void) interrupt TIMER3_VECTOR
{
	P23 = ~P23;
}

/********************* Timer4�жϺ���************************/
void timer4_int (void) interrupt TIMER4_VECTOR
{
	P24 = ~P24;
}

//========================================================================
// ����: u8	Timer_Inilize(u8 TIM, TIM_InitTypeDef *TIMx)
// ����: ��ʱ����ʼ������.
// ����: TIMx: �ṹ����,��ο�timer.h��Ķ���.
//       �ر�˵��:	TIM_ValueMode=COUNTERʱ,   TIM_Value��ֵΪ��������ĳ�ֵ, 1Tģʽ.
//					TIM_ValueMode=TIME_usʱ,   TIM_Value��ֵΪ��ʱus��, ʵ��װ��ʱ��65536����ʱֵ, �Զ��ж�12T��1Tģʽ.
//					TIM_ValueMode=SYS_CLOCKʱ, TIM_Value��ֵΪ��ʱϵͳʱ����, ʵ��װ��ʱ��65536����ʱֵ, �Զ��ж�12T��1Tģʽ.
// ����: 0: ��ʼ���ɹ�,    1: ��ʱ����Ź������,   2: ֵ�������.
// �汾: V1.0, 2018-3-22
//========================================================================
u8	Timer_Inilize(u8 TIM, TIM_InitTypeDef *TIMx)
{
	u8	div12;
	if(TIM > Timer4)	return 1;	//��ʱ����Ź������

	if(TIMx->TIM_ValueMode == COUNTER)	//��װֵΪ���������ֵ, �̶�Ϊ1T����ģʽ
	{
		if(TIMx->TIM_Value >= 65536UL)	return 2;	//ֵ����, ���ش���
		div12 = 0;		//1T mode
	}
	else	//��װֵΪus��ϵͳʱ����
	{
		if(TIMx->TIM_ValueMode == TIME_us)		//��װֵΪus
			TIMx->TIM_Value = (u32)(((float)MAIN_Fosc * (float)TIMx->TIM_Value)/1000000UL);	//��װ����ʱ��(us), ��������Ҫ��ϵͳʱ����.

		if(TIMx->TIM_Value >= (65536UL * 12))	return 2;	//ֵ����, ���ش���
		if(TIMx->TIM_Value < 65536UL)	div12 = 0;		//1T mode
		else
		{
			div12 = 1;	//12T mode
			TIMx->TIM_Value = TIMx->TIM_Value / 12;
		}
	}

	if(TIM == Timer0)
	{
		if(TIMx->TIM_Mode >  TIM_16BitAutoReloadNoMask)	return 3;	//ģʽ����
		TR0 = 0;	//ֹͣ����
		ET0 = 0;	//��ֹ�ж�
		PT0 = 0;	//�����ȼ��ж�
		TMOD &= 0xf0;	//��ʱģʽ, 16λ�Զ���װ
		AUXR &= ~0x80;	//12Tģʽ, 
		INT_CLKO &= ~0x01;	//�����ʱ��
		if(TIMx->TIM_Interrupt == ENABLE)		ET0 = 1;	//�����ж�
		if(TIMx->TIM_Polity == PolityHigh)		PT0 = 1;	//�����ȼ��ж�
		if(TIMx->TIM_ClkOut == ENABLE)	INT_CLKO |=  0x01;	//���ʱ��
		TMOD |= TIMx->TIM_Mode;	//����ģʽ,0: 16λ�Զ���װ, 1: 16λ��ʱ/����, 2: 8λ�Զ���װ, 3: 16λ�Զ���װ, ���������ж�
		if(TIMx->TIM_ValueMode == COUNTER)		//����������Ƶ
		{
			TMOD |=  0x04;	//����������Ƶ
			if((TIMx->TIM_Mode == TIM_8BitAutoReload) && (TIMx->TIM_Value >= 256))	return 2;	//ֵ����, ���ش���  8λ��װģʽ
		}
		if(TIMx->TIM_Mode == TIM_8BitAutoReload)	//��ʱ��8λ��װģʽ
		{
			if((div12 == 1) || (TIMx->TIM_Value >= (256 * 12)))	return 2;	//12Tģʽ, ֵ����, ���ش���
			else if(TIMx->TIM_Value > 256)	TIMx->TIM_Value = TIMx->TIM_Value / 12;	//12T
			else AUXR |=  0x80;	//��ʱ��, 1Tģʽ
			if(TIMx->TIM_ValueMode != COUNTER)	TIMx->TIM_Value = 256 - TIMx->TIM_Value;	//��ʱģʽ
			TH0 = (u8)(TIMx->TIM_Value);
			TL0 = (u8)(TIMx->TIM_Value);
		}
		else	//��ʱ��16λģʽ
		{
			if(div12 == 0)		AUXR |=  0x80;	//��ʱ��, 1Tģʽ	
			if(TIMx->TIM_ValueMode != COUNTER)	TIMx->TIM_Value = 65536UL - TIMx->TIM_Value;	//��ʱģʽ
			TH0 = (u8)(TIMx->TIM_Value >> 8);
			TL0 = (u8)(TIMx->TIM_Value);
		}
		if(TIMx->TIM_Run == ENABLE)	TR0 = 1;	//��ʼ����
		return	0;		//�ɹ�
	}

	if(TIM == Timer1)
	{
		if(TIMx->TIM_Mode >= TIM_16BitAutoReloadNoMask)	return 2;	//����
		TR1 = 0;	//ֹͣ����
		ET1 = 0;	//��ֹ�ж�
		PT1 = 0;	//�����ȼ��ж�
		TMOD &=  0x0f;	//��ʱģʽ, 16λ�Զ���װ
		AUXR &= ~0x40;	//12Tģʽ, 
		INT_CLKO &= ~0x02;	//�����ʱ��
		if(TIMx->TIM_Interrupt == ENABLE)		ET1 = 1;	//�����ж�
		if(TIMx->TIM_Polity == PolityHigh)		PT1 = 1;	//�����ȼ��ж�
		if(TIMx->TIM_ClkOut == ENABLE)	INT_CLKO |=  0x02;	//���ʱ��
		TMOD |= (TIMx->TIM_Mode << 4);	//����ģʽ,0: 16λ�Զ���װ, 1: 16λ��ʱ/����, 2: 8λ�Զ���װ
		if(TIMx->TIM_ValueMode == COUNTER)		//����������Ƶ
		{
			TMOD |=  0x40;	//����������Ƶ
			if((TIMx->TIM_Mode == TIM_8BitAutoReload) && (TIMx->TIM_Value >= 256))	return 2;	//ֵ����, ���ش���  8λ��װģʽ
		}
		if(TIMx->TIM_Mode == TIM_8BitAutoReload)	//��ʱ��8λ��װģʽ
		{
			if((div12 == 1) || (TIMx->TIM_Value >= (256 * 12)))	return 2;	//12Tģʽ, ֵ����, ���ش���
			else if(TIMx->TIM_Value > 256)	TIMx->TIM_Value = TIMx->TIM_Value / 12;	//12T
			else AUXR |=  0x40;	//��ʱ��, 1Tģʽ
			if(TIMx->TIM_ValueMode != COUNTER)	TIMx->TIM_Value = 256 - TIMx->TIM_Value;	//��ʱģʽ
			TH1 = (u8)(TIMx->TIM_Value);
			TL1 = (u8)(TIMx->TIM_Value);
		}
		else	//��ʱ��16λģʽ
		{
			if(div12 == 0)		AUXR |=  0x40;	//��ʱ��, 1Tģʽ	
			if(TIMx->TIM_ValueMode != COUNTER)	TIMx->TIM_Value = 65536UL - TIMx->TIM_Value;	//��ʱģʽ
			TH1 = (u8)(TIMx->TIM_Value >> 8);
			TL1 = (u8)(TIMx->TIM_Value);
		}

		if(TIMx->TIM_Run == ENABLE)	TR1 = 1;	//��ʼ����
		return	0;		//�ɹ�
	}

	if(TIM == Timer2)		//Timer2,�̶�Ϊ16λ�Զ���װ, �ж������ȼ�
	{
		AUXR &= ~0x1c;		//ֹͣ����, ��ʱģʽ, 12Tģʽ
		IE2  &= ~(1<<2);	//��ֹ�ж�
		INT_CLKO &= ~0x04;	//�����ʱ��
		if(TIMx->TIM_Interrupt == ENABLE)	IE2  |=  (1<<2);	//�����ж�
		if(TIMx->TIM_ClkOut == ENABLE)	INT_CLKO |=  0x04;		//���ʱ��
		if(TIMx->TIM_ValueMode == COUNTER)	AUXR |=  (1<<3);	//����������Ƶ
		if(div12 == 0)						AUXR |=  (1<<2);	//1Tģʽ
		if(TIMx->TIM_ValueMode != COUNTER)	TIMx->TIM_Value = 65536UL - TIMx->TIM_Value;	//��ʱģʽ
		TH2 = (u8)(TIMx->TIM_Value >> 8);
		TL2 = (u8)(TIMx->TIM_Value);
		if(TIMx->TIM_Run == ENABLE)	AUXR |=  (1<<4);	//��ʼ����
		return	0;		//�ɹ�
	}

	if(TIM == Timer3)		//Timer3,�̶�Ϊ16λ�Զ���װ, �ж������ȼ�
	{
		T4T3M &= 0xf0;		//ֹͣ����, ��ʱģʽ, 12Tģʽ, �����ʱ��
		IE2  &= ~(1<<5);	//��ֹ�ж�
		if(TIMx->TIM_Interrupt == ENABLE)	IE2   |=  (1<<5);	//�����ж�
		if(TIMx->TIM_ClkOut == ENABLE)		T4T3M |=  1;		//���ʱ��
		if(TIMx->TIM_ValueMode == COUNTER)	T4T3M |=  (1<<2);	//����������Ƶ
		if(div12 == 0)						T4T3M |=  (1<<1);	//1Tģʽ
		if(TIMx->TIM_ValueMode != COUNTER)	TIMx->TIM_Value = 65536UL - TIMx->TIM_Value;	//��ʱģʽ
		TH3 = (u8)(TIMx->TIM_Value >> 8);
		TL3 = (u8)(TIMx->TIM_Value);
		if(TIMx->TIM_Run == ENABLE)	T4T3M |=  (1<<3);	//��ʼ����
		return	0;		//�ɹ�
	}

	if(TIM == Timer4)		//Timer4,�̶�Ϊ16λ�Զ���װ, �ж������ȼ�
	{
		T4T3M &= 0x0f;		//ֹͣ����, ��ʱģʽ, 12Tģʽ, �����ʱ��
		IE2  &= ~(1<<6);	//��ֹ�ж�
		if(TIMx->TIM_Interrupt == ENABLE)	IE2   |=  (1<<6);	//�����ж�
		if(TIMx->TIM_ClkOut == ENABLE)		T4T3M |=  (1<<4);	//���ʱ��
		if(TIMx->TIM_ValueMode == COUNTER)	T4T3M |=  (1<<6);	//����������Ƶ
		if(div12 == 0)						T4T3M |=  (1<<5);	//1Tģʽ
		if(TIMx->TIM_ValueMode != COUNTER)	TIMx->TIM_Value = 65536UL - TIMx->TIM_Value;	//��ʱģʽ
		TH4 = (u8)(TIMx->TIM_Value >> 8);
		TL4 = (u8)(TIMx->TIM_Value);
		if(TIMx->TIM_Run == ENABLE)	T4T3M |=  (1<<7);	//��ʼ����
		return	0;		//�ɹ�
	}

	return 2;	//����
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

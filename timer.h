
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

#ifndef	__TIMER_H
#define	__TIMER_H

#include	"config.h"

#define	Timer0						0
#define	Timer1						1
#define	Timer2						2
#define	Timer3						3
#define	Timer4						4

#define	TIM_16BitAutoReload			0
#define	TIM_16Bit					1
#define	TIM_8BitAutoReload			2
#define	TIM_16BitAutoReloadNoMask	3

#define	SYS_CLOCK					0
#define	TIME_us						1
#define	COUNTER						2

typedef struct
{
	u8	TIM_Mode;		//����ģʽ,  	TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
	u8	TIM_Interrupt;	//�ж�����		ENABLE,DISABLE
	u8	TIM_Polity;		//���ȼ�����	PolityHigh,PolityLow
	u8	TIM_ClkOut;		//�ɱ��ʱ�����,	ENABLE,DISABLE
	u8	TIM_ValueMode;	//��ֵģʽ, TIME_us: ��װֵΪʱ��, ��λΪus.   SYS_CLOCK: ��װֵΪϵͳʱ����.	COUNTER: ���������ֵ(�̶�Ϊ1Tģʽ).
	u32	TIM_Value;		//װ�س�ֵ
	u8	TIM_Run;		//�Ƿ�����		ENABLE,DISABLE
} TIM_InitTypeDef;

u8	Timer_Inilize(u8 TIM, TIM_InitTypeDef *TIMx);
u16 get_time(void);
u8 timeout_nms(u16 timeout, u16 starttime);

#endif

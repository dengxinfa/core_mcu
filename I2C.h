
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


#ifndef	__I2C_H
#define	__I2C_H

#include "config.h"

/****************************/

void	WriteNbyte(u8 device_addr, u8 addr, u8 *p, char number);
void ReadNbyte(u8 device_addr, u8 addr, u8 *p, char number);
void ina219_Write_Register(u8 device_addr,  u8 reg, u16 dat);
void ina219_Write_Register_Init(u8 device_addr,  u8 reg, u16 dat);
void ina219_Read_Register(u8 device_addr,  u8 reg, s16 *dat);
#endif


#include "ina219.h"

u8 ina219_calValue;
u8  ina219_busVolt_LSB_mV = 4;   // Bus Voltage LSB value = 4mV
u8  ina219_shuntVolt_LSB_uV = 10;  // Shunt Voltage LSB value = 10uV
u32 ina219_current_LSB_uA;
u32 ina219_power_LSB_mW;

INA219_DATA ina219_20v;
INA219_DATA ina219_33v;
s32 INA219_Buff[5];

// INA219 Set Calibration 16V/16A(Max) 0.02Ω
void ina219_SetCalibration_16V_16A(u8 DEVICE_ADDR)
{
  u16 configValue;
  
  // By default we use a pretty huge range for the input voltage,
  // which probably isn't the most appropriate choice for system
  // that don't use a lot of power.  But all of the calculations
  // are shown below if you want to change the settings.  You will
  // also need to change any relevant register settings, such as
  // setting the VBUS_MAX to 16V instead of 32V, etc.
  
  // VBUS_MAX     = 16V   (Assumes 16V, can also be set to 32V)
  // VSHUNT_MAX   = 0.32  (Assumes Gain 8, 320mV, can also be 0.16, 0.08, 0.04)
  // RSHUNT       = 0.02   (Resistor value in ohms)
  
  // 1. Determine max possible current
  // MaxPossible_I = VSHUNT_MAX / RSHUNT
  // MaxPossible_I = 16A
  
  // 2. Determine max expected current
  // MaxExpected_I = 16A
  
  // 3. Calculate possible range of LSBs (Min = 15-bit, Max = 12-bit)
  // MinimumLSB = MaxExpected_I/32767
  // MinimumLSB = 0.00048            (0.48mA per bit)
  // MaximumLSB = MaxExpected_I/4096
  // MaximumLSB = 0,00390            (3.9mA per bit)
  
  // 4. Choose an LSB between the min and max values
  //    (Preferrably a roundish number close to MinLSB)
  // CurrentLSB = 0.00050            (500uA per bit)
  
  // 5. Compute the calibration register
  // Cal = trunc (0.04096 / (Current_LSB * RSHUNT))
  // Cal = 4096 (0x1000)
  
  ina219_calValue = 0x1000;
  
  // 6. Calculate the power LSB
  // PowerLSB = 20 * CurrentLSB
  // PowerLSB = 0.01 (10mW per bit)
  
  // 7. Compute the maximum current and shunt voltage values before overflow
  //
  // Max_Current = Current_LSB * 32767
  // Max_Current = 16.3835A before overflow
  //
  // If Max_Current > Max_Possible_I then
  //    Max_Current_Before_Overflow = MaxPossible_I
  // Else
  //    Max_Current_Before_Overflow = Max_Current
  // End If
  //
  // Max_ShuntVoltage = Max_Current_Before_Overflow * RSHUNT
  // Max_ShuntVoltage = 0.32V
  //
  // If Max_ShuntVoltage >= VSHUNT_MAX
  //    Max_ShuntVoltage_Before_Overflow = VSHUNT_MAX
  // Else
  //    Max_ShuntVoltage_Before_Overflow = Max_ShuntVoltage
  // End If
  
  // 8. Compute the Maximum Power
  // MaximumPower = Max_Current_Before_Overflow * VBUS_MAX
  // MaximumPower = 1.6 * 16V
  // MaximumPower = 256W
  
  // Set multipliers to convert raw current/power values
  ina219_current_LSB_uA = 480;     // Current LSB = 500uA per bit
  ina219_power_LSB_mW = 10;        // Power LSB = 10mW per bit = 20 * Current LSB
  
  // Set Calibration register to 'Cal' calculated above
  //ina219_Write_Register(DEVICE_ADDR, INA219_REG_CALIBRATION, ina219_calValue);
  
  // Set Config register to take into account the settings above
  configValue = ( INA219_CFG_BVOLT_RANGE_16V | INA219_CFG_SVOLT_RANGE_320MV | INA219_CFG_BADCRES_12BIT_16S_8MS | INA219_CFG_SADCRES_12BIT_16S_8MS | INA219_CFG_MODE_SANDBVOLT_CONTINUOUS );
  
  ina219_Write_Register_Init(DEVICE_ADDR, INA219_REG_CONFIG, configValue);
}

void ina219_configureRegisters(void)
{
  delay_ms(15);
  
  ina219_SetCalibration_16V_16A(INA219_20V);
	ina219_SetCalibration_16V_16A(INA219_33V);
}


void INA219_Init(void)
{  
  ina219_configureRegisters();
}


/* 这里以上是初始化函数 */
/* 从这里往下是功能函数 */


//s16 ina219_GetBusVoltage_raw(u8 DEVICE_ADDR)
//{
//  s16 val;
//  
//  ina219_Read_Register(DEVICE_ADDR, INA219_REG_BUSVOLTAGE, &val);
//  val >>= 3;                      // Shift to the right 3 to drop CNVR and OVF
//  
//  return (val);
//}

//s16 ina219_GetCurrent_raw(u8 DEVICE_ADDR)
//{
//  s16 val;
//  
//  // Sometimes a sharp load will reset the INA219, which will
//  // reset the cal register, meaning CURRENT and POWER will
//  // not be available ... avoid this by always setting a cal
//  // value even if it's an unfortunate extra step
//  ina219_Write_Register(DEVICE_ADDR, INA219_REG_CALIBRATION, ina219_calValue);
//  
//  // Now we can safely read the CURRENT register!
//  ina219_Read_Register(DEVICE_ADDR, INA219_REG_CURRENT, &val);
//  
//  return (val);
//}


//s16 ina219_GetBusVoltage_mV(u8 DEVICE_ADDR)
//{
//  s16 val;
//  
//  ina219_Read_Register(DEVICE_ADDR, INA219_REG_BUSVOLTAGE, &val);
//  val >>= 3;                      // Shift to the right 3 to drop CNVR and OVF
//  val *= ina219_busVolt_LSB_mV;   // multiply by LSB(4mV)
//  
//  return (val);
//}

//s32 ina219_GetShuntVoltage_uV(u8 DEVICE_ADDR)
//{
//  s32 val;
//  s16 reg;
//  
//  ina219_Read_Register(DEVICE_ADDR, INA219_REG_SHUNTVOLTAGE, &reg);
//  val = (s32)reg * ina219_shuntVolt_LSB_uV;   // multiply by LSB(10uV)
//  
//  return (val);
//}

s32 ina219_GetCurrent_uA(u8 DEVICE_ADDR)
{
  s32 val;
  s16 reg;
  
  // Sometimes a sharp load will reset the INA219, which will
  // reset the cal register, meaning CURRENT and POWER will
  // not be available ... avoid this by always setting a cal
  // value even if it's an unfortunate extra step
  ina219_Write_Register(DEVICE_ADDR, INA219_REG_CALIBRATION, ina219_calValue);
  
  // Now we can safely read the CURRENT register!
  ina219_Read_Register(DEVICE_ADDR, INA219_REG_CURRENT, &reg);
  
  val = (s32)reg * ina219_current_LSB_uA;
  
  return (val);
}

//s32 ina219_GetPower_mW(u8 DEVICE_ADDR)
//{
//  s32 val;
//  s16 reg;
//  
//  // Sometimes a sharp load will reset the INA219, which will
//  // reset the cal register, meaning CURRENT and POWER will
//  // not be available ... avoid this by always setting a cal
//  // value even if it's an unfortunate extra step
//  ina219_Write_Register(DEVICE_ADDR,INA219_REG_CALIBRATION,ina219_calValue);
//  // Now we can safely read the POWER register!
//  ina219_Read_Register(DEVICE_ADDR, INA219_REG_POWER, &reg);
//  
//  val = (s32)reg * ina219_power_LSB_mW;
//  
//  return (val);
//}

s32 INA219_Filter(void)    //中值滤波
{
	u8 i;
	u16 temp;
	for(i=1; i<6; i++)    //切换通道后第一次转换结果丢弃. 避免采样电容的残存电压影响.
		if(INA219_Buff[i] > INA219_Buff[i+1])
		{
			temp = INA219_Buff[i];
			INA219_Buff[i] = INA219_Buff[i+1];
			INA219_Buff[i+1] = temp;
		}			
	return INA219_Buff[3];
}

void INA_Current_To_ASCII(s32 current_data)
{
	u8 buff[7];
	current_data /= 1000;
	buff[0] = current_data/1000 + '0';
	buff[1] = current_data%1000/100 + '0';
	buff[2] = current_data%100/10 + '0';
	buff[3] = current_data%10 + '0';
	buff[4] =  'm';
	buff[5] =  'A';
	buff[6] =  0;
	PrintString1(buff);
}

void INA_Send_Current(void)
{
	PrintString1("3.3V_2.0V_Current: ");
	INA_Current_To_ASCII(ina219_33v.current_ina219);
	PrintString1(" ");
	INA_Current_To_ASCII(ina219_20v.current_ina219);
	PrintString1("\r\n");
}

void INA_Process(void)
{ 
	u8 i;
	for(i=0;i<5;i++)
		INA219_Buff[i] = ina219_GetCurrent_uA(INA219_20V); 
	ina219_20v.current_ina219 = INA219_Filter;
	for(i=0;i<5;i++)
		INA219_Buff[i] = ina219_GetCurrent_uA(INA219_33V); 
	ina219_33v.current_ina219 = INA219_Filter;
	
//	ina219_20v.voltage_ina219 = ina219_GetBusVoltage_mV(INA219_20V);      
//	ina219_20v.shunt_ina219 = ina219_GetShuntVoltage_uV(INA219_20V);           
//	ina219_20v.current_ina219 = ina219_GetCurrent_uA(INA219_20V);       
//	ina219_20v.power_ina219 = ina219_GetPower_mW(INA219_20V);
//	
//	ina219_33v.voltage_ina219 = ina219_GetBusVoltage_mV(INA219_33V);      
//	ina219_33v.shunt_ina219 = ina219_GetShuntVoltage_uV(INA219_33V);           
//	ina219_33v.current_ina219 = ina219_GetCurrent_uA(INA219_33V);       
//	ina219_33v.power_ina219 = ina219_GetPower_mW(INA219_33V);
}

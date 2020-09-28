#ifndef _DS18B20_H
#define _DS18B20_H

#include <stdio.h>
#include "stm32f4xx.h"


#define DS18B20_SKIP_ROM 			0xCC
#define DS18B20_Read_ROM 			0X33
#define DS18B20_MATCH_ROM 			0X55
#define DS18B20_SEARCH_ROM 			0XF0
#define DS18B20_ALARM_SEARCH 		0XEC

#define DS18B20_WRITE_SCRATCHPAD 	0X4E
#define DS18B20_READ_SCRATCHPAD 	0XBE
#define DS18B20_CONVERT_TEMPERATURE 0X44


#define DS18B20_GPIOx			GPIOC
#define DS18B20_GPIO_Pin_x		GPIO_Pin_1
#define DS18B20_DQ_OUT_L 		GPIO_ResetBits(DS18B20_GPIOx, DS18B20_GPIO_Pin_x)
#define DS18B20_DQ_OUT_H		GPIO_SetBits(DS18B20_GPIOx, DS18B20_GPIO_Pin_x)
#define	DS18B20_DQ_IN  			GPIO_ReadInputDataBit(DS18B20_GPIOx, DS18B20_GPIO_Pin_x)
/**
 * @brief Definition for 
 *	
 */ 		 


struct DS18B20_ROM_Code
{
	unsigned char familyCode;
	unsigned char serialNumber[6];
	unsigned char crc;
};
void DS18B20_Init(void);
uint8_t DS18B20_Read_Bit(void) ;
void DS18B20_write_Bit(unsigned char data); 
void DS18B20_WriteByte(unsigned char data);
unsigned char DS18B20_ReadByte(void);
unsigned char DS18B20_ExistenceCheck(void);
void DS18B20_Reset(void);

void DS18B20_Info(void);
float DS18B20_Temperature(void);

void MyDelayTenus(unsigned int i);
void MyDelayus(unsigned int timesus);
#endif /*_DS18B20_H*/

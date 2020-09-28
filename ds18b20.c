#include "ds18b20.h"
#include "usart.h"


/*******************************************************************************
* Function Name  : USART_Configuration
* Description    : Configure Open_USART 
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
/**
 * @brief Definition for COM port1, connected to USART3
 *	UARTx		TX	RX
 *	USART1	PA9	PA10
 *	USART2 	PA2	PA3 
 *	USART3	PC10 PC11
 */  
 void MyDelayTenus(unsigned int tenTimesus)
 {
	uint8_t timesOfTenus = 0;
	int16_t counter = 0;
	for(timesOfTenus=0;timesOfTenus<tenTimesus;timesOfTenus++)
	{
		for(counter=335;counter>=0;)	//10us
		{
			counter--;
		}
	}
 }
void  MyDelayus(unsigned int timesus)
{
	unsigned int timeOfus=0;
	int8_t counter = 0;
	while(timeOfus<timesus)
	{
		for(counter=33;counter>=0;)
		{
			counter--;
		}
		timeOfus++;
	}
}

void DS18B20_Init()
{
	
	GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = DS18B20_GPIO_Pin_x;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//内部上拉是弱上拉，如需大电流需外部上拉4.7K电阻
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(DS18B20_GPIOx, &GPIO_InitStructure); 
	
	DS18B20_Reset();
	DS18B20_ExistenceCheck();
	
}
void DS18B20_Reset(void)
{
	DS18B20_DQ_OUT_L;
	MyDelayTenus(60);	//us	480~960us	
	DS18B20_DQ_OUT_H;
	MyDelayus(15);	//us	15-60us
} 
unsigned char DS18B20_ExistenceCheck(void)
{
	uint8_t retry = 0;
	//依据手册，在检测到输出gpio有一个上升沿信号后，DS18B20等待15-60us并返回一个(60-240us)的低电平信号
    while(DS18B20_DQ_IN) 
	{ 
		if(retry++ > 60)
		{ 
			return 0;
		} 
		else 
		{
			MyDelayus(1);	
		} 
	}	
    retry = 0;	
    while(!DS18B20_DQ_IN) 
	{ 
//		USART_SendByte(USART1,0xAA);
		if(retry++ > 240)
		{
			return 0;
		} 
		else 
		{
			MyDelayus(1);	
		} 
	}	
	return 1;
}

uint8_t DS18B20_Read_Bit(void) 
{
    uint8_t data = 0;
	
    DS18B20_DQ_OUT_L;
    MyDelayus(3);
    DS18B20_DQ_OUT_H;
    MyDelayus(3);
    data = DS18B20_DQ_IN;
    MyDelayTenus(5);	//50us
    return data;
}

unsigned char DS18B20_ReadByte(void)
{
	unsigned char data = 0;
	unsigned char i=0;
    for(;i<8;i++)
    {
		data >>= 1;
		if(DS18B20_Read_Bit())
		{
			data |= 0x80;			
		}
    }
    return data;
}

void DS18B20_write_Bit(unsigned char data) 
{
	
	if(data&0x01)	//依据手册，写0时序是先有>60us的低电平，再有>1us的高电平	;写1时序是先有>1us的低电平，再有>60us的高电平
	{
		DS18B20_DQ_OUT_L;
		MyDelayus(3);     
		DS18B20_DQ_OUT_H;
		MyDelayus(65);    	 
	}
	else 						
	{
		DS18B20_DQ_OUT_L;
		MyDelayus(65);      
		DS18B20_DQ_OUT_H;
		MyDelayus(3);     
	}
}
void DS18B20_WriteByte(unsigned char data)
{
	unsigned char  i=0;
//	USART_SendByte(USART1,data);
	for (; i<8; i++) 			
    {		
		DS18B20_write_Bit(data);
		data >>= 1;	//读和写均是先低位后高位
    }
}
void DS18B20_Info()
{
	struct DS18B20_ROM_Code ROM_Code;
	uint8_t i=0;
	DS18B20_Reset();	//主站发复位脉冲
	DS18B20_ExistenceCheck();//从站发存在脉冲，DS18B20回应
	DS18B20_WriteByte(DS18B20_SEARCH_ROM);//主站发出搜索ROM指令
	
	ROM_Code.familyCode = DS18B20_ReadByte();	//64 位ROM code ,前8bit是家族码,都是0x28,接着的48bit是序列号，最后8w位是校验位
	ROM_Code.serialNumber[i++]=DS18B20_ReadByte();
	ROM_Code.serialNumber[i++]=DS18B20_ReadByte();
	ROM_Code.serialNumber[i++]=DS18B20_ReadByte();
	ROM_Code.serialNumber[i++]=DS18B20_ReadByte();
	ROM_Code.serialNumber[i++]=DS18B20_ReadByte();
	ROM_Code.serialNumber[i++]=DS18B20_ReadByte();
	ROM_Code.crc=DS18B20_ReadByte();
}

/*分辨率	最长转换时间
 * 9bit			93.75ms(tconv/8)
 * 10bit		187.5ms(tconv/4)
 * 11bit		375ms(tconv/2)
 * 12bit		750ms(tconv)
 */
float DS18B20_Temperature(void)	//
{
	float data = 0.0;
	uint8_t checkTrue=0, highByte=0,lowByte=0;
//	uint8_t tempIntNumber=0;
	
	DS18B20_Reset();
	checkTrue = DS18B20_ExistenceCheck();
//	USART_SendString(USART1,(uint8_t *)"checkTrue: ");
	
//	USART_SendByte(USART1,checkTrue);
	DS18B20_WriteByte(DS18B20_SKIP_ROM);//忽略ROM地址，直接发命令
	DS18B20_WriteByte(DS18B20_CONVERT_TEMPERATURE);//温度转换命令
	
	DS18B20_Reset();
	checkTrue = DS18B20_ExistenceCheck();	//
//	USART_SendByte(USART1,checkTrue);
	DS18B20_WriteByte(DS18B20_SKIP_ROM);//忽略ROM地址，直接发命令	每次向下发送命令都要有对ROM的写指令
	DS18B20_WriteByte(DS18B20_READ_SCRATCHPAD);//读取寄存器数据的命令
	lowByte=DS18B20_ReadByte();
	highByte=DS18B20_ReadByte();
	
	//以下是12bit时的分布
	//LSB 2^3	2^2	2^1 2^0 2^-1	2^-2 	2^-3 	2^-4
	//MSB S		S	S	S	S		2^6		2^5		2^4
	//以下是9bit时的数据 S表示符号 1为负
	//LSB 2^6 	2^5 2^4	2^3 2^2		2^1		2^0 	2^-1
	//MSB S		S	S	S	S		S		S		S
//	tempIntNumber = (highByte&0x07<<4)|(lowByte>>4);
	data=((((highByte&0x07)<<4)|(lowByte>>4))+(float)((lowByte>>3)&0x01)/2+
		(float)((lowByte>>2)&0x01)/4+(float)((lowByte>>1)&0x01)/8+(float)(lowByte&0x01)/16)*((highByte>>3)!=0?-1:1);
	USART_SendByte(USART1,highByte);USART_SendByte(USART1,lowByte);
//	data=(DS18B20_ReadByte()*(DS18B20_ReadByte()!=0?-1:1));	//先低字节后高字节，默认12bit
	return data;
}


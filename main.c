#include <xc.h>
#include <pic12f1840.h>
#include "GXHT30.h"
#include "MB1.h"



int Temperature;
int Humidity;
int Temperature_Value = 0;
int Humidity_Value = 0;
char ten = 0, one = 0, hundred = 0;
char ten1 = 0, one1 = 0, hundred1 = 0;

void main(void)
{
	main_init();
	IIC_init();
	TXSTAbits.TXEN = 1;		// 使能发送
	__delay_ms(10);
	
	while(1)
	{
		Clear();					// 初始化
		GXHT30_single_call();		// 单次读取温湿度
		bit_Value();				// ASCII转换
		TXREG_TRMT();				// 发送
		__delay_ms(800);			// 频率
		
	}
	
}

char ASCII_TH(int Value, char bit)
{
	if(bit == 1)
	{
		return (char)(Value % 10) + '0';
	}
	else if(bit == 2)
	{
		return (char)(Value % 100 / 10) + '0';
	}
	else if(bit == 3)
	{
		return (char)(Value / 100) + '0';
	}
	else
	{
		return '?';
	}
}

void TXREG_TRMT(void)
{
		TXREG = hundred;
		while(!TXSTAbits.TRMT);
		TXREG = ten;
		while(!TXSTAbits.TRMT);
		TXREG = 46;
		while(!TXSTAbits.TRMT);
		TXREG = one;
		while(!TXSTAbits.TRMT);
		TXREG = 161;
		while(!TXSTAbits.TRMT);
		TXREG = 230;
		while(!TXSTAbits.TRMT);
		TXREG = 32;
		while(!TXSTAbits.TRMT);
		TXREG = 32;
		while(!TXSTAbits.TRMT);
		
		TXREG = hundred1;
		while(!TXSTAbits.TRMT);
		TXREG = ten1;
		while(!TXSTAbits.TRMT);
		TXREG = 46;
		while(!TXSTAbits.TRMT);
		TXREG = one1;
		while(!TXSTAbits.TRMT);
		TXREG = 37;
		while(!TXSTAbits.TRMT);
		TXREG = 32;
		while(!TXSTAbits.TRMT);
		TXREG = 13;
		while(!TXSTAbits.TRMT);
		__delay_ms(5);
		
}

void Clear(void)
{
	ten = 0;
	one = 0;
	hundred = 0;
	ten1 = 0;
	one1 = 0;	
	hundred1 = 0;
	Temperature = 0;
	Humidity = 0;
	Temperature_Value = 0;
	Humidity_Value = 0;
	
}

void bit_Value(void)
{
	if(Humidity == 1000)	Humidity = 999;
		
	one = ASCII_TH(Temperature, 1);
	ten = ASCII_TH(Temperature, 2);
	hundred = ASCII_TH(Temperature, 3);
	
	one1 = ASCII_TH(Humidity, 1);
	ten1 = ASCII_TH(Humidity, 2);
	hundred1 = ASCII_TH(Humidity, 3);
	__delay_ms(5);
	
}




/*
void __interrupt() ISR(void)	// 中断处理函数
{
	if(TXIF == 1 && TXIF == 1)
	{
		TXIE = 0;
		TXIF = 0;

		TXIE = 1;
		
	}
}
*/

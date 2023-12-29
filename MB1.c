#include <xc.h>
#include "MB1.h"
#include "GXHT30.h"

void main_init(void)
{
	T_DI = 0;
	T_RO = 1;
	//T_RDE = 0;
	//RDE = 1;
	DI = 0;	
	RO = 0;
	
	OSCCON = 0b01110000;	// 设置振荡器为8MHz, 使能PLL(倍频)
	ANSELA = 0;
	APFCONbits.RXDTSEL = 0;	// 将RA1设定为接收
	APFCONbits.TXCKSEL = 0;	// 将RA0设定为发送
	TXSTAbits.BRGH = 1;		// 高速波特率
	BAUDCONbits.BRG16 = 1;	// 使用 16 位波特率发生器
	SPBRG = 207;
	TXSTAbits.SYNC = 0;		// 异步模式
	RCSTAbits.SPEN = 1;		// 使能异步串口
	
	// 发送中断
	/*
	INTCONbits.GIE = 1;
	INTCONbits.PEIE = 1;
	PIE1bits.TXIE = 1;
	PIR1bits.TXIF = 0;
	*/
}


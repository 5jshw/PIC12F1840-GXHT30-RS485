#include <xc.h>
#include <pic12f1840.h>
#include "MB1.h"
#include "GXHT30.h"

void main_init(void)
{
	T_DI = 0;
	T_RO = 1;
	DI = 0;	
	RO = 0;
	
	OSCCON = 0b01110000;	// 设置振荡器为8MHz, 禁止PLL(倍频)
	//OSCCON = 0b11110000;	// 设置振荡器为32MHz, 使能PLL(倍频)
	ADCON0 = 0x10;
	ANSELA = 0;
	OPTION_REG = 0x84;
	APFCONbits.RXDTSEL = 0;	// 将RA1设定为接收
	APFCONbits.TXCKSEL = 0;	// 将RA0设定为发送
	TXSTAbits.BRGH = 1;		// 高速波特率
	BAUDCONbits.BRG16 = 1;	// 使用 16 位波特率发生器
	SPBRG = 207;			// 8MHz下 9600
	TXSTAbits.SYNC = 0;		// 异步模式
	RCSTAbits.SPEN = 1;		// 使能异步串口
	RCSTAbits.RX9 = 0;		// 关闭第9位接收
	TXSTAbits.TX9 = 0;		// 关闭第9位发送
	TXSTAbits.TXEN = 1;		// 发送使能
	RCSTAbits.CREN = 1;		// 连续接收使能
	
	// 中断设置
	INTCONbits.GIE = 1;
	INTCONbits.PEIE = 1;

}


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
	
	OSCCON = 0b01110000;	// ��������Ϊ8MHz, ʹ��PLL(��Ƶ)
	ANSELA = 0;
	APFCONbits.RXDTSEL = 0;	// ��RA1�趨Ϊ����
	APFCONbits.TXCKSEL = 0;	// ��RA0�趨Ϊ����
	TXSTAbits.BRGH = 1;		// ���ٲ�����
	BAUDCONbits.BRG16 = 1;	// ʹ�� 16 λ�����ʷ�����
	SPBRG = 207;
	TXSTAbits.SYNC = 0;		// �첽ģʽ
	RCSTAbits.SPEN = 1;		// ʹ���첽����
	
	// �����ж�
	/*
	INTCONbits.GIE = 1;
	INTCONbits.PEIE = 1;
	PIE1bits.TXIE = 1;
	PIR1bits.TXIF = 0;
	*/
}


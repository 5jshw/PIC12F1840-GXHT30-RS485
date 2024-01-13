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
	
	OSCCON = 0b01110000;	// ��������Ϊ8MHz, ��ֹPLL(��Ƶ)
	//OSCCON = 0b11110000;	// ��������Ϊ32MHz, ʹ��PLL(��Ƶ)
	ADCON0 = 0x10;
	ANSELA = 0;
	OPTION_REG = 0x84;
	APFCONbits.RXDTSEL = 0;	// ��RA1�趨Ϊ����
	APFCONbits.TXCKSEL = 0;	// ��RA0�趨Ϊ����
	TXSTAbits.BRGH = 1;		// ���ٲ�����
	BAUDCONbits.BRG16 = 1;	// ʹ�� 16 λ�����ʷ�����
	SPBRG = 207;			// 8MHz�� 9600
	TXSTAbits.SYNC = 0;		// �첽ģʽ
	RCSTAbits.SPEN = 1;		// ʹ���첽����
	RCSTAbits.RX9 = 0;		// �رյ�9λ����
	TXSTAbits.TX9 = 0;		// �رյ�9λ����
	TXSTAbits.TXEN = 1;		// ����ʹ��
	RCSTAbits.CREN = 1;		// ��������ʹ��
	
	// �ж�����
	INTCONbits.GIE = 1;
	INTCONbits.PEIE = 1;

}


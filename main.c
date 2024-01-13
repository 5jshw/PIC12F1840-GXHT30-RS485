#include <xc.h>
#include <stdlib.h>
#include <pic12f1840.h>
#include "rs485.h"
#include "GXHT30.h"
#include "MB1.h"

extern int Temperature;	// �¶�
extern int Humidity;	// ʪ��
extern char cOurAddrHigh, cOurAddrLow; // 485ͨ�ŵ�ַ
extern char cRs485RxChar;	// 485��ȡ����
extern char zcCommand;	// ģʽ����
char cTHValue[5];		// 485ͨ�Ų�������
char cTestc[4];			// 485ͨ�Ž�������
char State_Affairs[8];	// 0��ַ��λ��1��ַ��λ��2�����ʸ�λ��3�����ʸ�λ��4�����ʵ�λ��5�����ʵ�λ��6�¶Ȳ�����7ʪ�Ȳ���
						// 00	01	
void main(void)
{
	char cPacketReady;	// ���ݰ�׼����־
	char cLen0fData;	// ���ݰ�����
	char cCmd;			// �������� ѭ��������
	main_init();
	IIC_init();
	__delay_ms(500);
	
	while(1)
	{
		GXHT30_single_call();			// ���ζ�ȡ��ʪ��
		bit_Value();					// ��ֵת��

		Rs485Initialise(0x00, 0x01);	// ����modbusͨ��
		cPacketReady = Rs485Process();	// ���ݰ�Ч���ж�
		if(cPacketReady)
		{
			Rs485GetPacket(&cCmd, &cLen0fData, &cTestc[0]);	// �������ݰ�
			if(cLen0fData != 0)			// ���ݳ��Ȳ���Ϊ0
			{
				if(zcCommand == 1)		// ���͵�ַ
				{
					Rs485SendPacket(0xA1, 2, &cTHValue[0]);
				}
				else if(zcCommand == 2)	// ���Ͳ�����
				{
					Rs485SendPacket(0xA2, 2, &cTHValue[0]);
				}
				else if(zcCommand == 3)	// ����״̬
				{
					Rs485SendPacket(0xA3, 4, &cTHValue[0]);
				}
				else if(zcCommand == 4)	// ������ʪ������
				{
					Rs485SendPacket(0xA4, 4, &cTHValue[0]);
				}
				else	// ����
				{
					Rs485SendPacket(0xA1, 0, 0);
				}
			}
			__delay_ms(250);
			Rs485Initialise(cOurAddrHigh, cOurAddrLow);	// ����modbusͨ��
		}
	}
}

char ASCII_TH(int Value, char bit)	// ��ֵ��ִ�����
{
	if(bit == 1)
	{
		return (char)(Value % 10);
	}
	else if(bit == 2)
	{
		return (char)(Value % 100 / 10);
	}
	else
	{
		return '?';
	}
}

void bit_Value(void)	// ��ֵ��ִ��ݺ���
{
	if(Humidity == 100)	Humidity = 99;
	cTHValue[1] = ASCII_TH(Temperature, 1);
	cTHValue[0] = ASCII_TH(Temperature, 2);
	cTHValue[3] = ASCII_TH(Humidity, 1);
	cTHValue[2] = ASCII_TH(Humidity, 2);
}

void __interrupt() UsartInterruptISR(void)	// �жϴ�����
{
	while(PIR1bits.RCIF)
	{
		cRs485RxChar = RCREG;
		if(!(RCSTA & 6))	// �жϽ���֡�������������
		{
			Rs485Decode();	// ����
			RCSTAbits.CREN = 1;
		}
		else
		{
			RCSTAbits.CREN = 0;
			cRs485RxChar = RCREG;
			cRs485RxChar = RCREG;
			RCSTAbits.CREN = 1;
		}
		PIR1bits.RCIF = 0;
	}
}

void EEPROM_Write(void)
{
	EECON1bits.EEPGD = 1;
	EECON1bits.WREN = 1;
	EECON1bits.CFGS = 0;
	EECON1bits.LWLO = 1;
	

	// �Ĵ�����ַ
	EEADRH = 0x01;
	EEADRL = 0x01;
	
	// �Ĵ�������
	EEDATH = 0x00;
	EEDATL = 0x01;
	
	EECON2 = 0x55;
	EECON2 = 0xAA;
	EECON1bits.WR = 1;
	
	EECON1bits.LWLO = 0;
	
}

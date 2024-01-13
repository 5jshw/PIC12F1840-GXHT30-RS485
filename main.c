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
char cTHValue[4];		// 485ͨ�Ų�������
char cTestc[4];			// 485ͨ�Ž�������

void main(void)
{
	char cPacketReady;	// ���ݰ�׼����־
	char cLen0fData;	// ���ݰ�����
	char cCmd;			// �������� ѭ��������
	main_init();		// �����ʼ��
	IIC_init();			// IIC��ʼ��
	EEPROM_Read(0x01, cOurAddrHigh);
	EEPROM_Read(0x02, cOurAddrLow);
	if(cOurAddrHigh == 0 && cOurAddrLow == 0)
	{
		cOurAddrHigh = 0x00;
		cOurAddrLow = 0x01;
	}
	__delay_ms(500);
	
	while(1)
	{
		GXHT30_single_call();			// ���ζ�ȡ��ʪ��
		bit_Value();					// ��ֵת��
		
		Rs485Initialise(cOurAddrHigh, cOurAddrLow);	// ����modbusͨ��
		cPacketReady = Rs485Process();	// ���ݰ�Ч���ж�
		if(cPacketReady)
		{
			Rs485GetPacket(&cCmd, &cLen0fData, &cTestc[0]);	// �������ݰ�
			if(cLen0fData != 0)			// ���ݳ��Ȳ���Ϊ0
			{
				if(zcCommand == 1)		// �޸ĵ�ַ
				{
					if(cTestc[0] == 0 && cTestc[1] == 0)
					{
						Rs485SendPacket(0xC0, 1, &BAD_NADDR[0]);
						zcCommand = 0;
					}
					else
					{
						EEPROM_Write(0x01, cTestc[0]);
						EEPROM_Write(0x02, cTestc[1]);
						Rs485SendPacket(0xA1, 2, &cTHValue[0]);
						zcCommand = 0;
					}
				}
				else if(zcCommand == 2)	// �޸Ĳ�����
				{
					Rs485SendPacket(0xA2, 2, &cTHValue[0]);
					zcCommand = 0;
				}
				else if(zcCommand == 3)	// ����״̬
				{
					Rs485SendPacket(0xA3, 4, &cTHValue[0]);
					zcCommand = 0;
				}
				else if(zcCommand == 4)	// ������ʪ������
				{
					Rs485SendPacket(0xA4, 4, &cTHValue[0]);
					zcCommand = 0;
				}
				else	// ����
				{
					
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

void EEPROM_Write(char eAddr, char eDate)	// ������д��EEPROM
{
	EEADR = eAddr;		// д��ַ
	EEDAT = eDate;		// д����
	EEPGD = 0;			// ��������EEPROM�洢��
	WREN = 1;			// ������/��������
	EECON2 = 0x55;		
	EECON2 = 0xAA;
	WR = 1;				// ����д����
	while(WR == 1);		// �ȴ�д��������
	WREN = 0;			// д��ֹ
}

void EEPROM_Read(char eAddr, char eDate)	// �����ݶ���EEPROM
{
	EEADR = eAddr;	// д���ȡ���ݵĵ�ַ
	EEPGD = 0;		// ��������EEPROM�洢��
	RD = 1;			// ������λ
	while(RD == 0);	// �ȴ���ȡ���
	eDate = EEDAT;	// ������������
}


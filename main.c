#include <xc.h>
#include <stdlib.h>
#include <pic12f1840.h>
#include "rs485.h"
#include "GXHT30.h"
#include "MB1.h"

extern int Temperature;	// 温度
extern int Humidity;	// 湿度
extern char cOurAddrHigh, cOurAddrLow; // 485通信地址
extern char cRs485RxChar;	// 485读取数据
extern char zcCommand;	// 模式命令
char cTHValue[4];		// 485通信测试数据
char cTestc[4];			// 485通信接收数据

void main(void)
{
	char cPacketReady;	// 数据包准备标志
	char cLen0fData;	// 数据包长度
	char cCmd;			// 控制命令 循环计数器
	main_init();		// 程序初始化
	IIC_init();			// IIC初始化
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
		GXHT30_single_call();			// 单次读取温湿度
		bit_Value();					// 数值转换
		
		Rs485Initialise(cOurAddrHigh, cOurAddrLow);	// 重置modbus通信
		cPacketReady = Rs485Process();	// 数据包效用判断
		if(cPacketReady)
		{
			Rs485GetPacket(&cCmd, &cLen0fData, &cTestc[0]);	// 传递数据包
			if(cLen0fData != 0)			// 数据长度不能为0
			{
				if(zcCommand == 1)		// 修改地址
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
				else if(zcCommand == 2)	// 修改波特率
				{
					Rs485SendPacket(0xA2, 2, &cTHValue[0]);
					zcCommand = 0;
				}
				else if(zcCommand == 3)	// 发送状态
				{
					Rs485SendPacket(0xA3, 4, &cTHValue[0]);
					zcCommand = 0;
				}
				else if(zcCommand == 4)	// 发送温湿度数据
				{
					Rs485SendPacket(0xA4, 4, &cTHValue[0]);
					zcCommand = 0;
				}
				else	// 其他
				{
					
				}
			}
			__delay_ms(250);
			Rs485Initialise(cOurAddrHigh, cOurAddrLow);	// 重置modbus通信
		}
	}
}

char ASCII_TH(int Value, char bit)	// 数值拆分处理函数
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

void bit_Value(void)	// 数值拆分传递函数
{
	if(Humidity == 100)	Humidity = 99;
	cTHValue[1] = ASCII_TH(Temperature, 1);
	cTHValue[0] = ASCII_TH(Temperature, 2);
	cTHValue[3] = ASCII_TH(Humidity, 1);
	cTHValue[2] = ASCII_TH(Humidity, 2);
}

void __interrupt() UsartInterruptISR(void)	// 中断处理函数
{
	while(PIR1bits.RCIF)
	{
		cRs485RxChar = RCREG;
		if(!(RCSTA & 6))	// 判断接收帧错误与溢出错误
		{
			Rs485Decode();	// 解码
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

void EEPROM_Write(char eAddr, char eDate)	// 将数据写入EEPROM
{
	EEADR = eAddr;		// 写地址
	EEDAT = eDate;		// 写数据
	EEPGD = 0;			// 访问数据EEPROM存储器
	WREN = 1;			// 允许编程/擦除周期
	EECON2 = 0x55;		
	EECON2 = 0xAA;
	WR = 1;				// 启动写操作
	while(WR == 1);		// 等待写操作结束
	WREN = 0;			// 写禁止
}

void EEPROM_Read(char eAddr, char eDate)	// 将数据读出EEPROM
{
	EEADR = eAddr;	// 写入读取数据的地址
	EEPGD = 0;		// 访问数据EEPROM存储器
	RD = 1;			// 读控制位
	while(RD == 0);	// 等待读取完成
	eDate = EEDAT;	// 读出所需数据
}


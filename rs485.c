#include <xc.h>
#include <stdlib.h>
#include "rs485.h"
#include "MB1.h"

// struct
char zcCommand;
char cOurAddrHigh, cOurAddrLow; // �������������ڴ洢���������ݵ�ָ���ӻ���ַ
char cRs485RxChar; // �������������ʱ�洢��RS485���յ��ĵ����ַ�
static char cRS485State; // ���ڱ�ʾRS485���ݴ���ĵ�ǰ״̬ ��ͨ����һ��״̬����һ���� ���ڸ������ݰ�������̵Ľ���

static char cStart; // ���ڴ洢���ݰ��Ŀ�ʼ�ַ�����
static char cNetAddrHigh, cNetAddrLow; // �������������ڴ洢���յ���ָ���ӻ���ַ
static char cLenExpected; // �洢���յ������ݰ���Ԥ�ڵ����ݳ���
static char cCommand; // �洢���ݰ��е������ֽ� ��ͨ��ָ����Ҫִ�еĲ��������ݰ�������
static char c485Buf[64]; // ����һ���ַ����� ���ڴ洢���յ������ݰ������ݲ���
static char cRxCrcHigh, cRxCrcLow; // �������������ڴ洢���յ������ݰ���CRCУ��͵ĸ�λ�͵�λ
static char cCalcCrcHigh, cCalcCrcLow; // �������������ڴ洢�������CRCУ��͵ĸ�λ�͵�λ ��������յ���CRCУ��ͽ��бȽ�
static char cBufPtr; // һ��ָ�루�������� ���ڸ����� c485Buf �����е�ǰ��д��λ��
static char cError; // ���ڴ洢�������ݰ������з����Ĵ�������
static char cCrcTmp, cCrcTmp5, cCrcTmp4, cCrcTmp3, cCrcTmp2; // ��Щ��ʱ������������CRC��������е��м䲽���������ʱ����
//} RS485_Protocol;

/****************************************************************************
	void Rs485Initialise(void)
	Initialise RS485 network driver ��ʼ�� RS485 ������������
****************************************************************************/
void Rs485Initialise(char cAddrHigh, char cAddrLow) // ��ʼ�� RS485 ������������ ͬʱָ�����������ݵĴӻ���ַ
{
  cOurAddrHigh = cAddrHigh;		// �洢���������ݵ�ָ���ӻ���ַ���ֽ�
  cOurAddrLow = cAddrLow;		// �洢���������ݵ�ָ���ӻ���ַ���ֽ�
  cRS485State = PKT_WAIT_START; // ��ʼ��״̬��ָ��
  T_RO = 0;
  PIE1bits.RCIE = 1;			// ���������ж�
}

/****************************************************************************
	char PacketForUs(void)
	Decide if packet valid and destined for this node. �ж����ݰ��Ƿ���Ч���Ƿ��Ա��ڵ�ΪĿ�ĵء�
	Ignore invalid packets and packets for other nodes ������Ч���ݰ��������ڵ�����ݰ�
****************************************************************************/
char Rs485Process(void) // ���ݰ�Ч���ж�
{
	char cOurPkt, cPktReady;	// ���ݰ�״̬
	cOurPkt = FALSE;			// Ĭ��״̬
	cPktReady = FALSE;			// ����ַ���� ��Ϊ��
	INTCONbits.GIE = 0;			// �ر�ȫ���ж�
	if(cRS485State == PKT_COMPLETE) // ������ݰ��Ƿ���ѯ���
	{
		if((cNetAddrHigh == cOurAddrHigh) && (cNetAddrLow == cOurAddrLow))	// �����յĵ�ַλ�Ƿ��Ӧָ���ӻ��ĵ�ַλ
		{
			cOurPkt = TRUE;		// ����ַ��ȷ �򷵻��棬��ִ����һ��
		}
		else
		{
			cOurPkt = FALSE;	// ����ַ���� ��Ϊ��
			cPktReady = FALSE;
			__delay_ms(200);
		}
		cRS485State = PostValidatePacket();			// ��֤CRC���ݰ� ȷ�����յĵ�ַλ�����ݣ�����ȷ��
		if((cRS485State == PKT_INVALID) || (cRS485State == PKT_VALID))	// ��У�����ַλ��CRC���жϷ��ؽ���Ƿ���Ч
		{
			// �ܾ���Ч���ݰ�
			if(cRS485State == PKT_INVALID)			// �ܾ�Ӧ����Ч���ݰ�
			{
				//if(cError == BAD_CRC)	;
				//else if(cError == BAD_LENGTH)	;
				__delay_ms(200);
				if(cOurPkt)	Rs485SendPacket(SENSOR_NAK, 0, NULL);	//	������Ч���ݰ�
				cRS485State = PKT_WAIT_START;		// ��״̬��ָ�븴λ �ȴ��´ν���
			}
			else if(cRS485State == PKT_VALID)		// ������ݰ���Ч
			{										// ���ҽ��յ�ַ��ȷ
				if(cOurPkt) cPktReady = TRUE;		// ��ַ��ȷ CRCУ����ȷ ��ʾ�����ý��׼��
				else  cRS485State = PKT_WAIT_START;	// ��ַ���� ��״̬��ָ�븴λ �ȴ��´ν���
			}
		}
	}
	INTCONbits.GIE = 1;		// ����ȫ���ж�
	return cPktReady;		// ����һ��ֵ ��ʾ�Ƿ�׼���ô˴�ͨѶ
}

/****************************************************************************
	void Rs485Decode(void)
	Decode an incomming packet on the RS485 network �����485���յ������ݰ�
	Expecting: �ֽ���������
	START, ��ʼ�ź�
	NETWORK ADDRESS_HIGH, ��ַ���ֽ�
	NETWORK ADDRESS_LOW, ��ַ���ֽ�
	PAYLOAD LENGTH, ��Ч�غɳ���
	COMMAND, ��������
	optional DATA, ��ѡ����
	CRC HIGH, CRC��λ
	CRC LOW CRC��λ
****************************************************************************/
char Rs485Decode(void) // �����485���յ������ݰ�
{
	// ʹ��״̬����������յ���ÿ���ֽ�
	switch (cRS485State) // ����״̬���ĵ�ǰ״̬���н���
	{	// �ȴ����Ŀ�ʼ��־
		case PKT_WAIT_START:	// 0
		  						cStart = cRs485RxChar; // �����յ����ַ��洢�� cStart ������
								if (cRs485RxChar == PKT_START) // ����Ƿ�Ϊ���ݰ��Ŀ�ʼ�ֽ�
								{
									cRS485State++; // �ƶ�����һ��״̬
								}
								break;
		// �ȴ����������ַ�ĸ��ֽ�
		case PKT_WAIT_ADDR_HIGH:// 1
								cNetAddrHigh = cRs485RxChar; // �洢ָ���ӻ��������ĵ�ַ
								cRS485State++; // �ƶ�����һ��״̬
								break;
		// �ȴ����������ַ�ĵ��ֽ�
		case PKT_WAIT_ADDR_LOW:	// 2
								cNetAddrLow = cRs485RxChar; // �洢ָ���ӻ��������ĵ�ַ
								cRS485State++; //�ƶ�����һ��״̬
								break;
		// �ȴ��������ݳ����ֽ�
		case PKT_WAIT_LEN:		// 3
								cLenExpected = cRs485RxChar; // �洢Ԥ�ڵ����ݳ���
								if (cLenExpected > sizeof(c485Buf)) //������ݳ����Ƿ񳬹��������Ĵ�С
								{
									cRS485State = PKT_INVALID; // ��������С ���ж�Ϊ��Ч���ݰ�
									cError = BAD_LENGTH; // ���ش�������Ϊ ���ݳ��ȴ���
								}
								else
								{
									cBufPtr = 0; // �����ݳ���С�ڻ�������С �����û�����ָ��
									cRS485State++; //�ƶ�����һ��״̬
								}
								break;
		// �ȴ����տ��������ֽ�
		case PKT_CMD:			// 4
								cCommand = cRs485RxChar; // �洢���������ֽ�
								if (PacketHasPayload(cCommand) == 1)	// A1�޸ĵ�ַ
								{
									cRS485State = PKT_WAIT_DATA;
									zcCommand = 1;
								}
								else if(PacketHasPayload(cCommand) == 2) // A2�޸Ĳ�����
								{
									cRS485State = PKT_WAIT_DATA;
									zcCommand = 2;
								}
								else if(PacketHasPayload(cCommand) == 3) // A3����״̬
								{
									cRS485State = PKT_WAIT_DATA;
									zcCommand = 3;
								}
								else if(PacketHasPayload(cCommand) == 4) // A4������ʪ������
								{
									cRS485State = PKT_WAIT_DATA;
									zcCommand = 4;
								}
								else
								{
									cRS485State = PKT_WAIT_CRC_HIGH; // ���޷��ж����������� ����CRCУ��
								}
								//cRS485State++;
								break;
		// �ȴ���������
		case PKT_WAIT_DATA:		// 5
								c485Buf[cBufPtr] = cRs485RxChar; // �����յ������ݴ洢��������
								cBufPtr++; //�ƶ�������ָ��
								if (cBufPtr == cLenExpected) // �����յ����ݳ��ȴﵽԤ�����ݳ���
								{
									cRS485State++;	// �ƶ�����һ��״̬
								}
								break;
		// �ȴ�����CRC���ֽ�
		case PKT_WAIT_CRC_HIGH:	// 6
								cRxCrcHigh = cRs485RxChar; // �洢CRC���ֽ�
								cRS485State++; // �ƶ�����һ���ֽ�
								break;
		// �ȴ�����CRC���ֽ�
		case PKT_WAIT_CRC_LOW:	// 7
								cRxCrcLow = cRs485RxChar; // �洢CRC���ֽ�
								cRS485State = PKT_COMPLETE; // ��״̬������Ϊ����ɽ���״̬
								break;
		// ���ݰ��ѽ������ ���ڿ���״̬
		case PKT_COMPLETE:		// 8
								break;       // ��ִ���κβ���
		case PKT_VALID:			// 9
								break;       // ��ִ���κβ���
		case PKT_INVALID:		// 255
								break;       // ��ִ���κβ���
		// �κ���������� ������ʼ״̬
		default:
								cRS485State = PKT_WAIT_START; // ��ʼ״̬
								break;
	}
	return cRS485State; // ���ص�ǰ��״̬��״̬
}

/****************************************************************************
	void Rs485SendPacket( char cAddr, char cCmd, char cLen, char *cData )
	Send a packet over the RS485 link ͨ��RS485��·�������ݰ�
	Input: NETWORK_ADDRESS, COMMAND, PAYLOAD_LENGTH, optional DATA �����ѡ ��ַ ָ�� ���ݳ��� ����
****************************************************************************/
void Rs485SendPacket(char cCmd, char cLen, char *cData) // ͨ��RS485��·�������ݰ�
{
	char c, d;
	PIE1bits.RCIE = 0;		// �رս����ж�
	T_RO = 1;
	__delay_ms(1);			// ����ʱ��
	cCalcCrcHigh = 0xFF;	// ��λCRC
	cCalcCrcLow = 0xFF;
	// Send some NULL preamblesfopr receiving UART Ϊ���� UART ����һЩ NULL ǰ����
	for (c=0; c < NUM_TX_PREAMBLE; c++) Rs485SendChar(0x00);	// ����3�� NULL ��շ��ͻ���
	Rs485UpdateCrc(PKT_START);		// ���´����͵�����CRC
	Rs485SendChar(PKT_START);		// ������ʼ�ź�
	Rs485UpdateCrc(cOurAddrHigh);
	Rs485SendChar(cOurAddrHigh);	// ���͵�ַ��λ
	Rs485UpdateCrc(cOurAddrLow);
	Rs485SendChar(cOurAddrLow);		// ���͵�ַ��λ
	Rs485UpdateCrc(cLen);
	Rs485SendChar(cLen);			// �������ݳ���
	Rs485UpdateCrc(cCmd);
	Rs485SendChar(cCmd);			// ���Ϳ�������

	if (cLen != 0)					// �跢�����ݳ��Ȳ��õ���0
	{
		for (c = 0; c < cLen; c++)	// ���η�������
		{
			d = cData[c];			// ������ȡ����������
			Rs485UpdateCrc(d);		// ����CRC
		}
		for (c = 0; c < cLen; c++)
		{
			d = cData[c];
			Rs485SendChar(d);		// ��������
		}
	}
	Rs485SendChar(cCalcCrcHigh);	// ����CRCУ������ֽ�
	Rs485SendChar(cCalcCrcLow);		// ����CRCУ������ֽ�
	__delay_ms(1);
	T_RO = 0;
	PIE1bits.RCIE = 1;				// Enable Receive Interrupt �򿪽����ж�
}

/****************************************************************************
	void Rs485GetPacket( char *cCommand, char cLen, char *cData )
	Pass packet to main application �����ݰ����ݸ�������
****************************************************************************/
void Rs485GetPacket(char *cCom, char *cLen, char *cData) // �����ݰ����ݸ�������
{
	char c;					// ѭ������
	*cCom = cCommand;		// �����յ������ֵ��cComָ����ָ���λ��
	*cLen = cLenExpected;	// �����յ������ݳ��ȸ�ֵ��cLenָ����ָ���λ��
	for (c=0; c < cLenExpected;c++)  cData[c] = c485Buf[c]; // �����յ������ݸ�ֵ��cDataָ����ָ���λ��
	cData[cLenExpected] = 0x00;	// ������ĩβ���һ�����ַ�null ��ʾ�ַ�������
}

/*************************************************************************
	CRC16 Lookup tables (High and Low Byte) for 4 bits per iteration. CRC16 ���ұ����ֽں͵��ֽڣ���ÿ�ε��� 4 λ��
*************************************************************************/
const char CRC16_LookupHigh[16] = {
		  0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70,
		  0x81, 0x91, 0xA1, 0xB1, 0xC1, 0xD1, 0xE1, 0xF1
};	// CRC ���ұ�
const char CRC16_LookupLow[16] = {
		  0x00, 0x21, 0x42, 0x63, 0x84, 0xA5, 0xC6, 0xE7,
		  0x08, 0x29, 0x4A, 0x6B, 0x8C, 0xAD, 0xCE, 0xEF
};

/****************************************************************************
	Before each message CRC is generated, the CRC register must be ��ÿ�����ɱ��� CRC ֮ǰ������ͨ�����øú�����ʼ�� CRC �Ĵ�����
	initialised by calling this function ����ͨ�����øú�������ʼ��
****************************************************************************/
void CRC16_Init(void) // CRC��ʼ��
{
	// ���� CCITT �淶�� CRC ��ʼ��Ϊ 0xFFFF
	cCalcCrcHigh = 0xFF;
	cCalcCrcLow = 0xFF;
}

/****************************************************************************
	Process 4 bits of the message to update the CRC Value. �����ĵ� 4 λ������ CRC ֵ��
	Note that the data must be in the low nibble of val. ע�⣬���ݱ���λ�� Val �ĵ�λ��
****************************************************************************/
void CRC16_Update4Bits(char val) // ����4λCRC
{
	char t;
	// ��һ������ȡ CRC �Ĵ���������Ҫ�� 4 ������
	t = cCalcCrcHigh >> 4;	// ����λ�Ĵ��������ƶ�4λ��Ž�������
	// ����Ϣ���ݰ�λ�����ȡ�ı�����
	t = t ^ val;			// ��λ���
	// ��CRC�Ĵ�������4λ
	cCalcCrcHigh = (cCalcCrcHigh << 4) | (cCalcCrcLow >> 4);	// �ϲ�
	cCalcCrcLow = cCalcCrcLow << 4;		// ����λ�Ĵ��������ƶ�4λ
	// ���в������������ CRC ����
	cCalcCrcHigh = cCalcCrcHigh ^ CRC16_LookupHigh[t];	// ��CRC���ֽڲ��ұ�λ���
	cCalcCrcLow = cCalcCrcLow ^ CRC16_LookupLow[t];		// ��CRC���ֽڲ��ұ�λ���
}

/****************************************************************************
	Process one Message Byte to update the current CRC Value ����һ����Ϣ�ֽڣ����µ�ǰ CRC ֵ
****************************************************************************/
void Rs485UpdateCrc(char cVal) // ����CRC
{
	CRC16_Update4Bits(cVal >> 4);	// ����CRC���ֽ�
	CRC16_Update4Bits(cVal & 0x0F);	// ����CRC���ֽ�
}

/****************************************************************************
	void Rs485SendChar( char c )
	Driver level of RS485 protocol RS485 Э������������� 
	Output character on RS485 driver �� RS485 ������������ַ� 
	Include line turn around time ������·��תʱ��
****************************************************************************/
void Rs485SendChar(char c) // ����һ�ֽ�
{
	TXREG = c;						// Load data to send ���������Է���
	while (!(TXSTA & TRMT_MASK));	// Wait for TX Empty �ȴ��������
}

/****************************************************************************
	char PostValidatePacket(void)
	Verify the CRC on the last packet received ��֤����յ������ݰ��� CRC 
	Check if the CRC is correct ��� CRC �Ƿ���ȷ 
	and return the updated state as the result �������º��״̬��Ϊ�������
****************************************************************************/
char PostValidatePacket(void) // ��֤CRCУ����
{
	char c, d;
	CRC16_Init();	// ��ʼ��
	Rs485UpdateCrc(PKT_START);		// ���θ��¼��CRCУ����, ��ʼ�ź�
	Rs485UpdateCrc(cNetAddrHigh);	// ��ַ��λ
	Rs485UpdateCrc(cNetAddrLow);	// ��ַ��λ
	Rs485UpdateCrc(cLenExpected);	// Ԥ���ֽڳ���
	Rs485UpdateCrc(cCommand);		// ��������

	// У����յ�����
	if (PacketHasPayload(cCommand))			// ������ݰ�����Ч�غ�
	{								// ���� CRC �а�������
		for (c = 0; c < cLenExpected; c++)	// �����ֽ�
		{
			d = c485Buf[c];			// ���ζ�ȡ�ֽ�����
			Rs485UpdateCrc(d);		// ���ν���CRCУ��
		}
	}
	// ��� CRC �Ƿ���ȷ 
	// �������º��״̬��Ϊ�������
	if ( (cRxCrcHigh == cCalcCrcHigh)&&(cRxCrcLow == cCalcCrcLow) ) // ��֤У��CRC�����CRC�Ƿ����
	{
		cRS485State = PKT_VALID;	// ������Чֵ
	}
	else
	{
		cError = BAD_CRC;			// ����У�������
		cRS485State = PKT_INVALID;	// ������Чֵ
	}
	return cRS485State; // ���ؼ����״̬��ָʾ��
}

/****************************************************************************
	char GetPacketCmdType(void)
	Check packet command type ��������������
	Return TRUE if packet has a data payload. ������ݰ���������Ч�غɣ��򷵻� TRUE
****************************************************************************/
char PacketHasPayload(char ccCommand) // ��������������
{
	if (ccCommand == SENSOR_ADDR)
	{
		return 1;	// A1�޸ĵ�ַ
	}
	else if(ccCommand == SENSOR_BAUD)
	{
		return 2;	// A2�޸Ĳ�����
	}
	else if(ccCommand == SENSOR_MOD)
	{
		return 3;	// A3����״̬
	}
	else if(ccCommand == SENSOR_GTHV)
	{
		return 4;	// A4������ʪ������
	}
	else
	{
		return 0;	// ��������
	}
}






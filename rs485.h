
char Rs485Process(void);			// ���ݰ�Ч���ж�
void Rs485Initialise(char cAddrHigh, char cAddrLow);	// ��ʼ��
char Rs485Decode(void);			// ���
void Rs485UpdateCrc(char cVal);	// ����У����
void CRC16_Init(void);			// У�����ʼ��
void CRC16_Update4Bits(char val);	// ����У����
void Rs485SendPacket(char cCmd, char cLen, char *cData);	// �������ݰ�
void Rs485GetPacket(char *cCom, char *cLen, char *cData);	// ��ȡ���ݰ�
void Rs485SendChar(char c);		// �����ַ�
char PostValidatePacket(void);		// ������֤���ݰ�
char PacketHasPayload(char ccCommand);		// ��������������

#define FALSE 1
#define TRUE 0

// Configurables ������
#define RS485_CONTROL PORTC	// 485ͨѶ�ӿ�����
#define OUTPUTS_ON 5		// ���
#define NUM_TX_PREAMBLE 3	// ����
#define TRMT_MASK 2			// ���ڼ��TRMTλ״̬������

// Protocol defines Э�鶨��
#define PKT_START 0x02		//��ʼ�ֽ�

// States of packet decoder state machine ���ݰ�������״̬����״̬
#define PKT_WAIT_START      0	// ��ʼ�ź�
#define PKT_WAIT_ADDR_HIGH  1	// ��ַ���ֽ�
#define PKT_WAIT_ADDR_LOW   2	// ��ַ���ֽ�
#define PKT_WAIT_LEN        3	// �ֽڳ���
#define PKT_CMD             4	// ��������
#define PKT_WAIT_DATA       5	// ����
#define PKT_WAIT_CRC_HIGH   6	// У������ֽ�
#define PKT_WAIT_CRC_LOW    7	// У������ֽ�
#define PKT_COMPLETE        8	// ����
#define PKT_VALID           9	// ��Ч
#define PKT_INVALID       255	// ��Ч

// Error codes �������
#define BAD_LENGTH 1	// ���ȴ���
#define BAD_CRC    2	// У�������

// Packet types ���ݰ�����
#define SENSOR_ADDR		0xA1	// ���͵�ַ
#define SENSOR_BAUD		0xA2	// ���Ͳ�����
#define SENSOR_MOD		0xA3	// ����״̬
#define SENSOR_GTHV		0xA4	// ������ʪ������

#define SENSOR_ACK		0xB0	// Ӧ��
#define SENSOR_NAK		0xC0	// ��Ӧ��

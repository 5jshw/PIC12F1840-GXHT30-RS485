
char Rs485Process(void);			// 数据包效用判断
void Rs485Initialise(char cAddrHigh, char cAddrLow);	// 初始化
char Rs485Decode(void);			// 解包
void Rs485UpdateCrc(char cVal);	// 更新校验码
void CRC16_Init(void);			// 校验码初始化
void CRC16_Update4Bits(char val);	// 更新校验码
void Rs485SendPacket(char cCmd, char cLen, char *cData);	// 发送数据包
void Rs485GetPacket(char *cCom, char *cLen, char *cData);	// 获取数据包
void Rs485SendChar(char c);		// 发送字符
char PostValidatePacket(void);		// 发送验证数据包
char PacketHasPayload(char ccCommand);		// 检查控制命令类型

#define FALSE 1
#define TRUE 0

// Configurables 配置字
#define RS485_CONTROL PORTC	// 485通讯接口配置
#define OUTPUTS_ON 5		// 输出
#define NUM_TX_PREAMBLE 3	// 序文
#define TRMT_MASK 2			// 用于检查TRMT位状态的掩码

// Protocol defines 协议定义
#define PKT_START 0x02		//起始字节

// States of packet decoder state machine 数据包解码器状态机的状态
#define PKT_WAIT_START      0	// 起始信号
#define PKT_WAIT_ADDR_HIGH  1	// 地址高字节
#define PKT_WAIT_ADDR_LOW   2	// 地址低字节
#define PKT_WAIT_LEN        3	// 字节长度
#define PKT_CMD             4	// 控制命令
#define PKT_WAIT_DATA       5	// 数据
#define PKT_WAIT_CRC_HIGH   6	// 校验码高字节
#define PKT_WAIT_CRC_LOW    7	// 校验码低字节
#define PKT_COMPLETE        8	// 完整
#define PKT_VALID           9	// 有效
#define PKT_INVALID       255	// 无效

// Error codes 错误代码
#define BAD_LENGTH 1	// 长度错误
#define BAD_CRC    2	// 校验码错误

// Packet types 数据包类型
#define SENSOR_ADDR		0xA1	// 发送地址
#define SENSOR_BAUD		0xA2	// 发送波特率
#define SENSOR_MOD		0xA3	// 发送状态
#define SENSOR_GTHV		0xA4	// 发送温湿度数据

#define SENSOR_ACK		0xB0	// 应答
#define SENSOR_NAK		0xC0	// 非应答

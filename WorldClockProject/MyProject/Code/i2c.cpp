#include "i2c.h"

#define SCL_ON		GPIOA->BSRR	= GPIO_Pin_0
#define SCL_OFF		GPIOA->BRR	= GPIO_Pin_0

#define SDA_ON		GPIOA->BSRR	= GPIO_Pin_1
#define SDA_OFF		GPIOA->BRR	= GPIO_Pin_1
#define SDA_DATA	(GPIOA->IDR & GPIO_Pin_1)


My_i2c SoftI2c;


static void delay(void)
{
	uint16_t i;
	for (i = 0; i < 50; i++);
}

/*-------------------------------------------------------
功能	: 初始化i2c
输入	: void
返回	: void
Copyright : yitiandelan.
---------------------------------------------------------*/
uint8_t My_i2c::init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//使能或者失能APB2外设时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//最高输出速率50MHz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;		//开漏输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);					//初始化外设GPIOx寄存器
	
	SDA_ON;
	SCL_ON;

	return flag;
}
/*-------------------------------------------------------
功能	: 读多个字节
输入	: void
返回	: void
Copyright : yitiandelan.
---------------------------------------------------------*/
uint8_t My_i2c::read_nbyte(uint8_t sla, uint8_t suba, uint8_t *s, uint8_t no)
{
	uint8_t i;

	SoftI2c.start();		/*启动总线*/
	SoftI2c.write(sla);		/*发送器件地址*/
	//	if(i2c.ack_flag==0)return(0);
	SoftI2c.write(suba);	/*发送器件子地址*/
	//	if(i2c.ack_flag==0)return(0);
	SoftI2c.start();
	SoftI2c.write(sla + 1);
	//	if(i2c.ack_flag==0)return(0);
	for (i = 0; i<no - 1; i++)
	{
		*s = SoftI2c.read();	/*发送数据*/
		SoftI2c.ack(1);			/*发送就答位*/
		s++;
	}
	*s = SoftI2c.read();
	SoftI2c.ack(0);
	SoftI2c.stop();

	return(1);
}
/*-------------------------------------------------------
功能	: 写多个字节
输入	: void
返回	: void
Copyright : yitiandelan.
---------------------------------------------------------*/
uint8_t My_i2c::write_nbyte(uint8_t sla, uint8_t suba, uint8_t *s, uint8_t no)
{
	uint8_t i;

	SoftI2c.start();
	SoftI2c.write(sla);
	if (SoftI2c.ack(1) == 0)
		return(0);
	SoftI2c.write(suba);
	if (SoftI2c.ack(1) == 0)
		return(0);

	for (i = 0; i<no; i++)
	{
		SoftI2c.write(*s);
		if (SoftI2c.ack(1) == 0)
			return(0);
		s++;
	}
	SoftI2c.stop();

	return(1);
}

/*-------------------------------------------------------
功能	: 读1字节
输入	: void
返回	: void
Copyright : yitiandelan.
---------------------------------------------------------*/
uint8_t My_i2c::read(void)
{
	uint8_t retc = 0;
	uint8_t BitCnt;

	SDA_ON;

	for (BitCnt = 0; BitCnt < 8; BitCnt++)
	{
		delay();
		SCL_OFF;		/*置时钟线为低，准备接收数据位*/
		delay();

		SCL_ON;			/*置时钟线为高使数据线上数据有效*/
		delay();

		retc <<= 1;
		if (SDA_DATA)
			retc++;		/*读数据位,接收的数据位放入retc 中 */
		delay();
	}
	SCL_OFF;
	delay();

	return retc;
}
/*-------------------------------------------------------
功能	: 写1字节
输入	: void
返回	: void
Copyright : yitiandelan.
---------------------------------------------------------*/
uint8_t My_i2c::write(uint8_t data)
{
	uint8_t i;

	for (i = 0; i<8; i++) /*要传送的数据长度为8 位*/
	{
		if ((data << i) & 0x80)
			SDA_ON; /*判断发送位*/
		else
			SDA_OFF;
		delay();

		SCL_ON;
		delay();

		SCL_OFF;
		delay();
	}
	SCL_ON;
	delay();
	
	if (SDA_DATA)
		flag = 0;
	else
		flag = 1; /*判断是否接收到应答信号*/
	delay();

	SCL_OFF;
	delay();

	return flag;
}

/*-------------------------------------------------------
功能	: 开始信号
输入	: void
返回	: void
Copyright : yitiandelan.
---------------------------------------------------------*/
uint8_t My_i2c::start(void)
{
	SDA_ON;
	delay();

	SCL_ON;
	delay();

	SDA_OFF;
	delay();

	SCL_OFF;
	delay();

	return flag;
}
/*-------------------------------------------------------
功能	: 结束信号
输入	: void
返回	: void
Copyright : yitiandelan.
---------------------------------------------------------*/
uint8_t My_i2c::stop(void)
{
	SDA_OFF;
	delay();

	SCL_ON;
	delay();

	SDA_ON;
	delay();

	return flag;
}
/*-------------------------------------------------------
功能	: 应答信号
输入	: void
返回	: void
Copyright : yitiandelan.
---------------------------------------------------------*/
uint8_t My_i2c::ack(uint8_t ceck)
{
	if (ceck)
		SDA_OFF;
	else
		SDA_ON;
	delay();

	SCL_ON;
	delay();

	SCL_OFF;
	delay();

	return flag;
}
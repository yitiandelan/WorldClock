#include "usart.h"

My_USART ttyS0;


/*-------------------------------------------------------
功能	: 初始化uart
输入	: void
返回	: void
Copyright : yitiandelan.
---------------------------------------------------------*/
void My_USART::init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	{
		GPIO_InitTypeDef GPIO_InitStructure;

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;			//UART2 TX
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//最高输出速率50MHz
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		//复用推挽输出
		GPIO_Init(GPIOA, &GPIO_InitStructure);				//初始化外设GPIOx寄存器

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;			//UART2 RX
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;		//上拉输入
		GPIO_Init(GPIOA, &GPIO_InitStructure);				//初始化外设GPIOx寄存器
	}
	{
		USART_InitTypeDef USART_InitStructure;
		USART_InitStructure.USART_BaudRate = 9600;						//波特率
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//8位数据
		USART_InitStructure.USART_StopBits = USART_StopBits_1;			//在帧结尾传输1个停止位
		USART_InitStructure.USART_Parity = USART_Parity_No;				//无校验
		USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;	//发送和接收都使能
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//硬件流控制失能
		USART_Init(USART2, &USART_InitStructure);

	}
	{
		DMA_InitTypeDef DMA_InitStructure;

		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

		DMA_DeInit(DMA1_Channel6);
		DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(USART2->DR));	//外设基地址
		DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)Rxbuf;					//内存基地址
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;						//外设作为数据传输的来源
		DMA_InitStructure.DMA_BufferSize = RxbufNum;							//指定DMA通道的DMA缓存的大小,单位为数据单位
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;		//外设地址寄存器不变
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;					//内存地址寄存器递增
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//数据宽度为8位
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;			//数据宽度为8位
		DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;							//工作在循环缓存模式
		DMA_InitStructure.DMA_Priority = DMA_Priority_Low;						//低优先级
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;							//非内存到内存传输
		DMA_Init(DMA1_Channel6, &DMA_InitStructure);							//初始化DMA

		DMA_DeInit(DMA1_Channel7);
		DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(USART2->DR));	//外设基地址
		DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)Txbuf;					//内存基地址
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;						//外设作为数据传输的目的地
		DMA_InitStructure.DMA_BufferSize = 0;									//指定DMA通道的DMA缓存的大小,单位为数据单位
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;		//外设地址寄存器不变
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;					//内存地址寄存器递增
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//数据宽度为8位
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;			//数据宽度为8位
		DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;							//工作在正常缓存模式
		DMA_InitStructure.DMA_Priority = DMA_Priority_Low;						//低优先级
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;							//非内存到内存传输
		DMA_Init(DMA1_Channel7, &DMA_InitStructure);							//初始化DMA
	}

	DMA_Cmd(DMA1_Channel6, ENABLE);												//使能DMA
	DMA_Cmd(DMA1_Channel7, ENABLE);												//使能DMA
	USART_DMACmd(USART2, USART_DMAReq_Rx | USART_DMAReq_Tx, ENABLE);			//使能或者失能指定USART的DMA请求
	USART_Cmd(USART2, ENABLE);													//使能usart2
//	USART_ClearFlag(USART2, USART_FLAG_TC);										//清除标志位
}
/*-------------------------------------------------------
功能	: 读出数据
输入	:
返回	: 读出个数
Copyright : yitiandelan.
---------------------------------------------------------*/
uint8_t My_USART::read(void)
{
	uint8_t bufNum = 0;

	DMA1_Channel6->CCR &= ~(1 << 0);				//关闭DMA通道
	
	bufNum = RxbufNum - (DMA1_Channel6->CNDTR);		//已存在缓冲区 但未读出的字节数

	DMA1_Channel6->CNDTR = RxbufNum;				//传输长度
	DMA1_Channel6->CMAR = (uint32_t)Rxbuf;			//缓存基地址

	DMA1_Channel6->CCR |= (1 << 0);					//使能DMA通道
	
	return bufNum;
}
/*-------------------------------------------------------
功能	: 计算缓冲区未读字节数
输入	:
返回	: 未读个数
Copyright : yitiandelan.
---------------------------------------------------------*/
uint8_t My_USART::Haskey(void)
{
	uint8_t bufNum = 0;

//	DMA1_Channel6->CCR &= ~(1 << 0);				//关闭DMA通道

	bufNum = RxbufNum - (DMA1_Channel6->CNDTR);		//已存在缓冲区 但未读出的字节数

	//DMA1_Channel6->CNDTR = RxbufNum;				//传输长度
	//DMA1_Channel6->CMAR = (uint32_t)Rxbuf;			//缓存基地址

	//DMA1_Channel6->CCR |= (1 << 0);					//使能DMA通道

	return bufNum;
}
/*-------------------------------------------------------
功能	: 发送数据
输入	: 发送数据的指针，发送的字节数
返回	: 0:成功 1:失败
Copyright : yitiandelan.
---------------------------------------------------------*/
uint8_t My_USART::write(const char *SendData, uint8_t Len)
{
	uint8_t i;

	if (Len >= TxbufNum)
		Len = TxbufNum - 1;

	for (i = 0; i < Len; i++)
		Txbuf[i] = *(SendData + i);
	
	DMA1_Channel7->CCR &= ~(1 << 0);		//关闭DMA通道
	DMA1_Channel7->CNDTR = Len;				//传输长度
	DMA1_Channel7->CCR |= (1 << 0);			//使能DMA通道

	//USART_SendData(USART2, Txbuf[0]);

	return 0;
}
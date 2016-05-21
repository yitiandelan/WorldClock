#include "LED.h"

#include "rtc.h"
#include "lm75.h"

My_Led Led;


/*	开辟显存	( 12 x 7Byte ) x 64																										*/
/*	第一帧																		第二帧								...		第n帧		*/
/*	p1[0,0]		p1[0,1]		p1[0,2]		p1[0,3]		....		p1[0,11]	;	p2[0,0]		p2[0,1]		p2[0,2]		...		pn[0,11]	*/
/*	p1[1,0]		p1[1,1]		p1[1,2]		p1[1,3]		....		p1[1,11]	;	p2[1,0]		p2[1,1]		p2[1,2]		...		pn[1,11]	*/
/*		.			.			.			.		....			.		;		.			.			.		...			.		*/
/*		.			.			.			.		....			.		;		.			.			.		...			.		*/
/*		.			.			.			.		....			.		;		.			.			.		...			.		*/
/*		.			.			.			.		....			.		;		.			.			.		...			.		*/
/*	p1[7,0]		p1[7,1]		p1[7,2]		p1[7,3]		....		p1[7,11]	;	p2[7,0]		p2[7,1]		p2[7,2]		...		pn[7,11]	*/
uint8_t	disBuf[disbufNum][12][7];

/*当前颜色*/
//uint8_t PiontRGB = 0x08 | 0x04;

/*输出使能*/
uint16_t disMask[12] = { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, };


const uint16_t MaskList[][12] =
{
	0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,		//no mask
	0x0000, 0x0070, 0x00f8, 0x01f8, 0x03f8, 0x07f0, 0x07f0, 0x03f8, 0x01f8, 0x00f8, 0x0070, 0x0000,		//love
	0x0000, 0x0000, 0x00e8, 0x0128, 0x0220, 0x0220, 0x0220, 0x0220, 0x0128, 0x00e8, 0x0000, 0x0000,		//:D
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0100, 0x0f80, 0x0900, 0x0000, 0x0f80, 0x0200, 0x0e00,		//th
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,		//null
};
const uint8_t Num_5x7_Mask[][6] =
{
	0x7C, 0x82, 0x82, 0x82, 0x7C, 0x00,		/*"0",0*/
	0x00, 0x04, 0x04, 0xFE, 0x00, 0x00,		/*"1",1*/
	0xC4, 0xa2, 0x92, 0x92, 0x8C, 0x00,		/*"2",2*/
	0x44, 0x82, 0x92, 0x92, 0x6C, 0x00,		/*"3",3*/
	0x30, 0x28, 0x24, 0xFE, 0x20, 0x00,		/*"4",4*/
	0x4E, 0x8A, 0x8A, 0x8A, 0x72, 0x00,		/*"5",5*/
	0x7C, 0x92, 0x92, 0x92, 0x64, 0x00,		/*"6",6*/
	0x02, 0xC2, 0x32, 0x0A, 0x06, 0x00,		/*"7",7*/
	0x6C, 0x92, 0x92, 0x92, 0x6C, 0x00,		/*"8",8*/
	0x4C, 0x92, 0x92, 0x92, 0x7C, 0x00,		/*"9",9*/
};

/*列寻址*/
uint16_t disMap[12] = { 0x0010, 0x0008, 0x0004, 0x0002, 0x0020, 0x8000, 0x4000, 0x2000, 0x1000, 0x0800, 0x0400, 0x0200 };

void SetPiontflag(uint8_t x, uint8_t y, uint8_t flag)
{
	if (flag)
		disMask[x] |= (1 << y);
	else
		disMask[x] &= ~(1 << y);
}

void SetPiontRGB(uint8_t R, uint8_t G, uint8_t B)
{
	Led.RGB = 0;
	if (R)
		Led.RGB |= 0x04;
	if (G)
		Led.RGB |= 0x08;
	if (B)
		Led.RGB |= 0x02;
}

void SetMask(uint8_t num)
{
	uint8_t i;
	for (i = 0; i < 12; i++)
		disMask[i] = MaskList[num][i];
}

void SetNumMask(uint8_t x, uint8_t y, uint8_t num)
{
	uint8_t i;

	if (num >= 10)
		num = 0;

	for (i = 0; i < 5; i++)
		disMask[i + x] |= Num_5x7_Mask[num][i] << y;
}

void SetNum(uint8_t num)
{
	//ReALLPiont();
	if (num >= 100)
		num = 0;

	if (num >= 10)
	{
		SetNumMask(0, 4, num / 10);
		SetNumMask(6, 4, num % 10);
	}
	else
	{
		SetNumMask(6, 4, num);
	}
}
void Setday(uint8_t num)
{
	switch (num)
	{
	case 0:
		SetPiontflag(3, 2, 1);
		SetPiontflag(4, 2, 1);
		SetPiontflag(5, 2, 1);
		break;
	case 1:
		SetPiontflag(1, 1, 1);
		SetPiontflag(2, 1, 1);
		SetPiontflag(3, 1, 1);
		break;
	case 2:
		SetPiontflag(6, 0, 1);
		SetPiontflag(7, 0, 1);
		SetPiontflag(8, 0, 1);
		SetPiontflag(9, 0, 1);
		break;
	case 3:
		SetPiontflag(7, 2, 1);
		SetPiontflag(8, 2, 1);
		SetPiontflag(9, 2, 1);
		break;
	case 4:
		SetPiontflag(4, 1, 1);
		SetPiontflag(5, 1, 1);
		SetPiontflag(6, 1, 1);
		SetPiontflag(7, 1, 1);
		break;
	case 5:
		SetPiontflag(8, 1, 1);
		SetPiontflag(9, 1, 1);
		SetPiontflag(10, 1, 1);
		break;
	case 6:
		SetPiontflag(4, 0, 1);
		SetPiontflag(5, 0, 1);
		SetPiontflag(6, 0, 1);
		break;
	default:
		break;
	}
}
void ReALLPiont(void)
{
	uint8_t i;
	for (i = 0; i < 12; i++)
		disMask[i] = 0;
}
void TheMatrix(void)
{
	uint8_t listNum;
	uint8_t fpsNum;

	for (fpsNum = 0; fpsNum < disbufNum; fpsNum++)
	for (listNum = 0; listNum < 12; listNum++)
	{
		disBuf[fpsNum][listNum][0] = disMap[listNum] >> 8;
		disBuf[fpsNum][listNum][1] = disMap[listNum] & 0xff;

		{
			uint32_t RGB;
			uint8_t i;
			uint8_t PiontColor;


			/*灰度等级*/
			//if (Led.Grad > disbufNum)
			//	Led.Grad = disbufNum;

			///*小于则显示颜色*/
			//if (fpsNum < Led.Grad)
			//{
				/*如果为奇数列*/
				if (listNum & 0x01)
				{
					PiontColor = 0x02;
				}
				else
				{
					PiontColor = 0x08;
				}
			//}
			//else
			//	PiontColor = 0;


			RGB = 0;

			for (i = 0; i < 8; i++)
			{
				RGB >>= 3;
				//if (disMask[listNum] & (1 << i))
					RGB |= (PiontColor << 20);
			}

			disBuf[fpsNum][listNum][4] = (RGB & 0x00ff0000) >> 16;
			disBuf[fpsNum][listNum][5] = (RGB & 0x0000ff00) >> 8;
			disBuf[fpsNum][listNum][6] = RGB & 0x000000ff;

			RGB = 0;

			//for (i = 0; i < 4; i++)
			//{
			//	RGB >>= 3;
			//	if (disMask[listNum] & ((1 << 8) << i))
			//		RGB |= (PiontColor << 8);
			//}

			disBuf[fpsNum][listNum][2] = (RGB & 0x0000ff00) >> 8;
			disBuf[fpsNum][listNum][3] = RGB & 0x000000ff;
		}
	}
}
void SetPiont(void)
{
	/*计算 hex*/
	uint8_t listNum;
	uint8_t fpsNum;

	for (fpsNum = 0; fpsNum < disbufNum;fpsNum++)
		for (listNum = 0; listNum < 12; listNum++)
		{
			disBuf[fpsNum][listNum][0] = disMap[listNum] >> 8;
			disBuf[fpsNum][listNum][1] = disMap[listNum] & 0xff;

			{
				uint32_t RGB;
				uint8_t i;
				uint8_t PiontColor;
				
				
				/*灰度等级*/
				if (Led.Grad > disbufNum)
					Led.Grad = disbufNum;

				/*小于则显示颜色*/
				if (fpsNum < Led.Grad)
				{
					/*如果为奇数列*/
					if (listNum & 0x01)
					{
						PiontColor = Led.RGB & 0x04;	//R
						PiontColor |= (Led.RGB & 0x08) >> 2;//G
						PiontColor |= (Led.RGB & 0x02) << 2;//B
					}
					else
					{
						PiontColor = Led.RGB;
					}
				}
				else
					PiontColor = 0;


				RGB = 0;

				for (i = 0; i < 8; i++)
				{
					RGB >>= 3;
					if (disMask[listNum] & (1 << i))
						RGB |= (PiontColor << 20);
				}

				disBuf[fpsNum][listNum][4] = (RGB & 0x00ff0000) >> 16;
				disBuf[fpsNum][listNum][5] = (RGB & 0x0000ff00) >> 8;
				disBuf[fpsNum][listNum][6] = RGB & 0x000000ff;

				RGB = 0;

				for (i = 0; i < 4; i++)
				{
					RGB >>= 3;
					if (disMask[listNum] & ((1 << 8) << i))
						RGB |= (PiontColor << 8);
				}

				disBuf[fpsNum][listNum][2] = (RGB & 0x0000ff00) >> 8;
				disBuf[fpsNum][listNum][3] = RGB & 0x000000ff;
			}
		}
}
void disPiont(void)
{
	static uint16_t listNum = 0;
	static bool flag = 1;

	if (flag)
	{
		listNum += 7;
		if (listNum >= (12 * 7 * disbufNum))
			listNum = 0;
	
		DMA1_Channel3->CCR &= ~(1 << 0);		//关闭DMA通道
		DMA1_Channel3->CMAR = (uint32_t)disBuf + listNum;
		DMA1_Channel3->CNDTR = 7;				//传输长度
		DMA1_Channel3->CCR |= (1 << 0);			//使能DMA通道
	}
	else
	{
		/*更新数据*/
		OCK_Set;
		OCK_Reset;
	}
	
	flag = !flag;

//	while (!DMA_GetFlagStatus(DMA1_FLAG_TC3));
//	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);

	/*更新数据*/
	//OCK_Reset;
	//OCK_Set;
	//OCK_Reset;
}

void Set_LED_Mode(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;			//复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;				//SPI_MISO
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;				//SPI_CS
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		//推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//GPIO_SetBits(GPIOA, GPIO_Pin_4);

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;		//设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;							//设置SPI工作模式:设置为主SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;						//设置SPI的数据大小:SPI发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;					//选择了串行时钟的稳态:时钟悬空高
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;				//数据捕获于第二个时钟沿
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;					//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI1CLK;		//定义波特率预分频的值:波特率预分频值为256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;						//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;								//CRC值计算的多项式
	SPI_Init(SPI1, &SPI_InitStructure);										//根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器

	{
		DMA_InitTypeDef DMA_InitStructure;

		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

		DMA_DeInit(DMA1_Channel3);
		DMA_InitStructure.DMA_PeripheralBaseAddr = 0x4001300C;                          //设置  接收外设(0x4001300C) 地址(源地址)
		DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)disBuf;                    //设置 SRAM 存储地址(源地址)
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                                //传输方向 内存-外设
		DMA_InitStructure.DMA_BufferSize = 7;                           //设置 SPI1 接收长度
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;                  //外设地址增量(不变)
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                           //内存地址增量(变化)
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;           //外设传输宽度(字节)
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;                   //内存传输宽度(字节)
		DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                                     //传输方式,一次传输完停止,不重新加载
		DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;                           //中断方式-高(三级)
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                                      //内存到内存方式禁止
		DMA_Init(DMA1_Channel3, &DMA_InitStructure);

		DMA_ITConfig(DMA1_Channel3, DMA_IT_TC, ENABLE);                                   //开启 DMA1_Channel3 传输完成中断

		SPI1->CR2 |= 1 << 1;                                                                //发送缓冲区DMA使能
		DMA_Cmd(DMA1_Channel3, DISABLE);                                                  //开启 DMA 通道 DMA1_Channel3
	}

	SPI_Cmd(SPI1, ENABLE);		//使能SPI外设

	{
		NVIC_InitTypeDef NVIC_InitStructure;

		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
		NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
	}
	{
		TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;

		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

		TIM_DeInit(TIM3);

		TIM_TimeBaseStructure.TIM_Period = disfps - 1;       							//定时周期  
		TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1;       							//预分频，不分频 72M / (0+1) = 72M
		TIM_TimeBaseStructure.TIM_ClockDivision = 0x00;    								//时钟分频系数
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  					//向上计数模式
		TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

		TIM_ClearFlag(TIM3, TIM_FLAG_Update);
		TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

		TIM_Cmd(TIM3, ENABLE);
	}
	
}

/*-------------------------------------------------------
功能	: 初始化led
输入	: void
返回	: void
Copyright : yitiandelan.
---------------------------------------------------------*/
void My_Led::init(void)
{
	ReALLPiont();
	SetPiont();
	Set_LED_Mode();

	MenuNum = 0;
	RGB = 0;

	Set2Num = 1;
	Set2RGB = Red | Green;

	Grad = 8;
}
/*-------------------------------------------------------
功能	: 改变led界面
输入	: void
返回	: void
Copyright : yitiandelan.
---------------------------------------------------------*/
void My_Led::Switch(void)
{

}

/*-------------------------------------------------------
功能	: 刷新Mask
输入	: void
返回	: void
Copyright : yitiandelan.
---------------------------------------------------------*/
void My_Led::ReMask(void)
{
	switch (Led.MenuNum)
	{
	case NoMask:
		ReALLPiont();
		break;
	case ShowClock:
		ReALLPiont();
		//rtc_timer.read();
		Setday(rtc_timer.NowTime.tm_wday);
		SetNum(rtc_timer.NowTime.tm_mday);
		break;
	case ShowLove:
		ReALLPiont();
		SetMask(1);
		break;
	case Showhappy:
		ReALLPiont();
		SetMask(2);
		break;
	case ShowTemperature:
		ReALLPiont();
		SetNum(lm75.NowTemper);
		break;
	case HappyBirthday:
		ReALLPiont();
		SetPiontflag(0, 4, 1);	SetPiontflag(1, 4, 1);	SetPiontflag(2, 4, 1);	SetPiontflag(3, 4, 1);	SetPiontflag(4, 4, 1);	//happy			
		SetPiontflag(0, 7, 1);	SetPiontflag(1, 7, 1);	SetPiontflag(2, 7, 1);	SetPiontflag(3, 7, 1);	SetPiontflag(4, 7, 1);	//Birthday
		SetPiontflag(5, 7, 1);	SetPiontflag(6, 7, 1);	SetPiontflag(7, 7, 1);

		break;
	case NewYear:
		ReALLPiont();
		SetPiontflag(0, 4, 1);	SetPiontflag(1, 4, 1);	SetPiontflag(2, 4, 1);	SetPiontflag(3, 4, 1);	SetPiontflag(4, 4, 1);	//happy	
		SetPiontflag(2, 6, 1);	SetPiontflag(3, 6, 1);	SetPiontflag(4, 6, 1);													//new
		SetPiontflag(7, 6, 1);	SetPiontflag(8, 6, 1);	SetPiontflag(9, 6, 1);	SetPiontflag(10, 6, 1);							//year	
		break;
	case CHrist:
		ReALLPiont();
		SetPiontflag(0, 5, 1);	SetPiontflag(1, 5, 1);	SetPiontflag(2, 5, 1);	SetPiontflag(3, 5, 1);	SetPiontflag(4, 5, 1);	//merry
		SetPiontflag(0, 8, 1);	SetPiontflag(1, 8, 1);	SetPiontflag(2, 8, 1);	SetPiontflag(3, 8, 1);	SetPiontflag(4, 8, 1);	//christmas
		SetPiontflag(5, 8, 1);	SetPiontflag(6, 8, 1);	SetPiontflag(7, 8, 1);	SetPiontflag(8, 8, 1);
		break;
	case ShowthNum:
		ReALLPiont();
		SetMask(3);												//th
		SetNumMask(0, 4, rtc_timer.NowTime.tm_year - 113);		//num
		break;
	case GoodNight:
		ReALLPiont();
		SetPiontflag(2, 9, 1);	SetPiontflag(3, 9, 1);	SetPiontflag(4, 9, 1);	SetPiontflag(5, 9, 1);							//good
		SetPiontflag(2, 11, 1);	SetPiontflag(3, 11, 1);	SetPiontflag(4, 11, 1);	SetPiontflag(5, 11, 1);	SetPiontflag(6, 11, 1);	//night
		break;
	case ShowCopyright:
		ReALLPiont();
		SetPiontflag(7, 4, 1);	SetPiontflag(8, 4, 1);	SetPiontflag(9, 4, 1);	SetPiontflag(10, 4, 1);	SetPiontflag(11, 4, 1);	//Clock
		SetPiontflag(8, 7, 1);	SetPiontflag(9, 7, 1);	SetPiontflag(10, 7, 1);	SetPiontflag(11, 7, 1);							//RevA	
		break;
	case ShowName:
		ReALLPiont();
		SetPiontflag(0, 0, 1);	SetPiontflag(1, 0, 1);	SetPiontflag(3, 0, 1);	SetPiontflag(4, 0, 1);							//it is
		SetPiontflag(3, 3, 1);	SetPiontflag(4, 3, 1);	SetPiontflag(5, 3, 1);	SetPiontflag(6, 3, 1);	SetPiontflag(7, 3, 1);	//tianlan
		SetPiontflag(8, 3, 1);	SetPiontflag(9, 3, 1);
		SetPiontflag(5, 6, 1);	SetPiontflag(6, 6, 1);																			//to
		SetPiontflag(8, 11, 1);	SetPiontflag(9, 11, 1);	SetPiontflag(10, 11, 1); SetPiontflag(11, 11, 1);						//zeze
		break;
	case ShowHello:
		ReALLPiont();
		SetPiontflag(1, 3, 1);	SetPiontflag(2, 3, 1);																			//hi
		SetPiontflag(8, 11, 1);	SetPiontflag(9, 11, 1);	SetPiontflag(10, 11, 1); SetPiontflag(11, 11, 1);						//zeze
		break;
	}
}

extern "C"
{
	void TIM3_IRQHandler(void)
	{
		TIM_ClearITPendingBit(TIM3, TIM_FLAG_Update);
		disPiont();
	}
}



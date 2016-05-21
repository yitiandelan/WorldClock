#ifndef __LED_H
#define __LED_H

#include "stm32f10x.h"

#define OCK_Set		GPIOA->BSRR = GPIO_Pin_4
#define OCK_Reset	GPIOA->BRR  = GPIO_Pin_4

#define DCK_Set		GPIOA->BSRR = GPIO_Pin_5
#define DCK_Reset	GPIOA->BRR  = GPIO_Pin_5

#define DIN_Set		GPIOA->BSRR = GPIO_Pin_7
#define DIN_Reset	GPIOA->BRR  = GPIO_Pin_7

/*显示刷新率 2 * 1/12 fps time(us)*/
#define disfps	16
/*SPI分频系数*/
#define SPI1CLK SPI_BaudRatePrescaler_2
/*颜色深度*/
#define disbufNum	8

#define NoMask			0	//null	
#define ShowClock		1	//时钟
#define ShowLove		2	//love
#define Showhappy		3	//happy
#define ShowTemperature	4	//温度
#define HappyBirthday	5	//生日快乐
#define NewYear			6	//新年快乐
#define CHrist			7	//圣诞节快乐
#define GoodNight		8	//晚安
#define ShowthNum		9	//周年纪念日
#define ShowCopyright	10	//版权信息
#define ShowName		11	//名字
#define ShowHello		12	//hi


//#define Blue	0x02
//#define Red		0x04
//#define Green	0x08

class My_Led
{
public:
	uint8_t	MenuNum;
	uint8_t Set2Num;
	uint8_t	RGB;
	uint8_t	Set2RGB;
	uint8_t Grad;

	void init(void);	//初始化led
	void Switch(void);	//
	void ReMask(void);	//更新Mask
private:

};

typedef enum LedColour
{
	Blue = 0x02,
	Red	= 0x04,
	Green = 0x08,
};

extern My_Led Led;

void Set_LED_Mode(void);
void SetPiont(void);
void disPiont(void);

void SetMask(uint8_t num);
void SetNumMask(uint8_t x, uint8_t y, uint8_t num);
void SetNum(uint8_t num);
void Setday(uint8_t num);

void SetPiontflag(uint8_t x, uint8_t y, uint8_t flag);
void SetPiontRGB(uint8_t R, uint8_t G, uint8_t B);
void ReALLPiont(void);

void TheMatrix(void);

#endif

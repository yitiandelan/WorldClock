#include "lm75.h"

My_LM75 lm75;

/*-------------------------------------------------------
功能	: 初始化lm75
输入	: void
返回	: void
Copyright : yitiandelan.
---------------------------------------------------------*/
void My_LM75::init(void)
{
	SoftI2c.init();
}
/*-------------------------------------------------------
功能	: 初始化lm75
输入	: void
返回	: void
Copyright : yitiandelan.
---------------------------------------------------------*/
void My_LM75::read(void)
{
	uint8_t hexbuf[2];
	int16_t temp = 0;

	SoftI2c.read_nbyte(0x90, 0, hexbuf, 2);

	temp |= hexbuf[0] << 8;
	temp |= hexbuf[1];

	NowTemper = temp / 256;
}



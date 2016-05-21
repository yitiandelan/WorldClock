#ifndef __lm75_H
#define __lm75_H

#include "stm32f10x.h"
#include "i2c.h"

class My_LM75
{
public:
	int8_t NowTemper;
	void init(void);
	void read(void);
private:
	
};

extern My_LM75 lm75;

#endif

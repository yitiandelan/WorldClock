#ifndef __i2c_H
#define __i2c_H

#include "stm32f10x.h"


class My_i2c
{
public:
	uint8_t init(void);/*初始化i2c接口*/
	uint8_t read(void);
	uint8_t write(uint8_t data);
	uint8_t start(void);
	uint8_t stop(void);
	uint8_t ack(uint8_t ceck);
	uint8_t read_nbyte(uint8_t sla, uint8_t suba, uint8_t *s, uint8_t no);
	uint8_t write_nbyte(uint8_t sla, uint8_t suba, uint8_t *s, uint8_t no);
private:
	uint8_t flag = 0;
};

extern My_i2c SoftI2c;

#endif

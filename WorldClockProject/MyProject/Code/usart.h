#ifndef __usart_H
#define __usart_H

#include "stm32f10x.h"

#define TxbufNum	30
#define RxbufNum	180

class My_USART
{
public:
	uint8_t Txbuf[TxbufNum];
	uint8_t Rxbuf[RxbufNum];

	void init(void);
	uint8_t read(void);
	uint8_t Haskey(void);
	uint8_t write(const char *SendData, uint8_t Len);
private:

};

extern My_USART ttyS0;

#endif

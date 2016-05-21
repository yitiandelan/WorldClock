#ifndef __RTC_H
#define __RTC_H

#include "stm32f10x.h"
#include "time.h"

class My_rtc
{
public:
	tm		NowTime;	//Calendar格式日期
	time_t	NowTime_x;	//Unix格式日期

	void init(void);	//初始化rtc
	void read(void);	//读rtc时间
	void write(void);	//写rtc时间
private:

};

extern My_rtc rtc_timer;

#endif
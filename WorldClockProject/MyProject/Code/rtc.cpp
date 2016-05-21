#include "rtc.h"

My_rtc rtc_timer;

/*-------------------------------------------------------
功能	: 初始化rtc
输入	: void
返回	: void
Copyright : yitiandelan.
---------------------------------------------------------*/
void My_rtc::init(void)
{
	if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
	{
		//重新配置RTC
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
		PWR_BackupAccessCmd(ENABLE);				//备份域解锁
		BKP_DeInit();
		RCC_LSEConfig(RCC_LSE_ON);
		while (!RCC_GetFlagStatus(RCC_FLAG_HSERDY));//设置后需要等待启动

		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);		//选择LSE为RTC设备 的 时钟
		RCC_RTCCLKCmd(ENABLE);						//使能RTC
		RTC_WaitForSynchro();						//等待同步

		//RTC_WaitForLastTask();

		RTC_SetPrescaler(32768); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1)*/
		RTC_WaitForLastTask();						//等待操作结束

		//RTC_ITConfig(RTC_IT_SEC, ENABLE);			//配置秒中断
		//RTC_WaitForLastTask();					//等待操作结束

		//配置完成后，向后备寄存器中写特殊字符0xA5A5
		BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
	}
	else
	{
		RCC_ClearFlag();							//清除RCC复位标志

		RCC_RTCCLKCmd(ENABLE);						//使能RTC
		RTC_WaitForSynchro();						//等待同步

		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
		PWR_BackupAccessCmd(ENABLE);				//备份域解锁

		//RTC_ITConfig(RTC_IT_SEC, ENABLE);			//配置秒中断
		//RTC_WaitForLastTask();					//等待操作结束
	}

	RCC_ClearFlag();
}
/*-------------------------------------------------------
功能	: 读rtc时间
输入	: void
返回	: void
Copyright : yitiandelan.
---------------------------------------------------------*/
void My_rtc::read(void)
{
	NowTime_x = RTC_GetCounter(); 

	localtime_r(&NowTime_x, &NowTime);
}
/*-------------------------------------------------------
功能	: 设置rtc时间
输入	: void
返回	: void
Copyright : yitiandelan.
---------------------------------------------------------*/
void My_rtc::write(void)
{
	RTC_WaitForLastTask();
	RTC_SetCounter(mktime(&NowTime));
	RTC_WaitForLastTask();
}
#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "task.h"
#include "SEGGER_RTT.h"

#include "stdio.h"
#include "string.h"

#include "LED.h"
#include "CMD.h"

#include "lm75.h"
#include "usart.h"
#include "rtc.h"


bool	IsStatic = 0;
bool	IsAuto = 1;

void vLedTask(void *pvParameters);
void vCmdTask(void *pvParameters);
void vSenTask(void *pvParameters);
void vBthTask(void *pvParameters);
void vSysTask(void *pvParameters);

xTaskHandle xLedHadle;
xTaskHandle xSysHadle;
xTaskHandle xCmdHadle;
xTaskHandle xSenHadle;
xTaskHandle xBthHadle;

int main()
{
	//ReALLPiont();
	//SetPiont();
	//Set_LED_Mode();

	Led.init();
	
	rtc_timer.init();
	rtc_timer.read();
	SEGGER_RTT_Init();

	/*创建任务*/
	xTaskCreate(vLedTask, "vLED", configMINIMAL_STACK_SIZE, NULL, 3, &xLedHadle);
	xTaskCreate(vSysTask, "vSys", configMINIMAL_STACK_SIZE, NULL, 4, &xSysHadle);
	xTaskCreate(vCmdTask, "vCmd", configMINIMAL_STACK_SIZE + 120, NULL, 2, &xCmdHadle);
	xTaskCreate(vSenTask, "vSen", configMINIMAL_STACK_SIZE, NULL, 1, &xSenHadle);
	xTaskCreate(vBthTask, "vBth", configMINIMAL_STACK_SIZE, NULL, 1, &xBthHadle);

	/*启动系统调度器*/
	vTaskStartScheduler();

	/*程序不会运行到这里，系统控制权是调度器*/
	while (1);
}

/*-------------------------------------------------------
功能	: 系统进程,led 切换逻辑; 传感器;蓝牙
输入	: void
返回	: void
Copyright : yitiandelan.
---------------------------------------------------------*/
void vSysTask(void * pvParameters)
{
	portTickType xLastWakeTime = xTaskGetTickCount();

	uint16_t i;

	while (1)
	{
		/*生日*/
		if ((rtc_timer.NowTime.tm_mon == (11 - 1))&(rtc_timer.NowTime.tm_mday == 1))
		{
			Led.Set2Num = HappyBirthday;
			Led.Set2RGB = Green;

			vTaskDelayUntil(&xLastWakeTime, 10000);
			continue;
		}
		/*纪念日*/
		else if ((rtc_timer.NowTime.tm_mon == (11 - 1))&(rtc_timer.NowTime.tm_mday == 14))
		{
			Led.Set2Num = ShowthNum;
			Led.Set2RGB = Red | Green;

			vTaskDelayUntil(&xLastWakeTime, 10000);
			continue;
		}
		/*新年*/
		else if ((rtc_timer.NowTime.tm_mon == (1 - 1))&(rtc_timer.NowTime.tm_mday == 1))
		{
			Led.Set2Num = NewYear;
			Led.Set2RGB = Red | Green | Blue;

			vTaskDelayUntil(&xLastWakeTime, 10000);
			continue;
		}
		/*圣诞节*/
		else if ((rtc_timer.NowTime.tm_mon == (12 - 1))&(rtc_timer.NowTime.tm_mday == 25))
		{
			Led.Set2Num = CHrist;
			Led.Set2RGB = Red | Blue;

			vTaskDelayUntil(&xLastWakeTime, 10000);
			continue;
		}
		/*晚安 23:00~3:00*/
		else if ((rtc_timer.NowTime.tm_hour >= 23)|(rtc_timer.NowTime.tm_hour <= 3))
		{
			Led.Set2Num = GoodNight;
			Led.Set2RGB = Green | Blue;

			vTaskDelayUntil(&xLastWakeTime, 10000);
			continue;
		}

		/*无蓝牙设备时，显示30秒时钟*/
		for (i = 0; ((i < 30)&(!IsStatic)); i++)
		{
			Led.Set2Num = ShowClock;
			Led.Set2RGB = Red | Green;

			vTaskDelayUntil(&xLastWakeTime, 1000);
		}
		/*无蓝牙设备时，显示30秒气温*/
		for (i = 0; ((i < 30)&(!IsStatic)); i++)
		{
			Led.Set2Num = ShowTemperature;
			Led.Set2RGB = Green;

			vTaskDelayUntil(&xLastWakeTime, 1000);
		}
		/*发现蓝牙设备，显示对应信息*/
		for (i = 0; ((i < 3)&(IsStatic)); i++)
		{
			Led.Set2Num = ShowHello;
			Led.Set2RGB = Red | Green | Blue;

			vTaskDelayUntil(&xLastWakeTime, 10000);
		}

		/*如果检测到蓝牙，显示30秒*/
		//if (IsStatic)
		//{
		//	Led.MenuNum = ShowLove;
		//	for (i = 0; ((i < 3)&(IsStatic)); i++)
		//		vTaskDelayUntil(&xLastWakeTime, 10000);
		//}
	}
}
/*-------------------------------------------------------
功能	: led驱动
输入	: void
返回	: void
Copyright : yitiandelan.
---------------------------------------------------------*/
void vLedTask(void * pvParameters)
{
	portTickType xLastWakeTime = xTaskGetTickCount();
	portTickType xLastChangeTime = xLastWakeTime;

	vTaskDelayUntil(&xLastWakeTime, 500);

	while (1)
	{
		//if (xLastWakeTime >= (xLastChangeTime + 30000))
		//{
		//	xLastChangeTime = xLastWakeTime;

		//	/*切换显示内容*/
		//	Led.Switch();
		//}
		//
		//if (IsAuto)
		//{
			//switch (Led.MenuNum)
			//{
			//case NoMask:
			//	ReALLPiont();
			//	break;
			//case ShowClock:
			//	ReALLPiont();
			//	//rtc_timer.read();
			//	Setday(rtc_timer.NowTime.tm_wday);
			//	SetNum(rtc_timer.NowTime.tm_mday);
			//	break;
			//case ShowLove:
			//	ReALLPiont();
			//	SetMask(1);
			//	break;
			//case Showhappy:
			//	ReALLPiont();
			//	SetMask(2);
			//	break;
			//case ShowTemperature:
			//	ReALLPiont();
			//	SetNum(lm75.NowTemper);
			//	break;
			//case HappyBirthday:
			//	ReALLPiont();
			//	SetPiontflag(0, 4, 1);	SetPiontflag(1, 4, 1);	SetPiontflag(2, 4, 1);	SetPiontflag(3, 4, 1);	SetPiontflag(4, 4, 1);	//happy			
			//	SetPiontflag(0, 7, 1);	SetPiontflag(1, 7, 1);	SetPiontflag(2, 7, 1);	SetPiontflag(3, 7, 1);	SetPiontflag(4, 7, 1);	//Birthday
			//	SetPiontflag(5, 7, 1);	SetPiontflag(6, 7, 1);	SetPiontflag(7, 7, 1);
			//	
			//	break;
			//case NewYear:
			//	ReALLPiont();
			//	SetPiontflag(0, 4, 1);	SetPiontflag(1, 4, 1);	SetPiontflag(2, 4, 1);	SetPiontflag(3, 4, 1);	SetPiontflag(4, 4, 1);	//happy	
			//	SetPiontflag(2, 6, 1);	SetPiontflag(3, 6, 1);	SetPiontflag(4, 6, 1);													//new
			//	SetPiontflag(7, 6, 1);	SetPiontflag(8, 6, 1);	SetPiontflag(9, 6, 1);	SetPiontflag(10, 6, 1);							//year	
			//	break;
			//case CHrist:
			//	ReALLPiont();
			//	SetPiontflag(0, 5, 1);	SetPiontflag(1, 5, 1);	SetPiontflag(2, 5, 1);	SetPiontflag(3, 5, 1);	SetPiontflag(4, 5, 1);	//merry
			//	SetPiontflag(0, 8, 1);	SetPiontflag(1, 8, 1);	SetPiontflag(2, 8, 1);	SetPiontflag(3, 8, 1);	SetPiontflag(4, 8, 1);	//christmas
			//	SetPiontflag(5, 8, 1);	SetPiontflag(6, 8, 1);	SetPiontflag(7, 8, 1);	SetPiontflag(8, 8, 1);
			//	break;
			//case ShowthNum:
			//	ReALLPiont();
			//	SetMask(3);												//th
			//	SetNumMask(0, 4, rtc_timer.NowTime.tm_year - 113);		//num
			//	break;
			//case GoodNight:
			//	ReALLPiont();
			//	SetPiontflag(2, 9, 1);	SetPiontflag(3, 9, 1);	SetPiontflag(4, 9, 1);	SetPiontflag(5, 9, 1);							//good
			//	SetPiontflag(2, 11, 1);	SetPiontflag(3, 11, 1);	SetPiontflag(4, 11, 1);	SetPiontflag(5, 11, 1);	SetPiontflag(6, 11, 1);	//night
			//	break;
			//case ShowCopyright:
			//	ReALLPiont();
			//	SetPiontflag(7, 4, 1);	SetPiontflag(8, 4, 1);	SetPiontflag(9, 4, 1);	SetPiontflag(10, 4, 1);	SetPiontflag(11, 4, 1);	//Clock
			//	SetPiontflag(8, 7, 1);	SetPiontflag(9, 7, 1);	SetPiontflag(10, 7, 1);	SetPiontflag(11, 7, 1);							//RevA	
			//	break;
			//case ShowName:
			//	ReALLPiont();
			//	SetPiontflag(0, 0, 1);	SetPiontflag(1, 0, 1);	SetPiontflag(3, 0, 1);	SetPiontflag(4, 0, 1);							//it is
			//	SetPiontflag(3, 3, 1);	SetPiontflag(4, 3, 1);	SetPiontflag(5, 3, 1);	SetPiontflag(6, 3, 1);	SetPiontflag(7, 3, 1);	//tianlan
			//	SetPiontflag(8, 3, 1);	SetPiontflag(9, 3, 1);
			//	SetPiontflag(5, 6, 1);	SetPiontflag(6, 6, 1);																			//to
			//	SetPiontflag(8, 11, 1);	SetPiontflag(9, 11, 1);	SetPiontflag(10, 11, 1);SetPiontflag(11, 11, 1);						//zeze
			//	break;
			//case ShowHello:
			//	ReALLPiont();
			//	SetPiontflag(1, 3, 1);	SetPiontflag(2, 3, 1);																			//hi
			//	SetPiontflag(8, 11, 1);	SetPiontflag(9, 11, 1);	SetPiontflag(10, 11, 1); SetPiontflag(11, 11, 1);						//zeze
			//	break;
			//}
		//}

		//while (1)
		//{
		//	TheMatrix();
		//	vTaskDelayUntil(&xLastWakeTime, 200);
		//}

		/*淡入淡出*/
		if ((Led.MenuNum != Led.Set2Num) | (Led.RGB != Led.Set2RGB))
		{
			for (; Led.Grad != 0; Led.Grad--)
			{
				SetPiont();
				vTaskDelayUntil(&xLastWakeTime, 80);
			}

			Led.MenuNum = Led.Set2Num;
			Led.RGB = Led.Set2RGB;
			Led.ReMask();
			
			for (; Led.Grad != disbufNum; Led.Grad++)
			{
				SetPiont();
				vTaskDelayUntil(&xLastWakeTime, 80);
			}
			//IsChange = 0;
		}
		else
		{
			Led.ReMask();
		}
		/*更新显存*/
		SetPiont();

		vTaskDelayUntil(&xLastWakeTime, 200);
	}
}

/*-------------------------------------------------------
功能	: 控制台进程
输入	: void
返回	: void
Copyright : yitiandelan.
---------------------------------------------------------*/
void vCmdTask(void *pvParameters)
{
	portTickType xLastWakeTime = xTaskGetTickCount();


	SEGGER_RTT_WriteString(0, "\r\n\r\n");
	SEGGER_RTT_WriteString(0, "---------------------------------------------------------\r\n");
	SEGGER_RTT_WriteString(0, "----------------STM32 J-link RTT Console-----------------\r\n");
	SEGGER_RTT_WriteString(0, "-------------------------by yitiandelan.-----------------\r\n");
	SEGGER_RTT_WriteString(0, "---------------------------------------------------------\r\n");
	SEGGER_RTT_WriteString(0, "\r\nroot>");

	while (1)
	{
		vTaskDelayUntil(&xLastWakeTime, 100);

		CmdTask();
	}

}
/*-------------------------------------------------------
功能	: 读取传感器
输入	: void
返回	: void
Copyright : yitiandelan.
---------------------------------------------------------*/
void vSenTask(void *pvParameters)
{
	portTickType xLastWakeTime = xTaskGetTickCount();

	lm75.init();

	while (1)
	{
		lm75.read();
		rtc_timer.read();
		
		vTaskDelayUntil(&xLastWakeTime, 500);
	}
}
/*-------------------------------------------------------
功能	: 蓝牙扫描进程
输入	: void
返回	: void
Copyright : yitiandelan.
---------------------------------------------------------*/
void vBthTask(void *pvParameters)
{
	portTickType xLastWakeTime = xTaskGetTickCount();
	portTickType xLastStaticTime = xLastWakeTime;

	uint8_t flag = false;

	ttyS0.init();

	while (1)
	{
		//ttyS0.write("AT+RNAMEa8,44,81,d7,4d,6f\r\n", 27);
		
		ttyS0.write("AT+RNAME22,22,95,43,a1,cc\r\n", 27);

		vTaskDelayUntil(&xLastWakeTime, 5000);

		//if (xLastWakeTime < (xLastStaticTime + 10000))
		//		IsStatic = 1;
		//else	IsStatic = 0;

		ttyS0.read();
		
		if (strncmp((const char *)ttyS0.Rxbuf, "OK\r\n+RNAME=", 11) == 0)
		{
			if (flag)
			{
				IsStatic = 0;
			}
			else
			{
				IsStatic = 1;
				flag = true;

				vTaskDelayUntil(&xLastWakeTime, 30000);
			}
			
		}
		else
			flag = false;

		if ((xLastStaticTime + 10000) <= xLastWakeTime)
			IsStatic = 0;

		ttyS0.Rxbuf[10] = ' ';
	}
}


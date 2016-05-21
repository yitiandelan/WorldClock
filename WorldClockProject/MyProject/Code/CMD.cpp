#include "CMD.h"
#include "LED.h"
#include "usart.h"
#include "lm75.h"
#include "rtc.h"

#include "ctype.h"


char str[32] = { 0 };
char strcnt = 0;

extern xTaskHandle xLedHadle;
extern xTaskHandle xCmdHadle;
extern xTaskHandle xSenHadle;
extern xTaskHandle xBthHadle;

extern bool	IsStatic;
extern bool IsAuto;

/*字符串转无符号整型*/
/*  by yitiandelan. */
uint16_t my_atol(char *str)
{
	uint8_t i;
	uint16_t num = 0;
	
	for (i = 0; i < 16; i++)
	{
		if (isdigit(*str))		//如果是数字
		{
			if (isdigit(*(str - 1)))	//上一个是数字
				num = num * 10 + (*str - '0');
			else
				num = *str - '0';
		}
		else if (isdigit(*(str - 1)))	//上一个是数字
			return num;
		
		str++;
	}
	return num;
}
/*字符串转无符号整型*/
/*  by yitiandelan. */
uint16_t my_strtol(char *str, char **p)
{
	uint8_t i;
	uint16_t num = 0;

	for (i = 0; i < 16; i++)
	{
		if (isdigit(*str))		//如果是数字
		{
			if (isdigit(*(str - 1)))	//上一个是数字
				num = num * 10 + (*str - '0');
			else
				num = *str - '0';
		}
		else if (isdigit(*(str - 1)))	//上一个是数字
		{
			*p = (char *)(str + 1);
			return num;
		}
		str++;
	}
	return 0;
}


void CmdTask(void)
{
	if (SEGGER_RTT_HasKey())
	{
		SEGGER_RTT_Read(0, str + strcnt, 1);
		if (str[strcnt] == '\n')
		{
			if (strncmp(str, "reboot\n", 7) == 0)
			{
				__set_FAULTMASK(1);		//关闭所有中断
				NVIC_SystemReset();		//复位
			}
			else if (strncmp(str, "tasklist\n", 9) == 0)
			{
				//vTaskList(Sysinfo);

				/*获取进程并显示*/
				SEGGER_RTT_WriteString(0, "\r\n\r\nTaskName	State Priority	Stack	Num");
				SEGGER_RTT_WriteString(0, "\r\n------------------------------------------\r\n");

				{
					volatile UBaseType_t uxArraySize;
					uint32_t ulTotalRunTime, ulStatsAsPercentage;
					TaskStatus_t pxTaskStatusArray[10];
					uint8_t x;
					char cStatus;
					uxArraySize = uxTaskGetNumberOfTasks();
					uxArraySize = uxTaskGetSystemState(pxTaskStatusArray, uxArraySize, &ulTotalRunTime);
					for (x = 0; x < uxArraySize; x++)
					{
						switch (pxTaskStatusArray[x].eCurrentState)
						{
						case eReady:		cStatus = 'R';
							break;
						case eBlocked:		cStatus = 'B';
							break;
						case eSuspended:	cStatus = 'S';
							break;
						case eDeleted:		cStatus = 'D';
							break;
						default:
							cStatus = 0x00;
							break;
						}
						SEGGER_RTT_printf(0, "%s\t\t%c\t%u\t%u\t%u\r\n", pxTaskStatusArray[x].pcTaskName, cStatus, (unsigned int)pxTaskStatusArray[x].uxCurrentPriority, (unsigned int)pxTaskStatusArray[x].usStackHighWaterMark, (unsigned int)pxTaskStatusArray[x].xTaskNumber);
					}
				}
			}
			else if (strncmp(str, "time\n", 5) == 0)
			{
				//rtc_timer.read();
				SEGGER_RTT_printf(0, "\r\n\r\nNow time : %.4d/%d/%d %d %.2d:%.2d:%.2d\r\n", (rtc_timer.NowTime.tm_year+1900), (rtc_timer.NowTime.tm_mon+1), rtc_timer.NowTime.tm_mday, rtc_timer.NowTime.tm_wday,rtc_timer.NowTime.tm_hour, rtc_timer.NowTime.tm_min, rtc_timer.NowTime.tm_sec);
			}
			else if (strncmp(str, "settime ", 8) == 0)
			{
				char *p;
				int year, mon, mday, hour, min, sec;

				p = str + 8;

				year = my_strtol(p, &p);
				mon = my_strtol(p, &p);
				mday = my_strtol(p, &p);
				hour = my_strtol(p, &p);
				min = my_strtol(p, &p);
				sec = my_strtol(p, &p);

				if ((year >= 1970)&(mon <= 12)&(mday <= 31)&(mday != 0)&(hour <= 23)&(min <= 59)&(sec <= 60))
				{
					rtc_timer.NowTime.tm_year = year - 1900;
					rtc_timer.NowTime.tm_mon = mon - 1;
					rtc_timer.NowTime.tm_mday = mday;

					rtc_timer.NowTime.tm_hour = hour;
					rtc_timer.NowTime.tm_min = min;
					rtc_timer.NowTime.tm_sec = sec;

					rtc_timer.write();

					SEGGER_RTT_printf(0, "\r\n\r\nSet time : %.4d/%d/%d   %.2d:%.2d:%.2d\r\n", year, mon, mday, hour, min, sec);
					SEGGER_RTT_printf(0, "Now time : %.4d/%d/%d %d %.2d:%.2d:%.2d\r\n", (rtc_timer.NowTime.tm_year + 1900), (rtc_timer.NowTime.tm_mon + 1), rtc_timer.NowTime.tm_mday, rtc_timer.NowTime.tm_wday, rtc_timer.NowTime.tm_hour, rtc_timer.NowTime.tm_min, rtc_timer.NowTime.tm_sec);
				}
				else
					SEGGER_RTT_WriteString(0, "\r\n\r\nInput data must be similar : 2014/12/31 00:00:00\r\nand the Years must more than 1970.\r\n");
			}
			else if (strncmp(str, "ttyS0\n", 6) == 0)
			{
				bool ttyS0flag = true;

				vTaskSuspend(xBthHadle);

				SEGGER_RTT_WriteString(0, "\r\n\r\nttyS0 is Open.\r\nPress ESC to exit.\r\n\r\n");

				while (ttyS0flag)
				{
					if (SEGGER_RTT_HasKey())
					{
						SEGGER_RTT_Read(0, str, 1);

						switch (str[0])
						{
						case 0x1B:
							ttyS0flag = false;
							break;
						case '\n':
							ttyS0.write("\r\n", 2);
							SEGGER_RTT_WriteString(0, "\r\n");
							break;
						default:
							ttyS0.write(str, 1);
							break;
						}
					}
					vTaskDelay(100);
					SEGGER_RTT_Write(0, (const char *)ttyS0.Rxbuf, ttyS0.read());
				}

				vTaskResume(xBthHadle);

				SEGGER_RTT_WriteString(0, "\r\n\r\nttyS0 is Close.\r\n");
			}
			else if (strncmp(str, "setpt\n", 6) == 0)
			{
				SEGGER_RTT_WriteString(0,"\r\n\r\nsetpt (0-11),(0-11),(0/1)\r\n");
			}
			else if (strncmp(str, "setpt ", 6) == 0)
			{
				{
					char *p;
					uint8_t i, x = 0, y = 0, flag = 0;

					p = str + 5;

					x = my_strtol(p, &p);
					y = my_strtol(p, &p);
					flag = my_strtol(p, &p);

					SetPiontflag(x, y, flag);

					SEGGER_RTT_printf(0, "\r\n\r\nSet (%d,%d) to %d successful.\r\n", x,y,flag);
				}
			}
			else if (strncmp(str, "setrgb ", 7) == 0)
			{
				{
					char *p;
					uint8_t i, r = 0, g = 0, b = 0;

					p = str + 5;

					r = my_strtol(p, &p);
					g = my_strtol(p, &p);
					b = my_strtol(p, &p);

					SetPiontRGB(r, g, b);

					SEGGER_RTT_printf(0, "\r\n\r\nSet R/G/B to %d,%d,%d successful.\r\n", r, g, b);
				}
			}
			else if (strncmp(str, "clear\n", 6) == 0)
			{
				ReALLPiont();

				SEGGER_RTT_printf(0, "\r\n\r\nClear successful.\r\n");
			}
			else if (strncmp(str, "set to ", 7) == 0)
			{
				if (strncmp(str + 7, "nomask\n", 7) == 0)
					SetMask(0);
				else if (strncmp(str + 7, "love\n", 5) == 0)
					SetMask(1);
				else if (strncmp(str + 7, "happy\n", 6) == 0)
					SetMask(2);
				else if (isdigit(str[7]))
					SetNum(my_atol(str + 7));

				SEGGER_RTT_printf(0, "\r\n%s successful.\r\n",str);
			}
			else if (strncmp(str, "info\n", 5) == 0)
			{
				SEGGER_RTT_WriteString(0, "\r\n\r\n");
				SEGGER_RTT_WriteString(0, "CPU:	STM32F103ZE.\r\n");
				SEGGER_RTT_WriteString(0, "kernel:	FreeRTOS V8.1.2\r\n");
				SEGGER_RTT_WriteString(0, "Booting from flash...\r\n");

				SEGGER_RTT_printf(0, "\r\nNow Temperature : %d'C\r\n", lm75.NowTemper);

				if (IsStatic)
					SEGGER_RTT_WriteString(0, "\r\nFind bluetooth device.\r\n");
				else
					SEGGER_RTT_WriteString(0, "\r\nNo find bluetooth device.\r\n");
			}
			else if (strncmp(str, "help\n", 5) == 0)
			{
				SEGGER_RTT_WriteString(0, "\r\n\r\n");
				SEGGER_RTT_WriteString(0, "Order		Meaning\r\n");
				SEGGER_RTT_WriteString(0, "------------------------------------------\r\n");
				SEGGER_RTT_WriteString(0, "info		-Show System info.\r\n");
				SEGGER_RTT_WriteString(0, "reboot		-System Reset.\r\n");
				SEGGER_RTT_WriteString(0, "tasklist	-Show Task info.\r\n");
				SEGGER_RTT_WriteString(0, "time		-Show System Time.\r\n");
			}

			SEGGER_RTT_WriteString(0, "\r\nroot>");

			for (; strcnt != 0; strcnt--)
				str[strcnt] = '\0';
		}
		else
		{
			strcnt++;
			if (strcnt == 32)
				strcnt = 0;
		}
	}
}

#ifndef __CONFIG_H__
#define __CONFIG_H__
#include "stm32f10x.h" 

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h" 
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "Delay.h"
#include "PC.h"
#include "WIFI.h"
#include "BlueTooth.h"
#include "Flash.h"
#include "Init.h"
#include "Sensor.h"
#include "OLED.h"
#include "Light.h"
#include "WarningTone.h"
#include "Reset.h"
#include "Utils.h"
#include "Dog.h"



#define  MAX_AT_WAIT_TIMES_COUNT	30000 // AT指令最大的等待时钟节拍
#define  TRY_AT_COMMAND_AGAIN_TIMES	30000 // AT指令失败后, 再次进行重试的时钟节拍间隔

#define  FLASH_ADDRESS 0x0800F000
#define  FLASH_STRING_BUF_LEN 40

#define  AT_SWJAP  "AT+CWJAP_DEF=\"%s\",\"%s\"\r\n"



#endif

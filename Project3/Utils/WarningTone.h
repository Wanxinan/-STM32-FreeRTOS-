#ifndef __WARNINGTONE_H__
#define __WARNINGTONE_H__
#include "Config.h"  

// 程序运行过程中, 在必要时刻, 如果程序运转异常, 发出警告提示音

void WarningTone_Init(void); // 蜂鸣器初始化

void openWarningTone(uint16_t GPIO_Pin_x);// 打开(低电平)
void closeWarningTone(uint16_t GPIO_Pin_x);// 打开
void changeWarningTone(uint16_t GPIO_Pin_x);// 蜂鸣器状态取反

void startTone(void); // 设备启动提示:"滴~"


#endif



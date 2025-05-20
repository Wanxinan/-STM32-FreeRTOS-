#ifndef __LIGHT_H__
#define __LIGHT_H__
#include "Config.h"  

// 程序运行过程中, 必要时刻, 通过灯状态变化提示用户

void Light_Init(void);// LED灯初始化

void openLight(uint16_t GPIO_Pin_x);// 开灯
void closeLight(uint16_t GPIO_Pin_x);// 关灯
void changeLight(uint16_t GPIO_Pin_x);// 灯状态取反

#endif


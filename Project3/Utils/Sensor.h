#ifndef __SENSOR_H__
#define __SENSOR_H__
#include "Config.h"  

// ADC传感器数据转化

void Sensor_Init(void);// ADC转化初始化
uint16_t AD_GetValue(uint8_t ADC_Channel); // ADC通道转化
void getMSG(char * buf);

#endif

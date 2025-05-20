#include "Sensor.h"


void Sensor_Init(void)
{
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	
	GPIO_InitTypeDef gpioInit;
	gpioInit.GPIO_Mode = GPIO_Mode_AIN;
	gpioInit.GPIO_Pin =  GPIO_Pin_1 | GPIO_Pin_4;// 使用PA1和PA4采样模拟数据
	gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpioInit);
	
	ADC_InitTypeDef ADC_InitStructure;	
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);
	ADC_Cmd(ADC1, ENABLE);
	
	ADC_ResetCalibration(ADC1);	
	while (ADC_GetResetCalibrationStatus(ADC1) == SET);
	ADC_StartCalibration(ADC1);
	while (ADC_GetCalibrationStatus(ADC1) == SET);
}

	
uint16_t AD_GetValue(uint8_t ADC_Channel)
{
	ADC_RegularChannelConfig(ADC1, ADC_Channel, 1, ADC_SampleTime_55Cycles5);
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);	
	while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
	return ADC_GetConversionValue(ADC1);
}

void getMSG(char * buf){
	uint16_t AD1 = AD_GetValue(ADC_Channel_1);		
	uint16_t AD4 = AD_GetValue(ADC_Channel_4);		

	// 转换为字符串并追加到buf
	char temp[50] = {0};
	sprintf(temp, "%s:%u,%s:%u","parameter1", AD1, "parameter2", AD4);
	

	// GET /0 HTTP/1.1 
	// 
	// { "status": 0 , "stm32_msg": "parameter1:2000,parameter2: 1700" }
	strcat(buf, "GET /0 HTTP/1.1 \r\n\r\n");
	strcat(buf, "{\"status\": 0, \"stm32_msg\":");
	strcat(buf, "\"");
	strcat(buf, temp);
	strcat(buf, "\" }");
}




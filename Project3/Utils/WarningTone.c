#include "WarningTone.h"


void WarningTone_Init(void) {
 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitTypeDef GPIO_InitStruct; 
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15; 
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_SetBits(GPIOB, GPIO_Pin_15); // 默认关闭
}

void openWarningTone(uint16_t GPIO_Pin_x){
	GPIO_ResetBits(GPIOB, GPIO_Pin_x); // 打开(低电平)
}
void closeWarningTone(uint16_t GPIO_Pin_x){
	GPIO_SetBits(GPIOB, GPIO_Pin_x); // 打开
}

// 取反蜂鸣器状态
void changeWarningTone(uint16_t GPIO_Pin_x){
    // 读取当前GPIO引脚的状态
    if(GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_x) == Bit_RESET){
        // 如果当前是低电平(蜂鸣器开启)则关闭蜂鸣器
        closeWarningTone(GPIO_Pin_x);
    }
    else{
        // 如果当前是高电平(蜂鸣器关闭)则打开蜂鸣器
        openWarningTone(GPIO_Pin_x);
    }
}

void startTone(void){
	openWarningTone(GPIO_Pin_15);
	Delay_ms(700);
	closeWarningTone(GPIO_Pin_15);
}


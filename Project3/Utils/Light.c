#include "Light.h"

void Light_Init(void) {

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_WriteBit(GPIOB, GPIO_Pin_5 |  GPIO_Pin_6 | GPIO_Pin_7, Bit_RESET);  // 默认关灯
}

void openLight(uint16_t GPIO_Pin_x) {
    GPIO_WriteBit(GPIOB, GPIO_Pin_x, Bit_SET); // 开灯
}

void closeLight(uint16_t GPIO_Pin_x) {
    GPIO_WriteBit(GPIOB, GPIO_Pin_x, Bit_RESET); // 关灯
}
// 灯状态取反
void changeLight(uint16_t GPIO_Pin_x){
	 if (GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_x) == Bit_SET) {
        // 如果当前引脚状态为高电平(灯亮),则关闭灯
        closeLight(GPIO_Pin_x);
    } else {
        // 如果当前引脚状态为低电平(灯灭),则打开灯
        openLight(GPIO_Pin_x);
    }
}




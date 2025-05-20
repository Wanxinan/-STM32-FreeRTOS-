#include "Reset.h"

void Reset_Init(void) {

    // 引脚配置
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);


    // 配置外部中断
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);

    EXTI_InitTypeDef EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; // 下降沿触发
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    // 配置中断
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =
        1; // 调高优先级: 用户按键的行为, 比较迫切,应该优先级比较高
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

}

int times = 0;
void EXTI0_IRQHandler(void) {
    if (EXTI_GetITStatus(EXTI_Line0) != RESET) {

        times++;
        Delay_ms(20); // 按键消抖

        while (times > 0) {
            // 获取按钮状态
            if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == Bit_RESET) {
                // 按钮还在按
                times++;
								printf1("key:48 \r\n");
                Delay_ms(20);
            } else {
                // 按钮释放
                break;
            }
        }

        if (times >= 200) { // 长按, 清空Flash, 复位重启
            printf1("long \r\n");
            flashClean();
            NVIC_SystemReset();
        } else { // 短按, 仅复位重启
            NVIC_SystemReset(); // 触发系统复位
        }

        times = 0;
        EXTI_ClearITPendingBit(EXTI_Line0);
    }
}





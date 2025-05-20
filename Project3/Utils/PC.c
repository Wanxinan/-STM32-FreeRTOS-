#include "PC.h"



void PC_Init(void){

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;//PA9:USART1_TX
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10:USART1_RX
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1, &USART_InitStructure);	
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); 
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 12;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
		

	USART_Cmd(USART1, ENABLE);
}

// 该中断目前只有一种触发方式, 即PC通过串口助手, 向STM32发送数据
void USART1_IRQHandler(void) {
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
		
		char data = USART_ReceiveData(USART1);
				
		// 读取数据, 放入消息队列
		//BaseType_t stat = pdTRUE;
		//xQueueSendFromISR(queue, &data, &stat );
		printf1("3\r\n");
				
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
}

void PC_SendByte(uint8_t Byte){
	USART_SendData(USART1, Byte);
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

void printf1(char *format, ...)
{
	char strs[200];	
	va_list list;
	va_start(list, format);
	vsprintf(strs, format, list);
	va_end(list);

	for (uint8_t i=0; strs[i] != '\0'; i++){
		PC_SendByte(strs[i]);
	}
}

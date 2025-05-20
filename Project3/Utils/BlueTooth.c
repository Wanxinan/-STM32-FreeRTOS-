#include "BlueTooth.h"


void BlueTooth_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	 
	GPIO_InitTypeDef initType;
	initType.GPIO_Mode = GPIO_Mode_AF_PP;
	initType.GPIO_Pin = GPIO_Pin_10; // TX
	initType.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &initType);
	
	initType.GPIO_Mode = GPIO_Mode_IPU;
	initType.GPIO_Pin = GPIO_Pin_11; // RX
	initType.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &initType);

	USART_InitTypeDef initUSART;
	initUSART.USART_BaudRate = 9600; // 需要注意和蓝牙通信的波特率
	initUSART.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	initUSART.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	initUSART.USART_Parity = USART_Parity_No;
	initUSART.USART_StopBits = USART_StopBits_1;
	initUSART.USART_WordLength = USART_WordLength_8b;	
	USART_Init(USART3, &initUSART);
	
	USART_Cmd(USART3, ENABLE);
}

void BlueTooth_NVIC(void)
{
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 12;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
}


void blueToothSendByte(uint8_t byte){
	USART_SendData(USART2, byte);		
	while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
}

void messageToApp(char *strs){
	printf1("messageToApp:47: %s \r\n",strs);
	for (uint8_t i = 0; strs[i] != '\0'; i++)
	{
		blueToothSendByte(strs[i]);		
	}
}


void USART3_IRQHandler(void)
{
	if (USART_GetITStatus(USART3, USART_IT_RXNE) == SET)
	{
		
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);
		char data = USART_ReceiveData(USART3);
		
		// 接收蓝牙发送过来的消息, 存入消息队列
		BaseType_t stat = pdTRUE;
		xQueueSendFromISR(queueBlueTooth, &data, &stat );
		
	}
}

void readBlueTooth(char * buf){
	
    int flag = 0;
		int size = 0;
    while (1) {
        char ch = 0;
        BaseType_t ret = xQueueReceive(queueBlueTooth, &ch,  1000);
				if(size==0 && ch != '!'){
					continue;
				}
				printf1("readBlueTooth:52: %s \r\n",buf);
        if (ret == pdPASS) { // 读到AT指令返回信息, 存储
            buf[size] = ch;
            size++;
        }

        if (ch == '!') {
            flag++;
        } 
				if(flag == 2){
					break;
				}
    }
}


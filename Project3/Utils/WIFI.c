#include "WIFI.h"

void WIFI_Init(void) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    // 对应USART2->Tx, 即数据发送引脚
    GPIO_InitTypeDef initType;
    initType.GPIO_Mode = GPIO_Mode_AF_PP;
    initType.GPIO_Pin = GPIO_Pin_2;
    initType.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &initType);

    // 对应USART2->Rx, 即数据接收引脚
    initType.GPIO_Mode = GPIO_Mode_IPU;
    initType.GPIO_Pin = GPIO_Pin_3;
    initType.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &initType);

    // USART配置
    USART_InitTypeDef initUSART;
    initUSART.USART_BaudRate = 115200;
    initUSART.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    initUSART.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    initUSART.USART_Parity = USART_Parity_No;
    initUSART.USART_StopBits = USART_StopBits_1;
    initUSART.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART2, &initUSART);


    // 让串口开始工作运行
    USART_Cmd(USART2, ENABLE);
}

void WIFI_NVIC(void) {
    // 配置开启USART接收数据的中断
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

    // 配置中断和优先级
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 12;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVIC_InitStructure);

}

void wifiSendByte(uint8_t byte) {
    USART_SendData(USART2, byte);

    while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
}

void wifiSendAtCommand(char *strs) {
    for (uint8_t i = 0; strs[i] != '\0'; i++) {
        wifiSendByte(strs[i]);
    }
}

void messageUpload(char *strs) {


    for (uint8_t i = 0; strs[i] != '\0'; i++) {
        wifiSendByte(strs[i]);
    }
}

void USART2_IRQHandler(void) {
    if (USART_GetITStatus(USART2, USART_IT_RXNE) == SET) {
        USART_ClearITPendingBit(USART2, USART_IT_RXNE);
        char data = USART_ReceiveData(USART2);

        // 接收ESP01S数据回传, 存入消息队列
        BaseType_t stat = pdTRUE;
        xQueueSendFromISR(queueWIFI, &data, &stat);
    }
}

int command(char * atStr) {
    // 清空消息队列
    xQueueReset(queueWIFI);

    TickType_t beginTicks = xTaskGetTickCount(); // 获得当前时钟节拍
    wifiSendAtCommand(atStr); // 发送命令
    printf1("command atStr 84: %s\r\n", atStr);
    char buf[150] = {0};
    int size = 0;

    while (1) {
        char ch = 0;
        BaseType_t ret = xQueueReceive(queueWIFI, &ch,  1000);

        if (ret == pdPASS) { // 读到AT指令返回信息, 存储
            buf[size] = ch;
            size++;
        }

        if (endsWithStr(buf, "OK\r\n")) {
            printf1("command buf 98: %s\r\n", buf);
            return 0;
        } else if (endsWithStr(buf, "FAIL\r\n")) {
            printf1("command buf 101: %s\r\n", buf);
            return -1;
        } else if (endsWithStr(buf, "ERROR\r\n")) {
            printf1("command buf 104: %s\r\n", buf);
            return -1;
        } else if (endsWithStr(buf, "busy p...\r\n")) {
            printf1("command buf 107: %s\r\n", buf);
            return -1;
        }

        if (MAX_AT_WAIT_TIMES_COUNT + beginTicks < xTaskGetTickCount()) {
            printf1("command buf 112: %s\r\n", buf);
            return -1;// 等待返回超时
        }
    }
}


int atCommand(void) {

    while (1) {

        int tips = 1; // 开始ESP01S指令, 开始提示
        xQueueSend(tipsQueue, &tips, 1000);
        int ret =  command("AT \r\n"); // 三大种可能: 失败, 成功, 超时
        tips = 0; // ESP01S指令结束, 结束提示
        xQueueSend(tipsQueue, &tips, 1000);

        if (ret == 0) { // 指令成功
            return 0;
        }

        // 走到这, 意味着AT指令失败

        tips = -1; // ESP01S指令失败, 开始体质
        xQueueSend(tipsQueue, &tips, 1000);
        Delay_ms(TRY_AT_COMMAND_AGAIN_TIMES);// AT指令失败后, 再次进行重试的时钟节拍间隔
        tips = 0; // 结束提示
        xQueueSend(tipsQueue, &tips, 1000);
    }
}

// 获取此刻ESP01S的网络连接状态: 0有链接,-1无连接
int getLinkedStatus(void) {

    TickType_t beginTicks = xTaskGetTickCount(); // 获得当前时钟节拍
    wifiSendAtCommand("AT+CWJAP_DEF?\r\n"); // 发送命令

    char buf[150] = {0};
    int size = 0;

    while (1) {
        char ch = 0;
        BaseType_t ret = xQueueReceive(queueWIFI, &ch,  1000);

        if (ret == pdPASS) { // 读到AT指令返回信息, 存储
            buf[size] = ch;
            size++;
        }

        // 情况1
        //AT+CWJAP_DEF?
        //No AP

        //OK

        // 情况2
        //AT+CWJAP_DEF?
        //+CWJAP_DEF:"snow","ea:02:96:74:89:99",1,-36,0

        //OK

        if (hasStr(buf, "No AP")) {
            return -1;
        } else if (hasStr(buf, "+CWJAP_DEF:\"")) {
            return 0;
        }

        if (MAX_AT_WAIT_TIMES_COUNT + beginTicks < xTaskGetTickCount()) {
            return -1;// 等待返回超时
        }
    }
}


//


int netWorkCommand(char * buffer) {

    char bufName[20] = {0};
    char bufPassword[20] = {0};
    int index = 0;

		// !账号=密码!
    // 切割出用户名和密码
    // 1~=
    for (int i = index + 1; i < FLASH_STRING_BUF_LEN; i++) {
        if (buffer[i] == '=') {
            index = i;
            break;
        }

        bufName[i - 1] = buffer[i];
    }

    // =~!
    for (int i = index + 1; i < FLASH_STRING_BUF_LEN; i++) {
        if (buffer[i] == '!') {
            break;
        }

        bufPassword[i - 1 - index] = buffer[i];
    }

    char buf[50] = {0};
    sprintf(buf, AT_SWJAP, bufName, bufPassword);

    while (1) {

        int tips = 1; // 开始ESP01S指令, 开始提示
        xQueueSend(tipsQueue, &tips, 1000);
        int ret =  command(buf); // 三大种可能: 失败, 成功, 超时
        printf1("netWorkCommand: 221: %s\r\n", buf);
        tips = 0; // ESP01S指令结束, 结束提示
        xQueueSend(tipsQueue, &tips, 1000);

        if (ret == 0) { // 指令成功
            return 0;
        }

        // 走到这, 意味着AT指令失败

        tips = -1; // ESP01S指令失败, 开始提示
        xQueueSend(tipsQueue, &tips, 1000);
        Delay_ms(TRY_AT_COMMAND_AGAIN_TIMES);// AT指令失败后, 再次进行重试的时钟节拍间隔
        tips = 0; // 结束提示
        xQueueSend(tipsQueue, &tips, 1000);
    }
}

int tcpCommand(void) {

    while (1) {

        int tips = 1; // 开始ESP01S指令, 开始提示
        xQueueSend(tipsQueue, &tips, 1000);
        int ret =  command("AT+CIPSTART=\"TCP\",\"47.115.220.165\",8000\r\n"); // 三种可能: 失败, 成功, 超时
        tips = 0; // ESP01S指令结束, 结束提示
        xQueueSend(tipsQueue, &tips, 1000);

        if (ret == 0) { // 指令成功
            return 0;
        }

        // 走到这, 意味着AT指令失败

        tips = -1; // ESP01S指令失败, 开始提示
        xQueueSend(tipsQueue, &tips, 1000);
        Delay_ms(TRY_AT_COMMAND_AGAIN_TIMES);// AT指令失败后, 再次进行重试的时钟节拍间隔
        tips = 0; // 结束提示
        xQueueSend(tipsQueue, &tips, 1000);
    }
}


int cipCommand(void) {

    while (1) {

        int tips = 1; // 开始ESP01S指令, 开始提示
        xQueueSend(tipsQueue, &tips, 1000);
        int ret =  command("AT+CIPMODE=1\r\n"); // 三大种可能: 失败, 成功, 超时
        tips = 0; // ESP01S指令结束, 结束提示
        xQueueSend(tipsQueue, &tips, 1000);

        if (ret == 0) { // 指令成功
            return 0;
        }

        // 走到这, 意味着AT指令失败

        tips = -1; // ESP01S指令失败, 开始提示
        xQueueSend(tipsQueue, &tips, 1000);
        Delay_ms(TRY_AT_COMMAND_AGAIN_TIMES);// AT指令失败后, 再次进行重试的时钟节拍间隔
        tips = 0; // 结束提示
        xQueueSend(tipsQueue, &tips, 1000);
    }
}


int beginSendCommand(void) {

    while (1) {

        int tips = 1; // 开始ESP01S指令, 开始提示
        xQueueSend(tipsQueue, &tips, 1000);
        int ret =  command("AT+CIPSEND\r\n"); // 三大种可能: 失败, 成功, 超时
        tips = 0; // ESP01S指令结束, 结束提示
        xQueueSend(tipsQueue, &tips, 1000);

        if (ret == 0) { // 指令成功
            return 0;
        }

        // 走到这, 意味着AT指令失败

        tips = -1; // ESP01S指令失败, 开始提示
        xQueueSend(tipsQueue, &tips, 1000);
        Delay_ms(TRY_AT_COMMAND_AGAIN_TIMES);// AT指令失败后, 再次进行重试的时钟节拍间隔
        tips = 0; // 结束提示
        xQueueSend(tipsQueue, &tips, 1000);
    }
}

int exitSendCommand(void) {
    vTaskDelay(1000);
    wifiSendAtCommand("+++");
    vTaskDelay(1000);
    return 0;
}

int closeCipCommand(void) {

    while (1) {

        int tips = 1; // 开始ESP01S指令, 开始提示
        xQueueSend(tipsQueue, &tips, 1000);
        int ret =  command("AT+CIPMODE=0\r\n"); // 三大种可能: 失败, 成功, 超时
        tips = 0; // ESP01S指令结束, 结束提示
        xQueueSend(tipsQueue, &tips, 1000);

        if (ret == 0) { // 指令成功
            return 0;
        }

        // 走到这, 意味着AT指令失败

        tips = -1; // ESP01S指令失败, 开始提示
        xQueueSend(tipsQueue, &tips, 1000);
        Delay_ms(TRY_AT_COMMAND_AGAIN_TIMES);// AT指令失败后, 再次进行重试的时钟节拍间隔
        tips = 0; // 结束提示
        xQueueSend(tipsQueue, &tips, 1000);
    }
}

int closeTCPCommand(void) {

    while (1) {

        int tips = 1; // 开始ESP01S指令, 开始提示
        xQueueSend(tipsQueue, &tips, 1000);
        int ret =  command("AT+CIPCLOSE\r\n"); // 三大种可能: 失败, 成功, 超时
        tips = 0; // ESP01S指令结束, 结束提示
        xQueueSend(tipsQueue, &tips, 1000);

        if (ret == 0) { // 指令成功
            return 0;
        }

        // 走到这, 意味着AT指令失败

        tips = -1; // ESP01S指令失败, 开始提示
        xQueueSend(tipsQueue, &tips, 1000);
        Delay_ms(TRY_AT_COMMAND_AGAIN_TIMES);// AT指令失败后, 再次进行重试的时钟节拍间隔
        tips = 0; // 结束提示
        xQueueSend(tipsQueue, &tips, 1000);
    }
}

void sendToALIYun(char * buffer) {
    printf1("sendToALIYun:360 \r\n");
    tcpCommand();
    printf1("sendToALIYun:362 \r\n");
    cipCommand();
    printf1("sendToALIYun:364 \r\n");
    beginSendCommand();
    printf1("sendToALIYun:366 \r\n");
    messageUpload(buffer);
    printf1("sendToALIYun:368 \r\n");
    exitSendCommand();
    printf1("sendToALIYun:370 \r\n");
    closeCipCommand();
    printf1("sendToALIYun:372 \r\n");
    closeTCPCommand();
    printf1("sendToALIYun:374 \r\n");
}



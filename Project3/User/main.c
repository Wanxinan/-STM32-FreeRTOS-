#include "main.h"


void tipsTask(void *arg) {

    int flag = 0;

    while (1) {
        int tips = 100;
        xQueueReceive(tipsQueue, &tips,  1000);

        if (tips == 1) {
            flag = 1;
            // 开始指令提示: 亮绿灯,黄灯闪烁提示
            openLight(GPIO_Pin_5);
            openLight(GPIO_Pin_7);

        } else if (tips == 0) {
            flag = 0;
            // 指令正常结束: 亮绿灯,关蜂鸣器,关黄灯,关红灯
            openLight(GPIO_Pin_5);
            closeLight(GPIO_Pin_7);
            closeWarningTone(GPIO_Pin_15);

        } else if (tips == -1) {
            flag = 2;
            // 指令失败提示: 灭绿灯,亮红灯,蜂鸣器开启
            closeLight(GPIO_Pin_5);
            openLight(GPIO_Pin_6);
            openWarningTone(GPIO_Pin_15);

        } else if (flag == 1) {
            changeLight(GPIO_Pin_7); // 指令执行过程中,黄灯闪烁
        } else if (flag == 2) {
            changeWarningTone(GPIO_Pin_15); // 指令失败等待过程中,蜂鸣器间断鸣叫
        }
    }
}

void mainTask(void * arg) {


    int netStatus = getLinkedStatus();
    int flashStatus = getFlashStatus();

    printf1("mainTask:46: %d ,%d \r\n", netStatus,  flashStatus);

    if (netStatus != 0 && flashStatus != 0) {
        // 未联网&Flash中也没有存储WIFI账号密码: 需要蓝牙获取wifi账号密码
        char buffer[40] = {0};
        readBlueTooth(buffer);
        printf1("mainTask:52: %s \r\n", buffer);
        // 连接网络
        netWorkCommand(buffer);

        // 存储Flash
        setFlash(buffer);

        // 回复给蓝牙连接成功
        messageToApp("{\"status\":0, \"wifi_name\": \"net\"}");

    } else if (netStatus != 0 && flashStatus == 0) {

        // 未联网, 但是Flash中存储有WIFI账号密码: 读取Flash, 联网
        char buffer[FLASH_STRING_BUF_LEN] = {0};
        readFlash(buffer);

        printf1("mainTask:68: %s \r\n", buffer);
        // 去联网
        netWorkCommand(buffer);

    } else {
        // 已经联网, 不用做别的操作
    }

    // 开启看门狗
    dogInit();

    // 循环读取传感器数据, 发送给阿里云服务器
    while (1) {
        // 仅在这里喂狗, 如果喂狗不及时(也就意味着传感器数据上传到阿里云过程有问题), 自动看门狗重启任务
        IWDG_ReloadCounter();

        // 读取传感器数据
        char buffer[150] = {0};
        getMSG(buffer);

        //printf1("mainTask:85: \r\n%s \r\n",buffer);

        // 建立TCP连接, 发送传感器数据, 关闭连接
        sendToALIYun(buffer);
    }
}


int main(void) {
    Init();

    xTaskCreate(mainTask, "mainTask", configMINIMAL_STACK_SIZE * 8, NULL, 3, NULL);
    xTaskCreate(tipsTask, "tipsTask", configMINIMAL_STACK_SIZE, NULL, 3, NULL);
    vTaskStartScheduler();

    while (1);
}

#ifndef __WIFI_H__
#define __WIFI_H__
#include "Config.h"  

// 基于串口2实现和ESP01S之间的通信
extern QueueHandle_t queueWIFI; // 字符串传输队列
extern QueueHandle_t tipsQueue;

void WIFI_Init(void); // 初始化串口
void WIFI_NVIC(void); // 初始化中断
void wifiSendAtCommand(char *strs); // AT指令发送给ESP01S
void messageUpload(char *strs); // 消息上传aliyun

int command(char * atStr); // 发送命令, 等待回复
int atCommand(void); // AT指令: 0表示正常,-1表示不正常
int getLinkedStatus(void); // 获取此刻ESP01S的网络连接状态: 0有链接,-1无连接
int netWorkCommand(char * buffer);
int tcpCommand(void);
int cipCommand(void);
int beginSendCommand(void);
int exitSendCommand(void);
int closeCipCommand(void);
int closeTCPCommand(void);
void sendToALIYun(char * buffer);
	
#endif

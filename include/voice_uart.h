/*
 * voice_uart.h
 * 语音串口通信模块 — 封装 UART 协议与语音播报
 */
#ifndef VOICE_UART_H
#define VOICE_UART_H

#include <Arduino.h>

// 初始化语音串口
void voiceUartInit();

// 发送单字节播报指令 (原 _uart_bobao1 ~ _uart_bobao16 的统一替代)
// msgId: U_MSG_BOBAO1 ~ U_MSG_BOBAO16 等，定义在 config.h
void voiceBroadcast(int msgId);

// 发送整数播报 (原 _uart_bozhensgshu)
void voiceReportInt(int value);

// 接收语音模块数据
int voiceAvailable();   // 返回可读字节数
int voiceRead();        // 读取一个字节

#endif // VOICE_UART_H

/*
 * voice_uart.cpp
 * 语音串口通信模块实现
 * 将原来 10+ 个几乎相同的 _uart_bobao*() 函数统一为 voiceBroadcast()
 */
#include "voice_uart.h"
#include "config.h"
#include <SoftwareSerial.h>

// 内部软件串口对象
static SoftwareSerial voiceSerial(PIN_UART_RX, PIN_UART_TX);

// 串口参数类型 (保留原始协议兼容)
typedef union {
  double   d_double;
  int      d_int;
  unsigned char d_ucs[8];
  char     d_char;
  unsigned char d_uchar;
  unsigned long d_long;
  short    d_short;
  float    d_float;
} uart_param_t;

// 16 位整数转 32 位整数 (平台兼容)
static void int16_to_int32(uart_param_t* param) {
  if (sizeof(int) >= 4)
    return;
  unsigned long value = param->d_long;
  unsigned long sign = (value >> 15) & 1;
  unsigned long v = value;
  if (sign)
    v = 0xFFFF0000 | value;
  uart_param_t p;
  p.d_long = v;
  param->d_ucs[0] = p.d_ucs[0];
  param->d_ucs[1] = p.d_ucs[1];
  param->d_ucs[2] = p.d_ucs[2];
  param->d_ucs[3] = p.d_ucs[3];
}

// 底层串口发送
static void uartSend(unsigned char* buff, int len) {
  for (int i = 0; i < len; i++) {
    voiceSerial.write(buff[i]);
  }
}

// ===================== 公开接口 =====================

void voiceUartInit() {
  voiceSerial.begin(115200);
}

void voiceBroadcast(int msgId) {
  unsigned char buff[UART_SEND_MAX] = {0};
  int i;

  // 写入消息头
  for (i = 0; i < UART_MSG_HEAD_LEN; i++) {
    buff[i] = UART_HEAD[i];
  }
  // 写入消息号
  buff[2] = (unsigned char)msgId;
  // 写入消息尾
  for (i = 0; i < UART_MSG_FOOT_LEN; i++) {
    buff[i + 3] = UART_FOOT[i];
  }

  uartSend(buff, 5);
}

void voiceReportInt(int value) {
  uart_param_t param;
  unsigned char buff[UART_SEND_MAX] = {0};
  int i;

  // 写入消息头
  for (i = 0; i < UART_MSG_HEAD_LEN; i++) {
    buff[i] = UART_HEAD[i];
  }
  // 写入消息号
  buff[2] = U_MSG_BOZHENSHSGU;
  // 写入整数值
  param.d_int = value;
  int16_to_int32(&param);
  buff[3] = param.d_ucs[0];
  buff[4] = param.d_ucs[1];
  buff[5] = 0;
  buff[6] = 0;
  // 写入消息尾
  for (i = 0; i < UART_MSG_FOOT_LEN; i++) {
    buff[i + 7] = UART_FOOT[i];
  }

  uartSend(buff, 9);
}

int voiceAvailable() {
  return voiceSerial.available();
}

int voiceRead() {
  return voiceSerial.read();
}

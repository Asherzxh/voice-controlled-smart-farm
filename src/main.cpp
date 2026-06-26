/*
 * main.cpp
 * Voice-Controll-Smart-Farm
 * 语音控制智能农场系统 — 主程序入口
 *
 * 模块划分:
 *   config.h     — 引脚定义、常量、指令号
 *   voice_uart   — 语音模块串口通信 (发送/接收)
 *   sensors      — 传感器数据采集 (DHT11/光敏/水滴/土壤/水位/超声波)
 *   actuators    — 执行器控制 (LED/风扇/蜂鸣器/舵机/继电器/LCD)
 *   commands     — 语音命令分发与执行
 *
 * 原始代码来源: https://www.keyesrobot.cn/
 * 编译平台: ESP32 + PlatformIO (Arduino framework)
 */
#include <Arduino.h>
#include "config.h"
#include "sensors.h"
#include "actuators.h"
#include "commands.h"
#include "voice_uart.h"

// 全局传感器数据
static SensorData_t sensorData;

// 传感器 LCD 显示页面轮询
static int          sensorPage      = 0;
static unsigned long lastPageSwitch = 0;
#define PAGE_INTERVAL  3000   // 每 3 秒切换一页
#define PAGE_COUNT     4      // 共 4 页

// 距离过近自动播报冷却
static unsigned long lastDistAlertTime = 0;
static int           distNearCount     = 0;      // 连续检测到"近"的次数 (过滤噪声)
static bool          distWasNear       = false;  // 上一次触发状态 (防重复触发)
#define DIST_NEAR_CONFIRM_COUNT  3              // 连续 N 次确认才触发 (防USB噪声误报)

void setup() {
  Serial.begin(9600);

  // 按顺序初始化各模块
  sensorsInit();
  actuatorsInit();
  commandsInit();

  // LCD 初始显示标题，2 秒后切入传感器轮显
  lcdShowTitle();
  delay(2000);
}

void loop() {
  // 1. 读取所有传感器
  sensorsReadAll(&sensorData);
  #if 1  // 调试打印 (发布时关闭)
  Serial.printf("温度: %d °C, 湿度: %d %%, 光照: %d, 雨滴: %d %%, 土壤: %d %%, 水位: %d, 距离: %.1f cm\n",
                sensorData.temperature, sensorData.humidity, sensorData.lightValue,
                sensorData.rainPercent, sensorData.soilPercent, sensorData.waterLevel, sensorData.distance);
  #endif
  // 2. 轮流显示传感器数据 (每 PAGE_INTERVAL 毫秒切换一页)
  unsigned long now = millis();
  if (now - lastPageSwitch >= PAGE_INTERVAL) {
    lastPageSwitch = now;
    sensorPage = (sensorPage + 1) % PAGE_COUNT;
  }
  lcdShowSensorPage(&sensorData, sensorPage);

  // 3. 轮询语音命令并执行
  commandsPollAndExecute(&sensorData);

  // 4. 自动检测: 距离过近 → 播报"有人经过"
  {
    bool distNearNow = (sensorData.distance > 0 && sensorData.distance < DIST_NEAR_THRESHOLD);
    unsigned long nowDist = millis();

    // 连续确认计数: 累加或清零 (USB供电噪声会产生单次毛刺, 真人会持续触发)
    if (distNearNow) {
      distNearCount++;
    } else {
      distNearCount = 0;
    }

    // 连续 N 次确认 + 冷却时间已过 → 触发播报
    if (distNearCount >= DIST_NEAR_CONFIRM_COUNT
        && !distWasNear
        && (nowDist - lastDistAlertTime >= DIST_ALERT_COOLDOWN_MS)) {
      lastDistAlertTime = nowDist;
      distNearCount = 0;  // 防止同一事件重复触发

      // 语音播报"有人经过"
      voiceBroadcast(U_MSG_PERSON_PASSING);
      buzzerPlayScale();  // 同时用蜂鸣器提示
      // 同时在 LCD 上短暂提示
      lcdShowLine(1, "Someone passing!");

      Serial.println("[Auto] Distance near — person passing alert");
    }
    distWasNear = (distNearCount >= DIST_NEAR_CONFIRM_COUNT);
  }

  // 5. 关闭蜂鸣器 (避免持续鸣响)
  buzzerStop();
}

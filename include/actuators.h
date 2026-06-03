/*
 * actuators.h
 * 执行器控制模块 — LED、风扇、蜂鸣器、舵机、继电器
 */
#ifndef ACTUATORS_H
#define ACTUATORS_H

#include <Arduino.h>
#include "sensors.h"

// 初始化所有执行器引脚和 PWM
void actuatorsInit();

// ===================== LED =====================
void ledSetBrightness(int brightness);  // 0~255

// ===================== 风扇 =====================
void fanSetSpeed(int speed);   // 0 = 停, 100 = 慢, 200 = 快
void fanStop();

// ===================== 蜂鸣器 =====================
// 播放升阶音符 (do re mi fa sol la si)
void buzzerPlayScale();
void buzzerStop();

// ===================== 舵机 (喂食) =====================
void servoFeedOpen();    // 打开喂食 (80°)
void servoFeedClose();   // 关闭喂食 (180°)

// ===================== 继电器 (水泵) =====================
void pumpOn();
void pumpOff();
void pumpRun(unsigned long durationMs);  // 运行指定时间后自动关闭

// ===================== LCD =====================
void lcdInit();
void lcdShowTitle();                     // 显示 "Voice Controll Smart Farm"
void lcdShowLine(int row, const char* text);  // 显示一行文字
void lcdClear();
void lcdShowSensorPage(const SensorData_t* data, int page);  // 轮流显示传感器数据

#endif // ACTUATORS_H

/*
 * commands.h
 * 语音命令分发模块 — 接收语音指令并调用对应执行器
 */
#ifndef COMMANDS_H
#define COMMANDS_H

#include <Arduino.h>
#include "sensors.h"

// 初始化命令模块 (初始化语音串口)
void commandsInit();

// 轮询语音串口，有命令则执行，返回命令号 (0 = 无命令)
int commandsPollAndExecute(const SensorData_t* data);

// 直接处理一个已知命令号 (供外部调用)
void commandsHandleCommand(int cmd, const SensorData_t* data);

// 自动灌溉检查 (土壤过干 + 水池有水时报警)
void commandsCheckAutoIrrigation(const SensorData_t* data);

#endif // COMMANDS_H

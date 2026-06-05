/*
 * commands.h
 * 语音命令分发模块 — 接收语音指令并调用对应执行器
 * 新增: 全传感器异常评估 / 智能建议 / 手动实施
 */
#ifndef COMMANDS_H
#define COMMANDS_H

#include <Arduino.h>
#include "sensors.h"
#include "config.h"

// 初始化命令模块 (初始化语音串口)
void commandsInit();

// 轮询语音串口，有命令则执行，返回命令号 (0 = 无命令)
int commandsPollAndExecute(const SensorData_t* data);

// 直接处理一个已知命令号 (供外部调用)
void commandsHandleCommand(int cmd, const SensorData_t* data);

// ===================== 新增: 异常评估 =====================

// 评估所有传感器数据，填写 EvalResult_t
EvalResult_t commandsEvaluate(const SensorData_t* data);

// 获取评估结果的中文描述 (用于 LCD 显示)
// idx: 0~9 对应各异常项, 返回描述文字; 无更多异常时返回 NULL
const char* commandsGetWarningText(const EvalResult_t* eval, int idx);

#endif // COMMANDS_H

/*
 * sensors.h
 * 传感器读取模块 — 统一管理所有传感器数据采集
 */
#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>

// 传感器数据结构体 — 一次读取全部数据
typedef struct {
  int   temperature;      // DHT11 温度 (°C)
  int   humidity;         // DHT11 空气湿度 (%)
  int   lightValue;       // 光敏传感器原始值
  int   rainPercent;      // 水滴传感器百分比 (%)
  int   soilRaw;          // 土壤湿度原始值
  int   soilPercent;      // 土壤湿度百分比 (%)
  int   waterLevel;       // 水位传感器原始值
  float distance;         // 超声波距离 (cm)
} SensorData_t;

// 初始化所有传感器引脚
void sensorsInit();

// 读取全部传感器数据 (每 loop 调用一次)
void sensorsReadAll(SensorData_t* data);

// 单独读取超声波距离 (可被 commands 模块单独调用)
float ultrasonicGetDistance();

#endif // SENSORS_H

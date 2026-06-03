/*
 * sensors.cpp
 * 传感器读取模块实现
 */
#include "sensors.h"
#include "config.h"
#include <dht11.h>

static dht11 DHT11;

void sensorsInit() {
  pinMode(PIN_PHOTOSENSOR,   INPUT);
  pinMode(PIN_STEAM,         INPUT);
  pinMode(PIN_SOIL_HUMIDITY, INPUT);
  pinMode(PIN_WATER_LEVEL,   INPUT);
  pinMode(PIN_TRIG,          OUTPUT);
  pinMode(PIN_ECHO,          INPUT);
}

void sensorsReadAll(SensorData_t* data) {
  // DHT11 温湿度
  int chk = DHT11.read(PIN_DHT11);
  data->temperature = DHT11.temperature;
  data->humidity    = DHT11.humidity;

  // 光敏传感器
  data->lightValue = analogRead(PIN_PHOTOSENSOR);

  // 水滴传感器 → 百分比
  int steamRaw = analogRead(PIN_STEAM);
  Serial.printf("水滴传感器原始值: %d\n", steamRaw);
  data->rainPercent = (int)round(((4096-steamRaw) / 4096.0) * 100);

  // 土壤湿度
  data->soilRaw = analogRead(PIN_SOIL_HUMIDITY);
  data->soilPercent = (int)round((data->soilRaw / 4096.0) * 100);

  // 水位传感器
  data->waterLevel = analogRead(PIN_WATER_LEVEL);

  // 超声波距离
  data->distance = ultrasonicGetDistance();
}

float ultrasonicGetDistance() {
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  long duration = pulseIn(PIN_ECHO, HIGH);
  float dist = duration / 58.0;
  delay(50);
  return dist;
}

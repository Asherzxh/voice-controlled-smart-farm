/*
 * actuators.cpp
 * 执行器控制模块实现 — LED、风扇、蜂鸣器、舵机、继电器、LCD
 */
#include "actuators.h"
#include "config.h"
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <esp32Servo.h>

// ===================== 硬件对象 =====================
static LiquidCrystal_I2C lcd(0x27, 16, 2);
static Servo feedServo;

// ===================== LEDC 通道分配 =====================
#define LEDC_CH_LED     1     // LED PWM (避开 Servo 库占用的通道 0)
// BUZZER_CHANNEL (4) 已在 config.h 定义，用于蜂鸣器

// ===================== 初始化 =====================

void actuatorsInit() {
  // --- LED PWM ---
  ledcSetup(LEDC_CH_LED, 5000, 8);
  ledcAttachPin(PIN_LED, LEDC_CH_LED);
  ledcWrite(LEDC_CH_LED, 0);

  // --- 蜂鸣器 PWM ---
  ledcSetup(BUZZER_CHANNEL, 5000, 8);
  ledcAttachPin(PIN_BUZZER, BUZZER_CHANNEL);
  ledcWrite(BUZZER_CHANNEL, 0);

  // --- 风扇电机 (TB6612 / L298N) ---
  pinMode(PIN_MOTOR_IN1, OUTPUT);
  pinMode(PIN_MOTOR_IN2, OUTPUT);
  digitalWrite(PIN_MOTOR_IN1, LOW);
  digitalWrite(PIN_MOTOR_IN2, LOW);

  // --- 舵机 ---
  feedServo.attach(PIN_SERVO);
  feedServo.write(180);  // 初始位置: 关闭

  // --- 继电器 (水泵) ---
  pinMode(PIN_RELAY, OUTPUT);
  digitalWrite(PIN_RELAY, LOW);

  // --- LCD ---
  lcdInit();
}

// ===================== LCD =====================

void lcdInit() {
  Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
  lcd.init();
  lcd.backlight();
  lcd.clear();
}

void lcdShowTitle() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Voice Controll");
  lcd.setCursor(0, 1);
  lcd.print("Smart Farm");
}

void lcdShowLine(int row, const char* text) {
  lcd.setCursor(0, row);
  lcd.print(text);
  // 用空格清除该行剩余字符
  int len = strlen(text);
  for (int i = len; i < 16; i++) {
    lcd.print(' ');
  }
}

void lcdClear() {
  lcd.clear();
}

void lcdShowSensorPage(const SensorData_t* data, int page) {
  char buf[17];

  switch (page) {
    case 0:
      // 温度 + 湿度
      snprintf(buf, sizeof(buf), "Temp: %d C", data->temperature);
      lcd.setCursor(0, 0);
      lcd.print(buf);
      for (int i = strlen(buf); i < 16; i++) lcd.print(' ');

      snprintf(buf, sizeof(buf), "Humi: %d %%", data->humidity);
      lcd.setCursor(0, 1);
      lcd.print(buf);
      for (int i = strlen(buf); i < 16; i++) lcd.print(' ');
      break;

    case 1:
      // 光照 + 雨量
      snprintf(buf, sizeof(buf), "Light: %d", data->lightValue);
      lcd.setCursor(0, 0);
      lcd.print(buf);
      for (int i = strlen(buf); i < 16; i++) lcd.print(' ');

      snprintf(buf, sizeof(buf), "Rain: %d %%", data->rainPercent);
      lcd.setCursor(0, 1);
      lcd.print(buf);
      for (int i = strlen(buf); i < 16; i++) lcd.print(' ');
      break;

    case 2:
      // 土壤湿度 + 水位
      snprintf(buf, sizeof(buf), "Soil: %d %%", data->soilPercent);
      lcd.setCursor(0, 0);
      lcd.print(buf);
      for (int i = strlen(buf); i < 16; i++) lcd.print(' ');

      snprintf(buf, sizeof(buf), "Water: %d", data->waterLevel);
      lcd.setCursor(0, 1);
      lcd.print(buf);
      for (int i = strlen(buf); i < 16; i++) lcd.print(' ');
      break;

    case 3:
      // 超声波距离
      snprintf(buf, sizeof(buf), "Dist: %.1f cm", data->distance);
      lcd.setCursor(0, 0);
      lcd.print(buf);
      for (int i = strlen(buf); i < 16; i++) lcd.print(' ');

      snprintf(buf, sizeof(buf), "All Sensors OK");
      lcd.setCursor(0, 1);
      lcd.print(buf);
      for (int i = strlen(buf); i < 16; i++) lcd.print(' ');
      break;

    default:
      break;
  }
}

// ===================== LED =====================

void ledSetBrightness(int brightness) {
  ledcWrite(LEDC_CH_LED, constrain(brightness, 0, 255));
}

// ===================== 风扇 =====================

void fanSetSpeed(int speed) {
  if (speed <= 0) {
    fanStop();
    return;
  }
  // 风扇直接用数字引脚高低电平控制
  digitalWrite(PIN_MOTOR_IN1, HIGH);
  digitalWrite(PIN_MOTOR_IN2, LOW);
}

void fanStop() {
  digitalWrite(PIN_MOTOR_IN1, LOW);
  digitalWrite(PIN_MOTOR_IN2, LOW);
}

// ===================== 蜂鸣器 =====================

void buzzerPlayScale() {
  // do re mi fa sol la si
  const int notes[] = {262, 294, 330, 349, 392, 440, 494};
  for (int i = 0; i < 7; i++) {
    ledcWriteTone(BUZZER_CHANNEL, notes[i]);
    delay(250);
    ledcWriteTone(BUZZER_CHANNEL, 0);
    delay(50);
  }
  ledcWrite(BUZZER_CHANNEL, 0);
}

void buzzerStop() {
  ledcWrite(BUZZER_CHANNEL, 0);
}

// ===================== 舵机 (喂食) =====================

void servoFeedOpen() {
  feedServo.write(80);   // 打开喂食口
}

void servoFeedClose() {
  feedServo.write(180);  // 关闭喂食口
}

// ===================== 继电器 (水泵) =====================

void pumpOn() {
  digitalWrite(PIN_RELAY, HIGH);
}

void pumpOff() {
  digitalWrite(PIN_RELAY, LOW);
}

void pumpRun(unsigned long durationMs) {
  pumpOn();
  delay(durationMs);
  pumpOff();
}

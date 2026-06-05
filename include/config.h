/*
 * config.h
 * 引脚定义、常量、全局配置
 */
#ifndef CONFIG_H
#define CONFIG_H

// ===================== 引脚定义 =====================
// 传感器引脚
const int PIN_PHOTOSENSOR   = 34;   // 光敏传感器 (ADC)
const int PIN_STEAM         = 35;   // 水滴传感器 (ADC)
const int PIN_SOIL_HUMIDITY = 32;   // 土壤湿度传感器 (ADC)
const int PIN_WATER_LEVEL   = 33;   // 水位传感器 (ADC)
const int PIN_DHT11         = 17;   // DHT11 温湿度传感器
const int PIN_TRIG          = 14;   // 超声波 Trig
const int PIN_ECHO          = 13;   // 超声波 Echo

// 执行器引脚
const int PIN_LED           = 27;   // LED
const int PIN_BUZZER        = 16;   // 无源蜂鸣器
const int PIN_MOTOR_IN1     = 19;   // 风扇电机 IN+
const int PIN_MOTOR_IN2     = 18;   // 风扇电机 IN-
const int PIN_SERVO         = 26;   // 舵机
const int PIN_RELAY         = 25;   // 继电器 (水泵)

// 软件串口引脚 (语音模块)
const int PIN_UART_RX       = 23;
const int PIN_UART_TX       = 5;

// I2C 引脚 (LCD)
const int PIN_I2C_SDA       = 21;
const int PIN_I2C_SCL       = 22;

// ===================== PWM 通道 =====================
const int BUZZER_CHANNEL    = 4;    // LEDC 通道

// ===================== LED 亮度等级 =====================
const int LED_OFF    = 0;
const int LED_DIM    = 50;
const int LED_MEDIUM = 150;
const int LED_BRIGHT = 255;

// ===================== 语音指令定义 =====================
// LED 控制
const int CMD_LED_ON_MEDIUM  = 1;   // 开灯 (中等亮度)
const int CMD_LED_OFF        = 2;   // 关灯
const int CMD_LED_BRIGHT     = 3;   // 最亮
const int CMD_LED_DIM        = 4;   // 暗亮

// 风扇控制
const int CMD_FAN_ON         = 5;   // 开风扇 (低速)
const int CMD_FAN_OFF        = 6;   // 关风扇
const int CMD_FAN_FAST       = 7;   // 风大
const int CMD_FAN_SLOW       = 8;   // 风小

// 灌溉
const int CMD_PUMP_ON        = 9;   // 继电器控制水泵
const int CMD_PUMP_OFF       = 10;  // 关闭水泵

// 音乐
const int CMD_MUSIC_ON       = 11;  // 播放音乐
const int CMD_MUSIC_OFF      = 12;  // 关闭音乐

// 喂食舵机
const int CMD_FEED_OPEN      = 13;  // 开始喂食
const int CMD_FEED_CLOSE     = 14;  // 停止喂食

// 语音播报
const int CMD_REPORT_TEMP    = 15;  // 播报温度
const int CMD_REPORT_HUMI    = 16;  // 播报湿度
const int CMD_REPORT_LIGHT   = 17;  // 播报光照
const int CMD_REPORT_RAIN    = 18;  // 播报雨水量
const int CMD_REPORT_DIST    = 19;  // 播报距离
const int CMD_REPORT_SOIL    = 20;  // 播报土壤湿度
const int CMD_REPORT_WATER   = 21;  // 播报水位

// 综合指令
const int CMD_REPORT_ALL     = 22;  // 播报全部信息
const int CMD_GET_ADVICE     = 23;  // 根据信息提建议
const int CMD_EXEC_ADVICE    = 24;  // 实施建议

// ===================== 灌溉阈值 =====================
const int SOIL_DRY_THRESHOLD = 15;   // 土壤湿度低于此值需要灌溉
const int WATER_MIN_LEVEL    = 800;  // 水池最低水位

// ===================== 串口协议常量 =====================
const unsigned char UART_HEAD[] = {0xaa, 0x55};
const unsigned char UART_FOOT[] = {0x55, 0xaa};
const int UART_SEND_MAX      = 32;
const int UART_MSG_HEAD_LEN  = 2;
const int UART_MSG_FOOT_LEN  = 2;

// 语音模块消息号
const int U_MSG_BOZHENSHSGU  = 1;   // 播报整数
const int U_MSG_BOXIAOSHU    = 2;   // 播报小数
const int U_MSG_BOBAO1       = 3;   // 播报1 (温度)
const int U_MSG_BOBAO2       = 4;   // 播报2 (摄氏度)
const int U_MSG_BOBAO3       = 5;   // 播报3 (雨水量)
const int U_MSG_BOBAO4       = 6;   // 播报4 (空气湿度)
const int U_MSG_BOBAO5       = 7;   // 播报5 (光照)
const int U_MSG_BOBAO6       = 8;   // 播报6 (土壤湿度)
const int U_MSG_BOBAO7       = 9;   // 播报7 (水深)
const int U_MSG_BOBAO9       = 10;  // 播报9 (距离)
const int U_MSG_BOBAO16      = 14;  // 播报16 (厘米)
const int U_MSG_BOBAO17      = 11;  // 播报17 (温度过高)
const int U_MSG_BOBAO18      = 12;  // 播报18 (土壤湿度过低)      
const int U_MSG_BOBAO19      = 13;  // 播报19 (一切正常)          

#endif // CONFIG_H

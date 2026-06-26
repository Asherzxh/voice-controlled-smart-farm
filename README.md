# 基于语音识别模块的智能农场

> Voice-Controlled Smart Farm — ESP32 + SU-03T 语音识别模块

## 项目简介

基于 ESP32 和 SU-03T 语音识别模块的智能农场控制系统，支持**语音命令**控制各类传感器采集和执行器操作，实现农场环境的智能监控与管理。

## 功能特性

### 🎤 语音控制
- 支持 24 条语音指令（LED、风扇、水泵、舵机、音乐、数据播报等）
- 自动语音播报传感器数据
- 距离检测 → 自动播报"有人经过"

### 🌡️ 传感器采集
| 传感器 | 功能 | 引脚 |
|--------|------|------|
| DHT11 | 温湿度采集 | GPIO 17 |
| 光敏传感器 | 光照强度 (ADC) | GPIO 34 |
| 水滴传感器 | 雨量检测 (ADC) | GPIO 35 |
| 土壤湿度传感器 | 土壤湿度 (ADC) | GPIO 32 |
| 水位传感器 | 水位检测 (ADC) | GPIO 33 |
| 超声波 (HC-SR04) | 距离检测 | Trig 14, Echo 13 |

### 🔧 执行器
| 执行器 | 功能 | 引脚 |
|--------|------|------|
| LED | 补光灯 | GPIO 27 |
| 无源蜂鸣器 | 提示音 / 音乐 | GPIO 16 |
| 风扇电机 | 降温通风 | GPIO 19, 18 |
| 舵机 | 自动喂食 | GPIO 26 |
| 继电器 | 水泵灌溉 | GPIO 25 |
| LCD1602 (I2C) | 数据显示 | SDA 21, SCL 22 |

### 📟 LCD 显示
- 4 页轮播显示传感器数据（温度/湿度/光照/雨量/土壤/水位/距离）

### ⚠️ 异常检测与建议
- 温度过高/过低 → 自动诊断建议
- 湿度过高/过低 → 通风/加湿提醒
- 土壤过干 → 建议灌溉
- 水位过低 → 提醒加水
- 光照过暗/过强 → 补光/遮光建议
- 距离过近 → 自动播报提醒

## 语音指令列表

| 编号 | 指令 | 功能 |
|------|------|------|
| 1 | 开灯 | LED 中等亮度 |
| 2 | 关灯 | 关闭 LED |
| 3 | 最亮 | LED 最高亮度 |
| 4 | 暗亮 | LED 最低亮度 |
| 5 | 开风扇 | 风扇低速 |
| 6 | 关风扇 | 关闭风扇 |
| 7 | 风大 | 风扇高速 |
| 8 | 风小 | 风扇低速 |
| 9 | 开灌溉 | 水泵开启 |
| 10 | 关灌溉 | 水泵关闭 |
| 11 | 放音乐 | 蜂鸣器播放旋律 |
| 12 | 关音乐 | 停止播放 |
| 13 | 喂食 | 舵机开 |
| 14 | 关喂食 | 舵机关 |
| 15-21 | 播报温度/湿度/光照/雨量/距离/土壤/水位 | 语音播报 |
| 22 | 播报全部 | 全部数据播报 |
| 23 | 给建议 | 异常诊断建议 |
| 24 | 实施建议 | 自动执行建议操作 |

## 硬件平台

- **主控芯片**: ESP32 (ESP32-WROOM-32)
- **语音模块**: SU-03T (串口通信, GPIO 23 RX / GPIO 5 TX)
- **显示屏**: LCD1602 + I2C 转接板
- **开发框架**: Arduino (PlatformIO)

## 编译与烧录

### 环境要求
- [PlatformIO IDE](https://platformio.org/)（VS Code 插件）或 PlatformIO CLI

### 依赖库
```
marcoschwartz/LiquidCrystal_I2C@^1.1.4
adafruit/DHT sensor library@^1.4.7
plerup/EspSoftwareSerial@^8.2.0
```

### 编译
```bash
pio run
```

### 烧录
```bash
pio run --target upload
```

### 串口监视
```bash
pio device monitor -b 9600
```

## 项目结构

```
├── platformio.ini          # PlatformIO 项目配置
├── include/
│   ├── config.h            # 引脚定义、常量、指令号
│   ├── sensors.h           # 传感器模块声明
│   ├── actuators.h         # 执行器模块声明
│   ├── commands.h          # 命令处理模块声明
│   └── voice_uart.h        # 语音串口模块声明
├── src/
│   ├── main.cpp            # 主程序入口
│   ├── sensors.cpp         # 传感器数据采集
│   ├── actuators.cpp       # 执行器控制
│   ├── commands.cpp        # 语音命令分发执行
│   └── voice_uart.cpp      # 语音模块串口通信
├── lib/                    # 第三方库
└── test/                   # 测试代码
```

## 语音模块配置

SU-03T 语音模块固件需在 [smartpi.cn](https://smartpi.cn) 平台配置对应的语音指令码（消息号 1~20），确保与 `config.h` 中的 `U_MSG_*` 宏定义一致。

## 参考来源

原始代码参考: [keyesrobot.cn](https://www.keyesrobot.cn/)

## License

MIT License

/*
 * commands.cpp
 * 语音命令分发模块实现
 * 将原 loop() 中的巨大 if 链拆分为清晰的命令处理
 *
 * 新增:
 *   - commandsEvaluate()     全传感器异常评估
 *   - cmdGetAdvice()         多条件综合建议 (纯语音播报)
 *   - cmdExecAdvice()        语音触发手动实施 (执行器+蜂鸣器)
 */
#include "commands.h"
#include "config.h"
#include "voice_uart.h"
#include "actuators.h"

// ===================== 内部命令处理函数 =====================

// LED 控制命令
static void cmdLedControl(int cmd) {
  switch (cmd) {
    case CMD_LED_ON_MEDIUM: ledSetBrightness(LED_MEDIUM); break;
    case CMD_LED_OFF:       ledSetBrightness(LED_OFF);    break;
    case CMD_LED_BRIGHT:    ledSetBrightness(LED_BRIGHT); break;
    case CMD_LED_DIM:       ledSetBrightness(LED_DIM);    break;
  }
}

// 风扇控制命令
static void cmdFanControl(int cmd) {
  switch (cmd) {
    case CMD_FAN_ON:   fanSetSpeed(100); break;
    case CMD_FAN_FAST: fanSetSpeed(200); break;
    case CMD_FAN_SLOW: fanSetSpeed(100); break;
    case CMD_FAN_OFF:  fanStop();        break;
  }
}

// 音乐控制命令
static void cmdMusicControl(int cmd) {
  switch (cmd) {
    case CMD_MUSIC_ON:  buzzerPlayScale(); break;
    case CMD_MUSIC_OFF: buzzerStop();      break;
  }
}

// 喂食控制命令
static void cmdFeedControl(int cmd) {
  switch (cmd) {
    case CMD_FEED_OPEN:  servoFeedOpen();  break;
    case CMD_FEED_CLOSE: servoFeedClose(); break;
  }
}

// 水泵控制命令
static void cmdPumpControl(int cmd, const SensorData_t* data) {
  switch (cmd) {
    case CMD_PUMP_ON:
      pumpRun(3000);
      break;
    case CMD_PUMP_OFF:
      pumpOff();
      break;
  }
}


// 单项播报命令 (温度/湿度/光照/雨水/距离/土壤/水位)
static void cmdReportSingle(int cmd, const SensorData_t* data) {
  switch (cmd) {
    case CMD_REPORT_TEMP:
      delay(2000);
      voiceBroadcast(U_MSG_BOBAO1);
      delay(2000);
      voiceReportInt(data->temperature);
      delay(2000);
      voiceBroadcast(U_MSG_BOBAO2);
      delay(2000);
      break;

    case CMD_REPORT_HUMI:
      delay(2000);
      voiceBroadcast(U_MSG_BOBAO4);
      delay(2000);
      voiceReportInt(data->humidity);
      delay(2000);
      break;

    case CMD_REPORT_LIGHT:
      delay(3000);
      voiceBroadcast(U_MSG_BOBAO5);
      delay(3000);
      voiceReportInt(data->lightValue);
      delay(2000);
      break;

    case CMD_REPORT_RAIN:
      delay(3000);
      voiceBroadcast(U_MSG_BOBAO3);
      delay(3000);
      voiceReportInt(data->rainPercent);
      delay(2000);
      break;

    case CMD_REPORT_DIST:
      delay(2000);
      voiceBroadcast(U_MSG_BOBAO9);
      delay(2000);
      voiceReportInt((int)data->distance);
      delay(1000);
      voiceBroadcast(U_MSG_BOBAO16);
      delay(2000);
      break;

    case CMD_REPORT_SOIL:
      delay(3000);
      voiceBroadcast(U_MSG_BOBAO6);
      delay(3000);
      voiceReportInt(data->soilPercent);
      delay(2000);
      break;

    case CMD_REPORT_WATER:
      delay(2000);
      voiceBroadcast(U_MSG_BOBAO7);
      delay(2000);
      voiceReportInt(data->waterLevel);
      delay(2000);
      break;
  }
}

// 综合播报: 播报全部传感器信息
static void cmdReportAll(const SensorData_t* data) {
  delay(2000);
  voiceBroadcast(U_MSG_BOBAO5);
  delay(2000);
  voiceReportInt(data->lightValue);
  delay(2000);

 delay(2000);
      voiceBroadcast(U_MSG_BOBAO1);
      delay(2000);
      voiceReportInt(data->temperature);
      delay(2000);
      voiceBroadcast(U_MSG_BOBAO2);
      delay(2000);

  voiceBroadcast(U_MSG_BOBAO6);
  delay(2000);
  voiceReportInt(data->soilPercent);
  delay(2000);

  voiceBroadcast(U_MSG_BOBAO4);
  delay(2000);
  voiceReportInt(data->humidity);
  delay(2000);

  voiceBroadcast(U_MSG_BOBAO3);
  delay(2000);
  voiceReportInt(data->rainPercent);
  delay(2000);

  voiceBroadcast(U_MSG_BOBAO7);
  delay(2000);
  voiceReportInt(data->waterLevel);
  delay(2000);
}

// ===================== 新增: 全传感器异常评估 =====================

EvalResult_t commandsEvaluate(const SensorData_t* data) {
  EvalResult_t eval;
  memset(&eval, 0, sizeof(eval));

  // 温度评估
  if (data->temperature > TEMP_HIGH_THRESHOLD) {
    eval.tempHigh = true;
  } else if (data->temperature < TEMP_LOW_THRESHOLD) {
    eval.tempLow = true;
  }

  // 湿度评估 (DHT11 正常时有效)
  if (data->humidity > 0) {
    if (data->humidity > HUMI_HIGH_THRESHOLD) {
      eval.humiHigh = true;
    } else if (data->humidity < HUMI_LOW_THRESHOLD) {
      eval.humiLow = true;
    }
  } else {
    eval.dhtError = true;  // DHT11 读数异常
  }

  // 光照评估
  if (data->lightValue < LIGHT_LOW_THRESHOLD) {
    eval.lightDark = true;
  } else if (data->lightValue > LIGHT_HIGH_THRESHOLD) {
    eval.lightStrong = true;
  }

  // 雨量评估
  if (data->rainPercent > RAIN_HEAVY_THRESHOLD) {
    eval.rainHeavy = true;
  }

  // 土壤湿度评估
  if (data->soilPercent <= SOIL_DRY_THRESHOLD) {
    eval.soilDry = true;
  }

  // 水位评估
  if (data->waterLevel < WATER_LOW_THRESHOLD) {
    eval.waterLow = true;
  }

  // 超声波距离评估
  if (data->distance > 0 && data->distance < DIST_NEAR_THRESHOLD) {
    eval.distNear = true;
  }

  // 汇总: 是否存在任一异常
  eval.anyWarning = (eval.tempHigh || eval.tempLow ||
                     eval.humiHigh || eval.humiLow ||
                     eval.lightDark || eval.lightStrong ||
                     eval.rainHeavy || eval.soilDry ||
                     eval.waterLow || eval.distNear ||
                     eval.dhtError);

  return eval;
}

// ===================== 智能建议: 全面多条件评估 =====================

static void cmdGetAdvice(const SensorData_t* data) {
  EvalResult_t eval = commandsEvaluate(data);

  if (!eval.anyWarning) {
    voiceBroadcast(U_MSG_BOBAO19);
    delay(2000);
    return;
  }

  // 按优先级排序异常并逐条播报
  // 优先级: 传感器故障 > 水位过低 > 土壤过干 > 雨量过大 > 温度异常 > 湿度异常 > 光照异常 > 距离过近

  if (eval.dhtError) {
    delay(2000);
    voiceBroadcast(U_MSG_DHT_ERROR);
    delay(2000);
  }
  Serial.print("我执行了接下来的评估");
  if (eval.waterLow) {
    delay(2000);
    voiceBroadcast(U_MSG_WATER_LOW);
    delay(2000);
  }
  if (eval.soilDry) {
      delay(2000);
    voiceBroadcast(U_MSG_BOBAO18);
    delay(2000);
  }
  if (eval.rainHeavy) {
      delay(2000);
    voiceBroadcast(U_MSG_RAIN_HEAVY);
    delay(2000);
  }
  if (eval.tempHigh) {
      delay(2000);
    voiceBroadcast(U_MSG_BOBAO17);
    delay(2000);
  }

 
  if (eval.lightDark) {
    delay(2000);
    voiceBroadcast(U_MSG_LIGHT_DARK);
    delay(2000);
  }

  if (eval.distNear) {
    delay(2000);
    voiceBroadcast(U_MSG_DIST_NEAR);
    delay(2000);
  }
}

// ===================== 执行建议: 手动实施 =====================

static void cmdExecAdvice(const SensorData_t* data) {
  EvalResult_t eval = commandsEvaluate(data);
  bool didSomething = false;

  // --- P0 级: 传感器故障报警 ---
  if (eval.dhtError) {
    buzzerPlayScale();
    delay(500);
    didSomething = true;
  }

  // --- P1 级: 灌溉 (土壤干 + 有水) ---
  if (eval.soilDry && data->waterLevel >= WATER_MIN_LEVEL) {
    pumpRun(3000);
    didSomething = true;
  } else if (eval.soilDry && data->waterLevel < WATER_MIN_LEVEL) {
    ledcWriteTone(BUZZER_CHANNEL, 494);
    delay(500);
    ledcWrite(BUZZER_CHANNEL, 0);
    didSomething = true;
  }

  // --- P2 级: 降温 (温度过高) ---
  if (eval.tempHigh) {
    fanSetSpeed(200);
    delay(3000);
    fanStop();
    didSomething = true;
  }



  // --- P4 级: 除湿 (湿度太高) ---
  if (eval.humiHigh) {
    fanSetSpeed(100);
    delay(5000);
    fanStop();
    didSomething = true;
  }



  // --- P6 级: 光照补偿 (太暗开灯) ---
  if (eval.lightDark) {
    ledSetBrightness(LED_MEDIUM);
    didSomething = true;
  }



  // --- P8 级: 雨量过大 ---
  if (eval.rainHeavy) {
    fanSetSpeed(100);
    delay(5000);
    fanStop();
    didSomething = true;
  }

  // --- P9 级: 水位过低 ---
  if (eval.waterLow) {
    ledcWriteTone(BUZZER_CHANNEL, 700);
    delay(500);
    ledcWrite(BUZZER_CHANNEL, 0);
    didSomething = true;
  }

  // --- P10 级: 物体靠近 ---
  if (eval.distNear) {
 
    didSomething = true;
  }

  if (!didSomething) {
    voiceBroadcast(U_MSG_BOBAO19);
    delay(1500);
  } 
}

// ===================== 辅助: 获取第 idx 个异常的文字描述 =====================

const char* commandsGetWarningText(const EvalResult_t* eval, int idx) {
  int count = 0;

  // 按优先级遍历
  if (eval->dhtError     && count++ == idx) return "DHT11 Error";
  if (eval->waterLow     && count++ == idx) return "Water Level Low";
  if (eval->soilDry      && count++ == idx) return "Soil Too Dry";
  if (eval->rainHeavy    && count++ == idx) return "Heavy Rain";
  if (eval->tempHigh     && count++ == idx) return "Temp Too High";
  if (eval->tempLow      && count++ == idx) return "Temp Too Low";
  if (eval->humiHigh     && count++ == idx) return "Humidity High";
  if (eval->humiLow      && count++ == idx) return "Humidity Low";
  if (eval->lightDark    && count++ == idx) return "Light Too Dim";
  if (eval->lightStrong  && count++ == idx) return "Light Strong";
  if (eval->distNear     && count++ == idx) return "Object Nearby";

  return NULL;  // 没有更多异常
}

// ===================== 公开接口 =====================

void commandsInit() {
  voiceUartInit();
}

int commandsPollAndExecute(const SensorData_t* data) {
  if (voiceAvailable() <= 0) {
    // 减少串口刷屏, 仅在调试时打开
     Serial.println("No voice command available.");
    return 0;
  }

  int cmd = voiceRead();
  Serial.printf("Voice CMD: %d\n", cmd);  // 调试输出

  commandsHandleCommand(cmd, data);
  return cmd;
}

void commandsHandleCommand(int cmd, const SensorData_t* data) {
  if (cmd == 0) return;

  // LED 控制
  if (cmd >= CMD_LED_ON_MEDIUM && cmd <= CMD_LED_DIM) {
    cmdLedControl(cmd);
    return;
  }

  // 风扇控制
  if (cmd >= CMD_FAN_ON && cmd <= CMD_FAN_OFF) {
    cmdFanControl(cmd);
    return;
  }

  // 音乐控制
  if (cmd == CMD_MUSIC_ON || cmd == CMD_MUSIC_OFF) {
    cmdMusicControl(cmd);
    return;
  }

  // 喂食控制
  if (cmd == CMD_FEED_OPEN || cmd == CMD_FEED_CLOSE) {
    cmdFeedControl(cmd);
    return;
  }

  // 水泵控制
  if (cmd == CMD_PUMP_ON || cmd == CMD_PUMP_OFF) {
    cmdPumpControl(cmd, data);
    return;
  }

  // 单项播报
  if (cmd >= CMD_REPORT_TEMP && cmd <= CMD_REPORT_WATER) {
    cmdReportSingle(cmd, data);
    return;
  }

  // 综合播报
  if (cmd == CMD_REPORT_ALL) {
    cmdReportAll(data);
    return;
  }

  // 智能建议 (全面评估)
  if (cmd == CMD_GET_ADVICE) {
    cmdGetAdvice(data);
    return;
  }

  // 执行建议 (全面实施)
  if (cmd == CMD_EXEC_ADVICE) {
    cmdExecAdvice(data);
    return;
  }
}

void commandsCheckAutoIrrigation(const SensorData_t* data) {
  if (data->soilPercent <= SOIL_DRY_THRESHOLD && data->waterLevel >= WATER_MIN_LEVEL) {
    ledcWriteTone(BUZZER_CHANNEL, 494);
    delay(200);
  }
}

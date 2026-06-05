/*
 * commands.cpp
 * 语音命令分发模块实现
 * 将原 loop() 中的巨大 if 链拆分为清晰的命令处理
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
      
        lcdClear();
        lcdShowLine(0, "Irrigating...");
        lcdShowLine(1, "Pump Working...");
        delay(1000);
        pumpRun(3000);
        lcdClear();
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
  lcdClear();
  lcdShowLine(0, "Reporting Info...");

  voiceBroadcast(U_MSG_BOBAO5);
  delay(2000);
  voiceReportInt(data->lightValue);
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

  lcdClear();
}

// 智能建议: 根据传感器数据给出建议
static void cmdGetAdvice(const SensorData_t* data) {
  lcdClear();
  lcdShowLine(0, "Analyzing...");
  delay(1000);

  if (data->soilPercent <= SOIL_DRY_THRESHOLD) {
    lcdShowLine(1, "Suggest: Water");
    voiceBroadcast(U_MSG_BOBAO18);
    delay(2000);
  } else if (data->temperature > 30) {
    lcdShowLine(1, "Suggest: Fan");
    voiceBroadcast(U_MSG_BOBAO17);
    delay(2000);
  } else {
    lcdShowLine(1, "System Normal");
    voiceBroadcast(U_MSG_BOBAO19);
    delay(2000);
  }
  lcdClear();
}

// 执行建议: 自动执行灌溉或降温
static void cmdExecAdvice(const SensorData_t* data) {

  if (data->soilPercent <= SOIL_DRY_THRESHOLD) {
    lcdShowLine(1, "Pump Working... ");
    pumpRun(3000);
  }
  if (data->temperature > 30) {
    lcdShowLine(1, "Fan Working...  ");
    fanSetSpeed(200);
    delay(3000);
    fanStop();
  }
 
}

// ===================== 公开接口 =====================

void commandsInit() {
  voiceUartInit();
}

int commandsPollAndExecute(const SensorData_t* data) {
  if (voiceAvailable() <= 0) {
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
  if (cmd == CMD_PUMP_ON) {
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

  // 智能建议
  if (cmd == CMD_GET_ADVICE) {
    cmdGetAdvice(data);
    return;
  }

  // 执行建议
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

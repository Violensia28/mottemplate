
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include "Config.h"
#include "PulseEngine.h"
#include "StorageManager.h"
#include "SafetyManager.h"
#include "SmartUI.h"
#include "ai/AIAdvisor.h"
#include "sense/SenseEngine.h"
#include "logging/Logger.h"

Adafruit_SSD1306 display(128, 64, &Wire, -1);
PulseEngine engine; StorageManager storage; SafetyManager safety; AIAdvisor ai; SenseEngine sense; Logger logger; SmartUI smart;

static int readEncoderDelta(){ static int8_t last=0; int a=digitalRead(PIN_ENC_A), b=digitalRead(PIN_ENC_B); int8_t v=(a<<1)|b, d=0; if ((last==0b00 && v==0b01) || (last==0b01 && v==0b11) || (last==0b11 && v==0b10) || (last==0b10 && v==0b00)) d=+1; if ((last==0b00 && v==0b10) || (last==0b10 && v==0b11) || (last==0b11 && v==0b01) || (last==0b01 && v==0b00)) d=-1; last=v; return d; }

void setup(){
  Serial.begin(115200);
  pinMode(PIN_ACTUATOR, OUTPUT);
  pinMode(PIN_ENC_A, INPUT_PULLUP); pinMode(PIN_ENC_B, INPUT_PULLUP); pinMode(PIN_ENC_BTN, INPUT_PULLUP);
  pinMode(PIN_TRIGGER, INPUT_PULLUP); pinMode(PIN_INTERLOCK, INPUT_PULLUP);
  pinMode(PIN_BUZZER, OUTPUT); digitalWrite(PIN_BUZZER, LOW);

  Wire.begin(PIN_OLED_SDA, PIN_OLED_SCL);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); display.clearDisplay(); display.display();

  storage.begin(); AppConfig cfg; storage.load(cfg);
  safety.begin(PIN_INTERLOCK, cfg.lockout_ms, cfg.cooldown_ms);
  ai.begin(); sense.begin(PIN_ADC_V, PIN_ADC_I, 10000); logger.begin();

  engine.begin(PIN_ACTUATOR, [&](PulseStage st){ smart.onStage(st); });
  smart.begin(&display, &engine, &storage, &safety, &ai, &sense, &logger);

  Serial.println(F("[BOOT] motsmartfirmware v0.3-SMART r1 (SAFE_SIM ON)"));
}

void loop(){
  int d = readEncoderDelta(); if (d!=0) smart.onRotate(d);
  static bool btnPrev = HIGH; bool btn = digitalRead(PIN_ENC_BTN);
  static uint32_t tDown=0; if (btnPrev==HIGH && btn==LOW) tDown=millis(); if (btnPrev==LOW && btn==HIGH){ uint32_t dur=millis()-tDown; if (dur>=LONG_PRESS_MS) smart.onLongPress(); else smart.onClick(); } btnPrev=btn;

  static bool trigPrev = HIGH; bool trg = digitalRead(PIN_TRIGGER); if (trigPrev==HIGH && trg==LOW) smart.onTrigger(); trigPrev=trg;

  engine.loop(); sense.tick(); smart.tick();
}

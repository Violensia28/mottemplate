
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
#ifdef SAFE_MODE
  Serial.printf("[CFG] SAFE_MODE (compile-time) = %d
", SAFE_MODE);
#else
  Serial.println("[CFG] SAFE_MODE (compile-time) = <not defined>");
#endif

#ifdef BUILD_TAG
  Serial.printf("[CFG] BUILD_TAG = %s
", BUILD_TAG);
#else
  Serial.println("[CFG] BUILD_TAG = <none>");
#endif

#ifndef BYPASS_SAFETY
  #define BYPASS_SAFETY 1  // 1 untuk uji buzzer via PC817; ubah 0 untuk produksi
#endif
extern bool g_safe_sim_on;  // pastikan variabel ini ada di project
#if BYPASS_SAFETY
  g_safe_sim_on = false;
  Serial.println("[SAFETY] BYPASS_SAFETY=1 -> FORCE SAFE_SIM OFF (TEST MODE)");
#endif
// ===== Ubah banner boot agar pakai runtime flag =====
Serial.printf("[BOOT] motsmartfirmware v0.3-SMART r1 (SAFE_SIM %s)
", g_safe_sim_on ? "ON" : "OFF");
// =======================================================================
#ifdef SAFE_MODE
  Serial.printf("[CFG] SAFE_MODE (compile-time) = %d\n", SAFE_MODE);
#else
  Serial.println("[CFG] SAFE_MODE (compile-time) = <not defined>");
#endif

#ifdef BUILD_TAG
  Serial.printf("[CFG] BUILD_TAG = %s\n", BUILD_TAG);
#else
  Serial.println("[CFG] BUILD_TAG = <none>");
#endif
#ifdef SAFE_MODE
  Serial.printf("[CFG] SAFE_MODE (compile-time) = %d\n", SAFE_MODE);
#else
  Serial.println("[CFG] SAFE_MODE (compile-time) = <not defined>");
#endif

#ifdef BUILD_TAG
  Serial.printf("[CFG] BUILD_TAG = %s\n", BUILD_TAG);
#else
  Serial.println("[CFG] BUILD_TAG = <none>");
#endif
#ifndef BYPASS_SAFETY
  #define BYPASS_SAFETY 0  // set 1 untuk uji buzzer via PC817
#endif

extern bool g_safe_sim_on; // pastikan variabel ini ada di project kamu

#if BYPASS_SAFETY
  g_safe_sim_on = false;
  Serial.println("[SAFETY] BYPASS_SAFETY=1 -> FORCE SAFE_SIM OFF (TEST MODE)");
#endif
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

  Serial.printf("[BOOT] motsmartfirmware v0.3-SMART r1 (SAFE_SIM %s)\n",
              g_safe_sim_on ? "ON" : "OFF");

void loop(){
  int d = readEncoderDelta(); if (d!=0) smart.onRotate(d);
  static bool btnPrev = HIGH; bool btn = digitalRead(PIN_ENC_BTN);
  static uint32_t tDown=0; if (btnPrev==HIGH && btn==LOW) tDown=millis(); if (btnPrev==LOW && btn==HIGH){ uint32_t dur=millis()-tDown; if (dur>=LONG_PRESS_MS) smart.onLongPress(); else smart.onClick(); } btnPrev=btn;

  static bool trigPrev = HIGH; bool trg = digitalRead(PIN_TRIGGER); if (trigPrev==HIGH && trg==LOW) smart.onTrigger(); trigPrev=trg;

  engine.loop(); sense.tick(); smart.tick();
}


#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Config.h"
#include "PulseEngine.h"
#include "StorageManager.h"
#include "SafetyManager.h"
#include "ai/AIAdvisor.h"
#include "sense/SenseEngine.h"
#include "logging/Logger.h"

enum class SmartView : uint8_t { HOME=0, DETAIL=1, RUNNING=2, RATING=3, SETTINGS=4, CALIB=5 };

enum class CalibField : uint8_t { V_SCALE=0, V_OFFSET=1, I_SCALE=2, I_OFFSET=3 };

struct SmartState {
  bool smartEnabled = true;
  float t_mm = 0.15f;            // ketebalan
  float v_guess = 2.0f;          // tebakan daya (Vrms)
  float i_guess = 300.f;         // tebakan daya (Irms)
  AITarget tgt{}; PulseProfile rec{}; WeldMetrics last{};
  float E_target = 0; float E_actual = 0; int8_t ratingSel = 0;
  // Calibration working copy
  float v_scale{1.0f}, v_offset{0.0f}, i_scale{1.0f}, i_offset{0.0f};
  CalibField calSel{CalibField::V_SCALE};
};

class SmartUI {
public:
  bool begin(Adafruit_SSD1306* d, PulseEngine* e, StorageManager* s,
             SafetyManager* safe, AIAdvisor* ai, SenseEngine* se, Logger* lg);

  void tick();
  void onRotate(int delta);
  void onClick();
  void onTrigger();
  void onLongPress();
  void onStage(PulseStage st);
  SmartState& state() { return st; }
private:
  Adafruit_SSD1306* display=nullptr; PulseEngine* engine=nullptr; StorageManager* storage=nullptr; SafetyManager* safety=nullptr; AIAdvisor* advisor=nullptr; SenseEngine* sense=nullptr; Logger* logger=nullptr;
  SmartView view=SmartView::HOME; SmartState st{}; uint32_t lastUi=0;
  AppConfig cfg{}; // loaded once

  void drawHeader(const __FlashStringHelper* title);
  void drawFooter(const String& hint);
  void drawEnergyBar(int16_t x,int16_t y,int16_t w,int16_t h,float frac);
  void drawIconsStatus();
  void drawHome(); void drawDetail(); void drawRunning(); void drawRating(); void drawSettings(); void drawCalib();
  void refreshRecommendation();
  uint8_t stageToId(PulseStage st);
};


#pragma once
#include <Preferences.h>
#include "PulseEngine.h"
#include "Config.h"

struct AppConfig {
  uint8_t version{MODEL_VERSION};
  uint8_t presetIndex{0};
  PulseProfile presets[10]{};
  bool doublePulseEnabled{true};
  bool triplePulseEnabled{true};
  uint16_t cooldown_ms{COOLDOWN_MS};
  uint16_t lockout_ms{LOCKOUT_MS};
  // Calibration (runtime; saved to NVS)
  float v_scale{1.0f};
  float v_offset{0.0f};
  float i_scale{1.0f};
  float i_offset{0.0f};
};

class StorageManager {
public:
  void begin();
  void load(AppConfig& cfg);
  void save(const AppConfig& cfg);
private:
  Preferences prefs;
  void loadDefaults(AppConfig& cfg);
};

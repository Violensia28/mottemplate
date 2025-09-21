
#pragma once
#include <Arduino.h>
#include "Config.h"

class SafetyManager {
public:
  void begin(uint8_t pinInterlock, uint16_t lockoutMs=LOCKOUT_MS, uint16_t cooldownMs=COOLDOWN_MS) {
    _pinInterlock = pinInterlock; pinMode(_pinInterlock, INPUT_PULLUP);
    _lockoutMs = lockoutMs; _cooldownMs = cooldownMs;
  }
  bool interlockOK() const { return digitalRead(_pinInterlock)==LOW; } // contoh aktif LOW
  bool canStart() const {
    uint32_t now = millis();
    if (!interlockOK()) return false;
    if (now - _lastEndMs < (_cooldownActive ? _cooldownMs : _lockoutMs)) return false;
    return true;
  }
  void onStart() { _cooldownActive = true; }
  void onEnd()   { _lastEndMs = millis(); _cooldownActive = true; }
  void onAbort() { _lastEndMs = millis(); _cooldownActive = true; }
private:
  uint8_t _pinInterlock{255};
  uint16_t _lockoutMs{LOCKOUT_MS}, _cooldownMs{COOLDOWN_MS};
  mutable bool _cooldownActive{false};
  mutable uint32_t _lastEndMs{0};
};


#pragma once
#include <Arduino.h>
#include "config.h"
#if ENABLE_LOGGING
#include <SPI.h>
#include <SD.h>
#endif
#include "ai/AIAdvisor.h"
#include "PulseEngine.h"

class Logger {
public:
  void begin();
  void logRecord(float t_mm, const AITarget& tgt, const PulseProfile& rec,
                 const WeldMetrics& wm, int8_t rating);
  bool sdOK() const { return _sdok; }
private:
#if ENABLE_LOGGING
  bool _sdok=false;
  void ensureHeader();
#else
  bool _sdok=false;
#endif
};

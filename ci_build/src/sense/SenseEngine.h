
#pragma once
#include <Arduino.h>
#include "ai/AIAdvisor.h"
#include "Config.h"

class SenseEngine {
public:
  void begin(int pinV, int pinI, uint32_t fs_hz=10000);
  void armForPulse();
  void onStageStart(uint8_t stageId);
  void onStageEnd(uint8_t stageId);
  void finalize(WeldMetrics& out);
  void tick(); // call in loop for sampling during ON stages
  void simulate(float Vrms, float Irms) { _Vr=Vrms; _Ir=Irms; _sim=true; }
  // status helpers
  bool vPinValid() const { return _pinV>=0; }
  bool iPinValid() const { return _pinI>=0; }
private:
  int _pinV=-1, _pinI=-1; uint32_t _fs=10000; uint32_t _dt_us=100;
  double _sumV2=0, _sumI2=0, _sumVI=0; uint32_t _n=0; uint32_t _t0=0; bool _on=false; uint8_t _sid=0; uint32_t _lastSample=0;
  WeldMetrics _wm{}; float _Vr=2.0f, _Ir=300.0f; bool _sim=false;
  void _resetAcc(); void _commitStage();
  float adcToVolt(int raw) { return (raw/ADC_MAX) * ADC_REF; }
};

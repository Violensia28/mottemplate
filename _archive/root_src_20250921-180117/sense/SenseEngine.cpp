
#include "sense/SenseEngine.h"
#include <math.h>

void SenseEngine::begin(int pinV, int pinI, uint32_t fs_hz){ _pinV=pinV; _pinI=pinI; _fs=fs_hz; _dt_us=(fs_hz>0? (uint32_t)(1000000UL/fs_hz):100); if (_pinV>=0) pinMode(_pinV, INPUT); if (_pinI>=0) pinMode(_pinI, INPUT); }
void SenseEngine::armForPulse(){ _wm = WeldMetrics{}; }
void SenseEngine::onStageStart(uint8_t stageId){ _sid=stageId; _on=true; _t0=millis(); _resetAcc(); _lastSample=micros(); }
void SenseEngine::onStageEnd(uint8_t stageId){ if (_sid==stageId) { _commitStage(); _on=false; } }
void SenseEngine::finalize(WeldMetrics& out){ out=_wm; }
void SenseEngine::_resetAcc(){ _sumV2=0; _sumI2=0; _sumVI=0; _n=0; }
void SenseEngine::_commitStage(){ float Vrms_adc=(_n>0? sqrt(_sumV2/_n):0.0); float Irms_adc=(_n>0? sqrt(_sumI2/_n):0.0); float Vrms=Vrms_adc; float Irms=Irms_adc; uint32_t dt_ms = millis()-_t0; float P=Vrms*Irms; float E=P*(dt_ms/1000.0f); if (_sid==1){ _wm.Vrms_pre=Vrms; _wm.Irms_pre=Irms; _wm.E_pre=E; _wm.R_pre=(Irms>0.01f? Vrms/Irms:0); } if (_sid==3){ _wm.Vrms_main=Vrms; _wm.Irms_main=Irms; _wm.E_main=E; _wm.R_main=(Irms>0.01f? Vrms/Irms:0); } if (_sid==5){ _wm.Vrms_temper=Vrms; _wm.Irms_temper=Irms; _wm.E_temper=E; } }
void SenseEngine::tick(){ if (!_on) return; uint32_t now=micros(); if (now-_lastSample<_dt_us) return; _lastSample=now; if (_sim){ float v=_Vr, i=_Ir; _sumV2+=v*v; _sumI2+=i*i; _sumVI+=v*i; _n++; return; } int rv=(_pinV>=0? analogRead(_pinV):0); int ri=(_pinI>=0? analogRead(_pinI):0); float v=adcToVolt(rv); float i=adcToVolt(ri); _sumV2+=v*v; _sumI2+=i*i; _sumVI+=v*i; _n++; }

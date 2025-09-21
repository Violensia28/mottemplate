
#include "PulseEngine.h"
#include "config.h"

void PulseEngine::begin(uint8_t pinActuator, StageCb cb) {
  pin = pinActuator; pinMode(pin, OUTPUT); setOutput(false); onStage = cb;
}

void PulseEngine::start(const PulseProfile& p) {
  if (running) return; prof = p; stage = PulseStage::NONE; running = true; advance(); }

void PulseEngine::cancel() { if (!running) return; stage = PulseStage::ABORT; setOutput(false); running = false; if (onStage) onStage(stage); }

void PulseEngine::loop() { if (!running) return; if ((int32_t)(millis() - stageEndMs) >= 0) advance(); }

void PulseEngine::setOutput(bool on) { outState = on; digitalWrite(pin, on ? HIGH : LOW); }

static inline bool _nz(uint16_t v){ return v>0; }

void PulseEngine::advance(){
  switch(stage){
    case PulseStage::NONE:
      if (_nz(prof.pre_ms)) { stage = PulseStage::PRE; setOutput(true); stageEndMs = millis() + prof.pre_ms; }
      else { stage = PulseStage::MAIN; setOutput(true); stageEndMs = millis() + prof.main_ms; }
      break;
    case PulseStage::PRE:
      setOutput(false);
      if (_nz(prof.gap1_ms)) { stage = PulseStage::GAP1; stageEndMs = millis() + prof.gap1_ms; }
      else { stage = PulseStage::MAIN; setOutput(true); stageEndMs = millis() + prof.main_ms; }
      break;
    case PulseStage::GAP1:
      stage = PulseStage::MAIN; setOutput(true); stageEndMs = millis() + prof.main_ms; break;
    case PulseStage::MAIN:
      setOutput(false);
      if (_nz(prof.temper_ms)) {
        if (_nz(prof.gap2_ms)) { stage = PulseStage::GAP2; stageEndMs = millis() + prof.gap2_ms; }
        else { stage = PulseStage::TEMPER; setOutput(true); stageEndMs = millis() + prof.temper_ms; }
      } else { stage = PulseStage::DONE; running = false; }
      break;
    case PulseStage::GAP2:
      stage = PulseStage::TEMPER; setOutput(true); stageEndMs = millis() + prof.temper_ms; break;
    case PulseStage::TEMPER:
      setOutput(false); stage = PulseStage::DONE; running = false; break;
    default: running = false; break;
  }
  if (onStage) onStage(stage);
}

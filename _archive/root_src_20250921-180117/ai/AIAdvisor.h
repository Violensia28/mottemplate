
#pragma once
#include <Arduino.h>
#include "PulseEngine.h"

struct WeldMetrics {
  float Vrms_pre=0, Irms_pre=0, E_pre=0;
  float Vrms_main=0, Irms_main=0, E_main=0;
  float Vrms_temper=0, Irms_temper=0, E_temper=0;
  float R_pre=0, R_main=0;
  float E_total() const { return E_pre + E_main + E_temper; }
};

struct AITarget {
  float E_target;         // Joule
  bool useTriple=false;
  bool useDouble=true;
  float preFrac=0.2f;     // fraksi energi untuk preheat
  float temperFrac=0.1f;  // fraksi energi untuk temper
};

struct AIModel {
  float theta[6];    // [1, t, t^2, Irms, Rpre, Rmain]
  float P[36];       // kovarians RLS (6x6)
  uint8_t version{1};
};

class AIAdvisor {
public:
  void begin();
  AITarget estimateTarget(float t_mm, const WeldMetrics* last);
  PulseProfile recommend(const AITarget& tgt, const WeldMetrics* last,
                         float v_rms_guess, float i_rms_guess);
  void updateRLS(float t_mm, const WeldMetrics& wm, int8_t rating);
  float getEtargetLast() const { return _lastTarget; }
private:
  AIModel M{}; float _lastTarget{0};
  void initDefaults(); void rlsUpdate(const float* x, float y);
};

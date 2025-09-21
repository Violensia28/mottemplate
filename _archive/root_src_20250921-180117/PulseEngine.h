
#pragma once
#include <Arduino.h>
#include <functional>

enum class PulseStage : uint8_t { NONE, PRE, GAP1, MAIN, GAP2, TEMPER, DONE, ABORT };

struct PulseProfile {
  uint16_t pre_ms{0};
  uint16_t gap1_ms{0};
  uint16_t main_ms{0};
  uint16_t gap2_ms{0};
  uint16_t temper_ms{0};
};

class PulseEngine {
public:
  using StageCb = std::function<void(PulseStage)>;
  void begin(uint8_t pinActuator, StageCb cb);
  bool busy() const { return running; }
  void start(const PulseProfile& p);
  void cancel();
  void loop();
  PulseStage currentStage() const { return stage; }
private:
  void setOutput(bool on);
  void advance();
  uint32_t stageEndMs{0};
  PulseProfile prof{};
  PulseStage stage{PulseStage::NONE};
  bool running{false};
  uint8_t pin{255};
  StageCb onStage{nullptr};
  bool outState{false};
};


#include "logging/Logger.h"

void Logger::begin(){
#if ENABLE_LOGGING
  if (SD.begin(PIN_SD_CS)) { _sdok=true; ensureHeader(); } else { _sdok=false; }
#else
  _sdok=false;
#endif
}

void Logger::logRecord(float t_mm, const AITarget& tgt, const PulseProfile& rec,
                       const WeldMetrics& wm, int8_t rating){
#if ENABLE_LOGGING
  if (!_sdok) return; File f = SD.open(LOG_FILE, FILE_APPEND); if (!f) return;
  f.printf("%lu,%.2f,%.1f,%u,%u,%u,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%d\n",
           millis(), t_mm, tgt.E_target,
           rec.pre_ms, rec.main_ms, rec.temper_ms,
           wm.Vrms_main, wm.Irms_main, wm.E_main,
           wm.Vrms_pre, wm.Irms_pre, wm.E_pre,
           (int)rating);
  f.close();
#endif
}

void Logger::ensureHeader(){
#if ENABLE_LOGGING
  if (SD.exists(LOG_FILE)) return; File f=SD.open(LOG_FILE, FILE_WRITE); if (!f) return;
  f.println("ts_ms,t_mm,E_target,pre_ms,main_ms,temper_ms,Vrms_main,Irms_main,E_main,Vrms_pre,Irms_pre,E_pre,rating"); f.close();
#endif
}

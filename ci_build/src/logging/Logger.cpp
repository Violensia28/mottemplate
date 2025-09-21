#include "Logger.h"
#ifdef USE_LOGGING
void Logger::begin(){ Serial.println("[LOG] start"); }
void Logger::ensureHeader(){
  if (!headerPrinted){ Serial.println("[LOG] ------------------------------"); headerPrinted = true; }
}
void Logger::info(const String& s){ ensureHeader(); Serial.println("[I] " + s); }
void Logger::warn(const String& s){ ensureHeader(); Serial.println("[W] " + s); }
void Logger::err (const String& s){ ensureHeader(); Serial.println("[E] " + s); }
#endif

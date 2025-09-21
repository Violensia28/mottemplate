#pragma once
#ifdef USE_LOGGING
#include <Arduino.h>
class Logger {
public:
  void begin();
  void ensureHeader();
  void info(const String& s);
  void warn(const String& s);
  void err (const String& s);
private:
  bool headerPrinted = false;
};
#endif

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

  // === API yang diminta SmartUI ===
  // Rekam satu baris data (tipe dibuat long/int agar fleksibel dengan struktur status kamu)
  void logRecord(long t_mm, long tgt, long rec, long last, long ratingSel);
  // Status SD (untuk ikon SD di UI)
  bool sdOK() const;

private:
  bool headerPrinted = false;
  bool sd_ok = false;     // default: tidak pakai SD (false)
};
#endif

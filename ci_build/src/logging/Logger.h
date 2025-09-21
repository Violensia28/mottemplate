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

  // Overload lama (tetap dipertahankan untuk pemanggilan numerik)
  void logRecord(long t_mm, long tgt, long rec, long last, long ratingSel);

  // Overload templated (menerima tipe apapun utk argumen 2..4)
  // Ini menyelesaikan pemanggilan dari SmartUI: (float, AITarget, PulseProfile, WeldMetrics, int8_t)
  template<typename T1, typename T2, typename T3>
  void logRecord(float t_mm, const T1&, const T2&, const T3&, int8_t ratingSel) {
    ensureHeader();
    Serial.printf("[REC] t=%.3f rating=%d\n", (double)t_mm, (int)ratingSel);
#ifdef USE_SDLOG
    if (sd_ok) {
      File f = SD.open("/motlog.csv", FILE_APPEND);
      if (f) {
        // CSV minimal (tanpa membongkar objek kompleks)
        f.printf("%.3f,,,,%d\n", (double)t_mm, (int)ratingSel);
        f.close();
      } else {
        Serial.println("[LOG] SD open fail (/motlog.csv)");
      }
    }
#endif
  }

  // Ikon status SD di SmartUI
  bool sdOK() const;

private:
  bool headerPrinted = false;
  bool sd_ok = false;
};
#endif

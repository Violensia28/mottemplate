#include "Logger.h"
#ifdef USE_LOGGING

// (Opsional) aktifkan logging ke SD dengan -D USE_SDLOG dan tentukan CS pin via -D LOG_SD_CS=5
#ifdef USE_SDLOG
  #include <FS.h>
  #include <SD.h>
  #ifndef LOG_SD_CS
  #define LOG_SD_CS 5
  #endif
#endif

void Logger::begin() {
#ifdef USE_SDLOG
  sd_ok = SD.begin(LOG_SD_CS);
#else
  sd_ok = false; // default: tidak menggunakan SD
#endif
  Serial.println("[LOG] start");
}

void Logger::ensureHeader() {
  if (!headerPrinted) {
    Serial.println("[LOG] ------------------------------");
    headerPrinted = true;
  }
}

void Logger::info(const String& s) {
  ensureHeader();
  Serial.println("[I] " + s);
}

void Logger::warn(const String& s) {
  ensureHeader();
  Serial.println("[W] " + s);
}

void Logger::err(const String& s) {
  ensureHeader();
  Serial.println("[E] " + s);
}

void Logger::logRecord(long t_mm, long tgt, long rec, long last, long ratingSel) {
  ensureHeader();
  // Tulis ke Serial (aman di semua board)
  Serial.printf("[REC] t=%ld tgt=%ld rec=%ld last=%ld rating=%ld\n", t_mm, tgt, rec, last, ratingSel);

#ifdef USE_SDLOG
  if (sd_ok) {
    File f = SD.open("/motlog.csv", FILE_APPEND);
    if (f) {
      // Format CSV sederhana
      f.printf("%ld,%ld,%ld,%ld,%ld\n", t_mm, tgt, rec, last, ratingSel);
      f.close();
    } else {
      Serial.println("[LOG] SD open fail (/motlog.csv)");
    }
  }
#endif
}

bool Logger::sdOK() const {
  return sd_ok;
}

#endif

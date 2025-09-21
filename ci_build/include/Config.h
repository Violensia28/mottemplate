
#pragma once

// ===== Build Flags (overridden by platformio.ini) =====
#ifndef SAFE_SIM
#define SAFE_SIM            1
#endif
#ifndef BUILD_SMART_MODE
#define BUILD_SMART_MODE    1
#endif
#ifndef ENABLE_LOGGING
#define ENABLE_LOGGING      0
#endif
#ifndef USE_ZC_SYNC
#define USE_ZC_SYNC         0
#endif

// ===== Pin Mapping (ESP32 DevKit V1) =====
#define PIN_OLED_SDA        21
#define PIN_OLED_SCL        22

#define PIN_ENC_A           32
#define PIN_ENC_B           33
#define PIN_ENC_BTN         27
#define PIN_TRIGGER         14
#define PIN_INTERLOCK       13

#define PIN_BUZZER          15
#define PIN_ACTUATOR        2   // SAFE_SIM: LED onboard
#define PIN_SSR_IN          25  // LV side signal to SSR input

// Sensors (FULL)
#define PIN_ADC_V           34  // ZMPT101B analog out
#define PIN_ADC_I           35  // ACS758 analog out

// MicroSD (SPI)
#define PIN_SD_CS           5
#define PIN_SD_MOSI         23
#define PIN_SD_MISO         19
#define PIN_SD_SCK          18

// ===== UI Timing =====
#define UI_FPS              30
#define LONG_PRESS_MS       1200
#define DEBOUNCE_MS         20

// ===== ADC & Calibration (defaults; will be overridden by NVS values) =====
#define ADC_MAX             4095.0f
#define ADC_REF             3.30f

// ===== Safety Timing =====
#define LOCKOUT_MS          500
#define COOLDOWN_MS         400

// ===== Model Defaults =====
#define MODEL_VERSION       2   // bump: add calibration params in NVS

// ===== Logging =====
#define LOG_FILE            "/weld_log.csv"


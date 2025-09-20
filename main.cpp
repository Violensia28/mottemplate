#include <Arduino.h>

// --- Optional UI libs (compile-time guarded) ---
#ifdef USE_OLED
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
Adafruit_SSD1306 display(128, 64, &Wire, -1);
#endif

void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println("MOT Smart Firmware (Template) booting...");

#ifdef USE_OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 not found");
  } else {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0,0);
    display.println("MOT Smart Firmware");
    display.println("Template build");
    display.display();
  }
#endif
}

void loop() {
  static uint32_t last = 0;
  if (millis() - last > 1000) {
    last = millis();
    Serial.println("heartbeat");
  }
}

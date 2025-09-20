#include <Arduino.h>
void setup(){ Serial.begin(115200); Serial.println("Firmware OK"); }
void loop(){ delay(1000); Serial.println("heartbeat"); }

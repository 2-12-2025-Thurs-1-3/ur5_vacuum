#include <WiFi.h>
 
// https://randomnerdtutorials.com/esp-now-esp32-arduino-ide/
// USED FOR GETTING VACUUM MAC ADDRESS (In case ESP32 gets switched.)


void setup(){
  Serial.begin();
  WiFi.mode(WIFI_MODE_STA);
}
 
void loop(){  
  Serial.println(WiFi.macAddress());
  delay(250);
}
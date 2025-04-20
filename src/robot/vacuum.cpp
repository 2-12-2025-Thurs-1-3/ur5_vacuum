#include <esp_now.h>
#include <WiFi.h>
#include "pinout.h"

/*
VACUUM:

This code receives signals to turn the vacuum pump on and off from the joystick using ESP NOW.
Check "pinout.h" and color wire functions (taped next to UR5) for wiring.

"timeout" is the time the solenoid can remain on for, flushing the line.
"cooldown" is the time before the solenoid can be turned on again.

WARNINGS:
    - BLUE WIRE MUST NOT BE TURNED ON FOR MORE THAN 20 SECONDS AT A TIME!!!!!
      The code was written with this in mind; the "on" signal gets processed further,
      allowing for the quick flushing of line using the solenoid, to enable throwing of bottles.
    - You may change the cooldown and timeout times at your own discretion.
    - Make sure that messages are received correctly from joystick before using.
*/


// Structure to receive data (must match sender)
typedef struct struct_message {
    bool vacuum_on;
} struct_message;

struct_message myData;

// Timing variables
unsigned long off_time = 0;
unsigned long cur_t = 0;
unsigned long cd_t = 0;
unsigned long to_t = 0;

// State flags
bool was_on = false;
bool was_solenoid = false;
bool timeout = false;
bool cooldown = false;

// Time thresholds in milliseconds
unsigned long cooldown_time = 10000;       // Cooldown duration
unsigned long timeout_time = 20000;  // Solenoid active time limit

void pinLogic(bool on) {
  cur_t = millis();

  // Handle cooldown expiration
  if (cooldown && (cur_t - cd_t) > cooldown_time) {
    cooldown = false;
    Serial.println("Cooldown over");
  }

  // Handle solenoid timeout
  if (was_solenoid && (cur_t - to_t) > timeout_time) {
    digitalWrite(BLUE, LOW);  // Turn off solenoid
    was_solenoid = false;
    cooldown = true;
    cd_t = cur_t;
    Serial.println("Solenoid auto-off (timeout), cooldown started");
  }

  // If vacuum is ON
  if (on) {
    digitalWrite(WHITE, HIGH);

    if (was_solenoid) {
      digitalWrite(BLUE, LOW);  // Turn off solenoid
      was_solenoid = false;
      cooldown = true;
      cd_t = cur_t;
      Serial.println("Solenoid interrupted by vacuum, cooldown started");
    }

    was_on = true;
    Serial.println("VACUUM ON");
    return;
  }

  // If vacuum is OFF
  digitalWrite(WHITE, LOW);

  // Try to turn ON solenoid only if:
  // - Not in cooldown
  // - Solenoid is currently OFF
  // - Vacuum was previously ON
  if (!cooldown && !was_solenoid && was_on) {
    digitalWrite(BLUE, HIGH);  // Turn on solenoid
    was_solenoid = true;
    to_t = cur_t;
    was_on = false;            // Reset the trigger
    Serial.println("SOLENOID ON");
  }
}



// Callback when data is received via ESP-NOW
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("Vacuum On: ");
  Serial.println(myData.vacuum_on ? "true" : "false");

  pinLogic(myData.vacuum_on);
}

void setup() {
  Serial.begin(115200);

  // Set up pin modes (optional - ensure WHITE and BLUE are defined)
  pinMode(WHITE, OUTPUT);
  pinMode(BLUE, OUTPUT);
  pinMode(TEST, INPUT_PULLUP); 

  // Start WiFi in station mode
  WiFi.mode(WIFI_STA);

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register ESP-NOW receive callback
  esp_now_register_recv_cb(OnDataRecv);

  // Initialize time tracking
  unsigned long now = millis();
  off_time = now;
  cur_t = now;
  cd_t = now;
}

void loop() {
  // // Nothing here; everything happens on ESP-NOW receive
  // bool on = !digitalRead(TEST);
  // //Serial.println(on);
  // pinLogic(on);
}

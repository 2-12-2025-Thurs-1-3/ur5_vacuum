#include <Wire.h>
#include <Adafruit_VL6180X.h>

#define SHDN1 5 // GPIO pin for sensor 1 SHDN
#define SHDN2 6  // GPIO pin for sensor 2 SHDN

Adafruit_VL6180X tof1 = Adafruit_VL6180X();  // Will become 0x30
Adafruit_VL6180X tof2 = Adafruit_VL6180X();  // Remains at 0x29

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Setup shutdown pins as outputs
  pinMode(SHDN1, OUTPUT);
  pinMode(SHDN2, OUTPUT);

  // Shut down both sensors initially
  digitalWrite(SHDN1, LOW);
  digitalWrite(SHDN2, LOW);
  delay(10);

  // Power up sensor 1 only and assign new address
  digitalWrite(SHDN1, HIGH);
  delay(10);
  if (!tof1.begin()) {
    Serial.println("Failed to initialize VL6180X sensor 1");
    while (1);
  }
  tof1.setAddress(0x30);  // Assign new address to sensor 1
  Serial.println("VL6180X #1 initialized at address 0x30");

  // Power up sensor 2 (still at 0x29)
  digitalWrite(SHDN2, HIGH);
  delay(10);
  if (!tof2.begin()) {
    Serial.println("Failed to initialize VL6180X sensor 2");
    while (1);
  }
  Serial.println("VL6180X #2 initialized at address 0x29");
}


void loop() {
  // Read distance from VL6180X #1
  uint8_t range1 = tof1.readRange();
  uint8_t status1 = tof1.readRangeStatus();

  // Read distance from VL6180X #2
  uint8_t range2 = tof2.readRange();
  uint8_t status2 = tof2.readRangeStatus();

  // Print distance from sensor 1
  Serial.print("[VL6180X 0x30] Distance: ");
  if (status1 == VL6180X_ERROR_NONE) {
    Serial.print(range1);
  } else {
    Serial.print("Error ");
    Serial.print(status1);
  }
  Serial.print(" mm");

  // Print distance from sensor 2
  Serial.print("\t[VL6180X 0x29] Distance: ");
  if (status2 == VL6180X_ERROR_NONE) {
    Serial.print(range2);
  } else {
    Serial.print("Error ");
    Serial.print(status2);
  }
  Serial.println(" mm");

  delay(100);  // Small delay for reading
}

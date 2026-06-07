#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

int gasPin = 34;

void setup() {

  Serial.begin(115200);
  SerialBT.begin("ESP32_sender");
}

void loop() {

  int gas = analogRead(gasPin);

  SerialBT.println(gas);

  delay(2000);
}

/*
  Reward LED for task.html (Arduino Uno R4 WiFi).
  '1' turns the LED on, '0' turns it off. Commands arrive either:
    - over Bluetooth LE (Android tablet, Chrome Web Bluetooth), or
    - over the USB cable at 9600 baud (desktop Chrome Web Serial, arduino_test.html).

  Needs the ArduinoBLE library: Arduino IDE > Tools > Manage Libraries > search "ArduinoBLE".
  The UUIDs below must match BLE_SERVICE / BLE_LED in task.html.
*/
#include <ArduinoBLE.h>

const int LED_PIN = LED_BUILTIN;   // change to the pin your LED is wired to

BLEService ledService("19b10000-e8f2-537e-4f6c-d104768a1214");
BLEByteCharacteristic ledCommand("19b10001-e8f2-537e-4f6c-d104768a1214", BLEWrite | BLEWriteWithoutResponse);

bool bleOk = false;

void setLed(char c) {
  if (c == '1') digitalWrite(LED_PIN, HIGH);
  if (c == '0') digitalWrite(LED_PIN, LOW);
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  Serial.begin(9600);              // must match ARDUINO_BAUD in task.html

  bleOk = BLE.begin();
  if (!bleOk) {
    // Bluetooth didn't start (often fixed by updating the board's WiFi/BLE firmware).
    // USB serial still works.
    Serial.println("BLE failed to start");
    return;
  }
  BLE.setLocalName("CageLED");
  BLE.setAdvertisedService(ledService);   // lets the browser's picker find this board
  ledService.addCharacteristic(ledCommand);
  BLE.addService(ledService);
  BLE.advertise();
  Serial.println("BLE advertising as CageLED");
}

void loop() {
  if (bleOk) {
    BLE.poll();
    if (ledCommand.written()) setLed((char)ledCommand.value());
  }
  while (Serial.available()) setLed((char)Serial.read());
}

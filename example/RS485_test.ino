#include <Arduino.h>
#include <Modbus.h>

Modbus modbus;

void setup() {
  Serial.begin(115200);
  modbus.begin(9600);
}

void loop() {
  modbus.writeRegister(1, 99, 1278);
  delay(2000);
  modbus.writeRegister(1, 99, 1151);
  delay(2000);
  modbus.writeRegister(1, 99, 1150);
  delay(2000);

  modbus.sendRequest(1, 0, 10);

  for (int i = 0; i < 10; i++) {
    uint16_t registerValue = modbus.getRegisterValue(i);
    Serial.print("Register ");
    Serial.print(i);
    Serial.print(": ");
    Serial.println(registerValue);
  }

  delay(10000);
}

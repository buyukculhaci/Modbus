#ifndef Modbus_h
#define Modbus_h

#include <Arduino.h>
#include <HardwareSerial.h>

//Default ESP32 pins, change it as your development card's Rx2 Tx2 pins
#define DEFAULT_TX_PIN 17
#define DEFAULT_RX_PIN 16
#define DEFAULT_DE_PIN 5
#define DEFAULT_RE_PIN 5

class Modbus {
  public:
    Modbus();
    void begin(long baudRate);
    void begin(long baudRate, int txPin, int rxPin, int dePin, int rePin);
    void sendRequest(uint8_t slaveAddress, uint16_t registerAddress, uint16_t numberOfRegisters);
    bool available();
    void processResponse();
    uint16_t getRegisterValue(int index);
    void writeRegister(uint8_t slaveAddress, uint16_t registerAddress, int value);

  private:
    HardwareSerial _serial;
    int _dePin;
    int _rePin;
    uint8_t _responseBuffer[256];
    int _responseLength;
    uint16_t _registers[128];

    uint16_t calculateCRC(uint8_t *data, uint16_t length);
};

#endif

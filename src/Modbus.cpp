#include "Modbus.h"

Modbus::Modbus()
    : _serial(2), _dePin(DEFAULT_DE_PIN), _rePin(DEFAULT_RE_PIN), _responseLength(0) {}

void Modbus::begin(long baudRate) {
  begin(baudRate, DEFAULT_TX_PIN, DEFAULT_RX_PIN, DEFAULT_DE_PIN, DEFAULT_RE_PIN);
}

void Modbus::begin(long baudRate, int txPin, int rxPin, int dePin, int rePin) {
  _dePin = dePin;
  _rePin = rePin;
  pinMode(_dePin, OUTPUT);
  pinMode(_rePin, OUTPUT);
  digitalWrite(_dePin, LOW);
  digitalWrite(_rePin, LOW);
  _serial.begin(baudRate, SERIAL_8N1, rxPin, txPin);
}

void Modbus::sendRequest(uint8_t slaveAddress, uint16_t registerAddress, uint16_t numberOfRegisters) {
  uint8_t request[8];

  request[0] = slaveAddress;
  request[1] = 0x03;
  request[2] = (registerAddress >> 8) & 0xFF;
  request[3] = registerAddress & 0xFF;
  request[4] = (numberOfRegisters >> 8) & 0xFF;
  request[5] = numberOfRegisters & 0xFF;

  uint16_t crc = calculateCRC(request, 6);
  request[6] = crc & 0xFF;
  request[7] = (crc >> 8) & 0xFF;

  digitalWrite(_dePin, HIGH);
  digitalWrite(_rePin, HIGH);
  delay(1);
  _serial.write(request, 8);
  _serial.flush();
  digitalWrite(_dePin, LOW);
  digitalWrite(_rePin, LOW);
  delay(100);

  if (available()) {
    processResponse();
  }
}

bool Modbus::available() {
  return _serial.available() >= 7;
}

void Modbus::processResponse() {
  _responseLength = _serial.readBytes(_responseBuffer, _serial.available());

  if (_responseLength >= 5) {
    uint8_t byteCount = _responseBuffer[2];
    if (_responseLength == (5 + byteCount)) {
      for (int i = 0; i < byteCount / 2; i++) {
        _registers[i] = (_responseBuffer[3 + i * 2] << 8) | _responseBuffer[4 + i * 2];
      }
    }
  }
}

uint16_t Modbus::getRegisterValue(int index) {
  if (index >= 0 && index < 128) {
    return _registers[index];
  }
  return 0;
}

void Modbus::writeRegister(uint8_t slaveAddress, uint16_t registerAddress, int value) {
  uint8_t request[8];

  request[0] = slaveAddress;
  request[1] = 0x06;
  request[2] = (registerAddress >> 8) & 0xFF;
  request[3] = registerAddress & 0xFF;
  request[4] = (value >> 8) & 0xFF;
  request[5] = value & 0xFF;

  uint16_t crc = calculateCRC(request, 6);
  request[6] = crc & 0xFF;
  request[7] = (crc >> 8) & 0xFF;

  digitalWrite(_dePin, HIGH);
  digitalWrite(_rePin, HIGH);
  delay(1);
  _serial.write(request, 8);
  _serial.flush();
  digitalWrite(_dePin, LOW);
  digitalWrite(_rePin, LOW);
  delay(100);

  if (available()) {
    processResponse();
  }
}

uint16_t Modbus::calculateCRC(uint8_t *data, uint16_t length) {
  uint16_t crc = 0xFFFF;
  for (uint16_t pos = 0; pos < length; pos++) {
    crc ^= (uint16_t)data[pos];
    for (int i = 8; i != 0; i--) {
      if ((crc & 0x0001) != 0) {
        crc >>= 1;
        crc ^= 0xA001;
      } else {
        crc >>= 1;
      }
    }
  }
  return crc;
}

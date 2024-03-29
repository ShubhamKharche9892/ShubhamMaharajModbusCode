/* ModbusMessage

Copyright 2018 Bert Melis

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


CRC16 calculation

copyright 2006 Modbus.org
MODBUS over serial line specification and implementation guide V1.02

*/

#include "ModbusMessage.h"

using namespace esp32ModbusRTUInternals;  // NOLINT

static uint8_t crcHiTable[] = {
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
  0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
  0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
  0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
  0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
  0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
  0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
  0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
  0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
  0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
  0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
  0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
  0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
  0x40
};

static uint8_t crcLoTable[] = {
  0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
  0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
  0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
  0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
  0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
  0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
  0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
  0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
  0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
  0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
  0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
  0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
  0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
  0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
  0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
  0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
  0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
  0x40
};

uint16_t CRC16(uint8_t* msg, size_t len) {
  uint8_t crcHi = 0xFF;
  uint8_t crcLo = 0xFF;
  uint8_t index;

  while (len--) {
    index = crcLo ^ *msg++;
    crcLo = crcHi ^ crcHiTable[index];
    crcHi = crcLoTable[index];
  }
  return (crcHi << 8 | crcLo);
}

uint8_t low(uint16_t in) {
  return (in & 0xff);
}

uint8_t high(uint16_t in) {
  return ((in >> 8) & 0xff);
}

uint16_t make_word(uint8_t high, uint8_t low) {
  return ((high << 8) | low);
}

ModbusMessage::ModbusMessage(uint8_t length) :
  _buffer(nullptr),
  _length(length),
  _index(0) {
  if (length < 5) _length = 5;  // minimum for Modbus Exception codes
  _buffer = new uint8_t[_length];
  for (uint8_t i = 0; i < _length; ++i) {
    _buffer[i] = 0;
  }
}

ModbusMessage::~ModbusMessage() {
  delete[] _buffer;
}

uint8_t* ModbusMessage::getMessage() {
  return _buffer;
}

uint8_t ModbusMessage::getSize() {
  return _index;
}

void ModbusMessage::add(uint8_t value) {
  if (_index < _length) _buffer[_index++] = value;
}

ModbusRequest::ModbusRequest(uint8_t length) :
  ModbusMessage(length),
  _slaveAddress(0),
  _functionCode(0),
  _address(0),
  _byteCount(0) {}

  uint16_t ModbusRequest::getAddress() {
  return _address;
}

ModbusRequest02::ModbusRequest02(uint8_t slaveAddress, uint16_t address, uint16_t numberCoils) :
  ModbusRequest(8) {
  _slaveAddress = slaveAddress;
  _functionCode = esp32Modbus::READ_DISCR_INPUT;
  _address = address;
  _byteCount = numberCoils / 8 + 1;
  add(_slaveAddress);
  add(_functionCode);
  add(high(_address));
  add(low(_address));
  add(high(numberCoils));
  add(low(numberCoils));
  uint16_t CRC = CRC16(_buffer, 6);
  add(low(CRC));
  add(high(CRC));
}

size_t ModbusRequest02::responseLength() {
  return 5 + _byteCount;
}

ModbusRequest03::ModbusRequest03(uint8_t slaveAddress, uint16_t address, uint16_t numberRegisters) :
  ModbusRequest(8) {
  _slaveAddress = slaveAddress;
  _functionCode = esp32Modbus::READ_HOLD_REGISTER;
  _address = address;
  _byteCount = numberRegisters * 2;  // register is 2 bytes wide
  add(_slaveAddress);
  add(_functionCode);
  add(high(_address));
  add(low(_address));
  add(high(numberRegisters));
  add(low(numberRegisters));
  uint16_t CRC = CRC16(_buffer, 6);
  add(low(CRC));
  add(high(CRC));
}

size_t ModbusRequest03::responseLength() {
  return 5 + _byteCount;
}

ModbusRequest04::ModbusRequest04(uint8_t slaveAddress, uint16_t address, uint16_t numberRegisters) :
  ModbusRequest(8) {
  _slaveAddress = slaveAddress;
  _functionCode = esp32Modbus::READ_INPUT_REGISTER;
  _address = address;
  _byteCount = numberRegisters * 2;  // register is 2 bytes wide
  add(_slaveAddress);
  add(_functionCode);
  add(high(_address));
  add(low(_address));
  add(high(numberRegisters));
  add(low(numberRegisters));
  uint16_t CRC = CRC16(_buffer, 6);
  add(low(CRC));
  add(high(CRC));
}

size_t ModbusRequest04::responseLength() {
  // slaveAddress (1) + functionCode (1) + byteCount (1) + length x 2 + CRC (2)
  return 5 + _byteCount;
}

ModbusRequest06::ModbusRequest06(uint8_t slaveAddress, uint16_t address, uint16_t data) :
  ModbusRequest(8) {
  _slaveAddress = slaveAddress;
  _functionCode = esp32Modbus::WRITE_HOLD_REGISTER;
  _address = address;
  _byteCount = 2;  // 1 register is 2 bytes wide
  add(_slaveAddress);
  add(_functionCode);
  add(high(_address));
  add(low(_address));
  add(high(data));
  add(low(data));
  uint16_t CRC = CRC16(_buffer, 6);
  add(low(CRC));
  add(high(CRC));
}

size_t ModbusRequest06::responseLength() {
  return 8;
}

ModbusRequest16::ModbusRequest16(uint8_t slaveAddress, uint16_t address, uint16_t numberRegisters, uint8_t* data) :
  ModbusRequest(9 + (numberRegisters * 2)) {
  _slaveAddress = slaveAddress;
  _functionCode = esp32Modbus::WRITE_MULT_REGISTERS;
  _address = address;
  _byteCount = numberRegisters * 2;  // register is 2 bytes wide
  add(_slaveAddress);
  add(_functionCode);
  add(high(_address));
  add(low(_address));
  add(high(numberRegisters));
  add(low(numberRegisters));
  add(_byteCount);
  for (int i = 0; i < _byteCount; i++) {
      add(data[i]);
    }
  uint16_t CRC = CRC16(_buffer, 7 + _byteCount);
  add(low(CRC));
  add(high(CRC));
}

size_t ModbusRequest16::responseLength() {
  return 8;
}

ModbusResponse::ModbusResponse(uint8_t length, ModbusRequest* request) :
  ModbusMessage(length),
  _request(request),
  _error(esp32Modbus::SUCCES) {}

bool ModbusResponse::isComplete() {
  if (_buffer[1] > 0x80 && _index == 5) {  // 5: slaveAddress(1), errorCode(1), CRC(2) + indexed
    return true;
  }
  if (_index == _request->responseLength()) return true;
  return false;
}

bool ModbusResponse::isSucces() {
  if (!isComplete()) {
    _error = esp32Modbus::TIMEOUT;
  } else if (_buffer[1] > 0x80) {
    _error = static_cast<esp32Modbus::Error>(_buffer[2]);
  } else if (!checkCRC()) {
    _error = esp32Modbus::CRC_ERROR;
  // TODO(bertmelis): add other checks
  } else {
    _error = esp32Modbus::SUCCES;
  }
  if (_error == esp32Modbus::SUCCES) {
    return true;
  } else {
    return false;
  }
}

bool ModbusResponse::checkCRC() {
  uint16_t CRC = CRC16(_buffer, _length - 2);
  if (low(CRC) == _buffer[_length - 2] && high(CRC) == _buffer[_length -1]) {
    return true;
  } else {
    return false;
  }
}

esp32Modbus::Error ModbusResponse::getError() const {
  return _error;
}

uint8_t ModbusResponse::getSlaveAddress() {
  return _buffer[0];
}

esp32Modbus::FunctionCode ModbusResponse::getFunctionCode() {
  return static_cast<esp32Modbus::FunctionCode>(_buffer[1]);
}

uint8_t* ModbusResponse::getData() {
  return &_buffer[3];
}

uint8_t ModbusResponse::getByteCount() {
  return _buffer[2];
}

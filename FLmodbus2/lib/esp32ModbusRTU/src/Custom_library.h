#ifndef CUSTOM_LIBRARY_H
#define CUSTOM_LIBRARY_H

uint16_t convertFrom8To16(uint8_t dataFirst, uint8_t dataSecond);
// int add(int x, int y);

void ExecuteCommand(char commandStr[]);
void ModbusInit(void);
bool GetTempHumValue();
bool GetLowTemp();
bool GetHighTemp();
bool GetLowHumidity();
bool GetHighHumidity();
bool SetAlarm(int status);
bool SetLowTemp(uint16_t value);
bool SetHighTemp(uint16_t value);
bool SetLowHumidity(uint16_t value);
bool SetHighHumidity(uint16_t value);

#endif

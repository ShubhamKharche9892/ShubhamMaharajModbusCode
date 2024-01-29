#ifndef CUSTOM_LIBRARY_H
#define CUSTOM_LIBRARY_H
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)

#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

uint16_t convertFrom8To16(uint8_t dataFirst, uint8_t dataSecond);

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

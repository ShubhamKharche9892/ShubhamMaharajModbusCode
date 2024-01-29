#include <esp32ModbusRTU.h>
#include <Arduino.h>
#include <algorithm>
#include "Custom_library.h"

esp32ModbusRTU modbus(&Serial1, 15);
void ExecuteCommand(char commandStr[])
{
    if (strcmp(commandStr, "compare") == 0)
    {
        Serial.print("\nCommand is Compared\n");
    }

    ///// get function////////////////

    else if (strcmp(commandStr, "getval") == 0)
    {
        Serial.print("\nsending Modbus request...\n");
        if (GetTempHumValue())
        {
            Serial.println("request successfull\n");
        }
    }
    else if (strcmp(commandStr, "getlowt") == 0)
    {
        Serial.print("\nsending Modbus request...\n");
        if (GetLowTemp())
        {
            Serial.println("request successfull\n");
        }
    }
    else if (strcmp(commandStr, "gethight") == 0)
    {
        Serial.print("\nsending Modbus request...\n");
        if (GetHighTemp())
        {
            Serial.println("request successfull\n");
        }
    }
    else if (strcmp(commandStr, "getlowh") == 0)
    {
        Serial.print("\nsending Modbus request...\n");
        if (GetLowHumidity())
        {
            Serial.println("request successfull\n");
        }
    }
    else if (strcmp(commandStr, "gethighh") == 0)
    {
        Serial.print("\nsending Modbus request...\n");
        if (GetHighHumidity())
        {
            Serial.println("request successfull\n");
        }
    }
    ///////set function
    else if (strcmp(commandStr, "setlowt") == 0)
    {
        Serial.print("\nsending Modbus request...\n");
        uint16_t a = 250;
        if (SetLowTemp(a))
        {
            Serial.println("request successfull\n");
        }
    }
    else if (strcmp(commandStr, "sethight") == 0)
    {
        Serial.print("\nsending Modbus request...\n");
        uint16_t b = 300;
        if (SetHighTemp(b))
        {
            Serial.println("request successfull\n");
        }
    }
    else if (strcmp(commandStr, "setlowh") == 0)
    {
        Serial.print("\nsending Modbus request...\n");
        uint16_t c = 600;
        if (SetLowHumidity(c))
        {
            Serial.println("request successfull\n");
        }
    }
    else if (strcmp(commandStr, "sethighh") == 0)
    {
        Serial.print("\nsending Modbus request...\n");
        uint16_t d = 700;
        if (SetHighHumidity(d))
        {
            Serial.println("request successfull\n");
        }
    }
    else if (strcmp(commandStr, "setalarm") == 0)
    {
        int f;
        Serial.print("\nsending Modbus request...\n");
        Serial.print("ON/OFF: ");
        String e = Serial.readStringUntil('\n');
        if (e == "0")
        {
            f = 0;
        }
        if (e == "1")
        {
            f = 1;
        }
        delay(2000);
        if (SetAlarm(f))
        {
            Serial.println("request successfull\n");
        }
    }
    // else{
    //     Serial.print("Invalid Command\n");
    // }

}

uint16_t convertFrom8To16(uint8_t dataFirst, uint8_t dataSecond)
{
    uint16_t dataBoth = 0x0000;

    dataBoth = dataFirst;
    dataBoth = dataBoth << 8;
    dataBoth |= dataSecond;
    return dataBoth;
}

void ModbusInit(void)
{

    modbus.onData([](uint8_t serverAddress, esp32Modbus::FunctionCode fc, uint16_t address, uint8_t *data, size_t length)
                  {
        if(fc == 0x03)
    {
    Serial.printf("id: 0x%02x fc: 0x%02x len %u: 0x", serverAddress, fc, length);
    for (size_t i = 0; i < length; ++i) {
      Serial.printf("%02x", data[i]);
    }
    printf("\n");
    
        if(address == 0x00)
        {
        uint16_t temp = convertFrom8To16(data[0], data[1]);
        uint16_t humidity = convertFrom8To16(data[2], data[3]);

        float h=humidity/10.0;
        float t=temp/10.0;

        printf("Humidity: %7.1f\n", h);
        printf("Temperature: %7.1f\n", t);
        }
        else if (address == 0x001A)
        {
        uint16_t temp = convertFrom8To16(data[0], data[1]);
        float t=temp/10.0;
        printf("Temperature Lower Limit: %7.1f\n", t);

        }
        else if (address == 0x0019){
        uint16_t temp = convertFrom8To16(data[0], data[1]);
        float t=temp/10.0;
        printf("Temperature Upper Limit: %7.1f\n", t);
        }
        else if (address == 0x001C){
        uint16_t temp = convertFrom8To16(data[0], data[1]);
        float t=temp/10.0;
        printf("Humidity Lower Limit: %7.1f\n", t);
        }
        else if (address == 0x001B){
        uint16_t temp = convertFrom8To16(data[0], data[1]);
        float t=temp/10.0;
        printf("Humidity Upper Limit: %7.1f\n", t);
        }//0x001F
        else if (address == 0x001F){
        
        printf("\n");
        }
    }
if(fc==0x06){
     Serial.printf("id: 0x%02x fc: 0x%02x ", serverAddress, fc);
     Serial.printf("\nwrote value successfully....");
    
}
    std::reverse(data, data + 4);  // fix endianness
    Serial.printf("\nval: %.2f", *reinterpret_cast<float*>(data));
    Serial.print("\n\n"); });
    modbus.onError([](esp32Modbus::Error error)
                   { Serial.printf("error: 0x%02x\n\n", static_cast<uint8_t>(error)); });

    modbus.begin();
}

bool GetTempHumValue()
{
    return modbus.readHoldingRegisters(0x01, 0, 2);
}
bool GetLowTemp()
{
    return modbus.readHoldingRegisters(0x01, 0x001A, 1);
}
bool GetHighTemp()
{
    return modbus.readHoldingRegisters(0x01, 0x0019, 1);
}
bool GetLowHumidity()
{
    return modbus.readHoldingRegisters(0x01, 0x001C, 1);
}
bool GetHighHumidity()
{
    return modbus.readHoldingRegisters(0x01, 0x001B, 1);
}
//////////set
bool SetAlarm(int status)
{

    if (status == 0 || status == 1)
    {
        modbus.writeSingleHoldingRegister(0x01, 0x001F, status);
    }
    else
    {
        Serial.printf("Invalid value");
    }
    return 0;
}
bool SetLowTemp(uint16_t value)
{
    return modbus.writeSingleHoldingRegister(0x01, 0x001A, value);
}
bool SetHighTemp(uint16_t value)
{
    return modbus.writeSingleHoldingRegister(0x01, 0x0019, value);
}
bool SetLowHumidity(uint16_t value)
{
    return modbus.writeSingleHoldingRegister(0x01, 0x001C, value);
}
bool SetHighHumidity(uint16_t value)
{
    return modbus.writeSingleHoldingRegister(0x01, 0x001B, value);
}

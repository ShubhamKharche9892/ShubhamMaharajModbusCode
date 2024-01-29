/*

Copyright 2018 Bert Melis

This example reads 2 words (4 bytes) from address 52 of a server with id 1.
address 52 = register 30053 (Eastron SDM630 Total system power)
The ESP is connected to a max3485 with pins 17 (RX), 4 (TX) and 16 as RTS.

*/

#include <Arduino.h>
#include "Custom_library.h"
#include "BluetoothSerial.h"
BluetoothSerial BT;
void setup()
{
  Serial.begin(115200);                    // Serial output
  Serial1.begin(9600, SERIAL_8N1, 16, 17); // Modbus connection
  BT.begin("ESP32test");
  Serial.println("The device started, now you can pair it with bluetooth!");

  ModbusInit();
}
char commandStr[10];
int strIndex = 0;
void loop()
{
  // add(4,5);

  if (BT.available() > 0)
  {
    char ch = BT.read();

    if (ch == '\n' || strIndex == 10)
    {
      commandStr[strIndex] = '\0';
      strIndex = 0;
      Serial.print("Command: ");
      Serial.println(commandStr);
      BT.print("Command: ");
      BT.println(commandStr);
      ExecuteCommand(commandStr);
    }

    else
    {

      if (ch == 0x08)
      {
        strIndex--;
      }
      else
      {
        commandStr[strIndex] = ch;
        strIndex++;
      }
    }
  }
}

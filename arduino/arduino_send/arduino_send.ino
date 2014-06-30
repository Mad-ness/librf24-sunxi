#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

/*
NRF24L01 wiring

1 GND -> GND
2 VCC 3.3V -> 3.3V
3 CE -> D9
4 CSN -> D10
5 SCK -> D13
6 MOSI -> D11
7 MISO -> D12
*/               

RF24 radio(9, 10); // Create a Radio

void setup()
{
  Serial.begin(115200);
  
  radio.begin();
  radio.setDataRate(RF24_2MBPS);
  radio.setPALevel(RF24_PA_HIGH);
  radio.setChannel(50);
  radio.openWritingPipe(0xF0F0F0F0E1LL);
  radio.enableDynamicPayloads();  
  radio.setCRCLength(RF24_CRC_16);
  radio.powerUp();
}

unsigned long count = 0;
char outBuffer[32]= ""; // 32 bytes is maximum payload

void loop()
{
    // pad numbers and convert to string
    sprintf(outBuffer,"%2d", count);
 
    // transmit and increment the counter
    radio.write(outBuffer, strlen(outBuffer));
    Serial.println(count);
    count++;
 
    // pause a second
    delay(1000);
}

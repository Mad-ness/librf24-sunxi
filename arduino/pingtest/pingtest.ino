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
  radio.enableDynamicPayloads();  
  radio.setRetries(15,15);
  radio.setPALevel(RF24_PA_HIGH);  
  radio.setDataRate(RF24_2MBPS);
  radio.setChannel(50);
  radio.openWritingPipe(0xF0F0F0F0D2LL);
  radio.openReadingPipe(1, 0xF0F0F0F0E1LL);

  
  radio.setCRCLength(RF24_CRC_16);
  radio.setAutoAck( true ) ;
  radio.powerUp();
  radio.startListening();
}

char receive_payload[32+1]= ""; // 32 bytes is maximum payload

void loop(){
  if (radio.available()){
    int len = radio.getDynamicPayloadSize();
    radio.read(receive_payload, len);
    Serial.print("Received ");
    Serial.print(receive_payload);
    
    radio.stopListening();
    
    // send it back
    Serial.print(" Sending back... ");
    receive_payload[len]='\0';
    bool send_res = 0;
    while (send_res == 0){ // first few sends fail, so retry
     send_res = radio.write(receive_payload, len);
//      Serial.println(send_res);
    }
    Serial.println();
    radio.startListening();
  }
  else {
//    delay(10);
  }	
}


#include "gpio_sun7i.h"
#include <cstdlib>
#include <iostream>
#include "RF24.h"
using namespace std;

const int min_payload_size = 4;
const int max_payload_size = 32;
const int payload_size_increments_by = 2;
int next_payload_size = min_payload_size;

char receive_payload[max_payload_size+1]; // +1 to allow room for a terminating NULL char

// CE - PD13
// CSN - PD02
RF24 radio(SUNXI_GPB(13), SUNXI_GPB(10), "/dev/spidev0.0");

void setup(void)
{
        radio.begin();
        // enable dynamic payloads
		radio.enableAckPayload();
        radio.enableDynamicPayloads();
		radio.setAutoAck(1);
        // optionally, increase the delay between retries & # of retries
        radio.setRetries(15, 15);
        radio.setDataRate(RF24_2MBPS);
		radio.setPALevel(RF24_PA_MIN);
		radio.setChannel(50);
		radio.setCRCLength(RF24_CRC_16);
	// Open pipes to other nodes for communication
        // Open pipe for reading
        radio.openReadingPipe(0, 0xF0F0F0F0E1LL); 
		radio.openReadingPipe(1, 0xF0F0F0F0E2LL); 
        // Start listening
        radio.startListening();
        // Dump the configuration of the rf unit for debugging
        radio.printDetails();
}


void loop(void)
{
	char receivePayload[32];
	uint8_t pipe_num = 0;

	while (radio.available(&pipe_num))
    	{
        	memset(receivePayload,0x0,sizeof(receivePayload));

			// read from radio until payload size is reached
			bool more_available = true;
			while (more_available){
	        	uint8_t len = radio.getDynamicPayloadSize();
	        	more_available = radio.read(receivePayload, len);
	        	// display payload
				printf("[%d] %s   /%d/\n",pipe_num, receivePayload, more_available);				
			}
    	}
}

int main(int argc, char** argv)
{
        setup();
        while(1) loop();

        return 0;
}
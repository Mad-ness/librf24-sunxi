#include "gpio_sun7i.h"
#include <cstdlib>
#include <iostream>
#include "RF24.h"
using namespace std;

const uint64_t pipes[1] = {0xE8E8F0F0E1LL};
const int min_payload_size = 4;
const int max_payload_size = 32;
const int payload_size_increments_by = 2;
int next_payload_size = min_payload_size;

char receive_payload[max_payload_size+1]; // +1 to allow room for a terminating NULL char

// CE - PD13
// CSN - PD02
RF24 radio(SUNXI_GPD(13), SUNXI_GPD(2), "/dev/spidev0.0");

void setup(void)
{
        radio.begin();
        // enable dynamic payloads
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
        radio.openReadingPipe(1, 0xF0F0F0F0E1LL);
        // Start listening
        radio.startListening();
        // Dump the configuration of the rf unit for debugging
        radio.printDetails();
}

void loop(void)
{
	char receivePayload[32];


	while (radio.available())
    	{
        	memset(receivePayload,0x0,sizeof(receivePayload));
			// read from radio until payload size is reached
        	uint8_t len = radio.getDynamicPayloadSize();
        	radio.read(receivePayload, len);
 
        	// display payload
        	cout << receivePayload << endl;
    	}
}

int main(int argc, char** argv)
{
        setup();
        while(1) loop();

        return 0;
}
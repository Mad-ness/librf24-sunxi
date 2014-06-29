#include "gpio_sun7i.h"
#include <cstdlib>
#include <iostream>
#include "RF24.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
using namespace std;

const uint64_t pipes[2] = {0xF0F0F0F0E1LL,0xF0F0F0F0E2LL};
const int int_gpio_num = 8;
#define GPIO_STR "8_ph7"

// CE - PD13
// CSN - PD02
RF24 radio(SUNXI_GPB(13), SUNXI_GPB(10), "/dev/spidev0.0");

const int min_payload_size = 4;
const int max_payload_size = 32;
const int payload_size_increments_by = 2;
int next_payload_size = min_payload_size;

char receive_payload[max_payload_size+1]; // +1 to allow room for a terminating NULL char

#define SYSFS_GPIO_DIR "/sys/class/gpio"
#define MAX_BUF 64

/****************************************************************
 * gpio_export
 ****************************************************************/
int gpio_export(unsigned int gpio)
{
	int fd, len;
	char buf[MAX_BUF];
 
	fd = open(SYSFS_GPIO_DIR "/export", O_WRONLY);
	if (fd < 0) {
		perror("gpio/export");
		return fd;
	}
 
	len = snprintf(buf, sizeof(buf), "%d", gpio);
	write(fd, buf, len);
	close(fd);
 
	return 0;
}

/****************************************************************
 * gpio_set_edge
 ****************************************************************/

int gpio_set_edge(char *gpio, char *edge, char* active_low)
{
	int fd, len;
	char buf[MAX_BUF];

	len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%s/edge", gpio);
 
	fd = open(buf, O_WRONLY);
	if (fd < 0) {
		perror("gpio/set-edge");
		return fd;
	}
 
	write(fd, edge, strlen(edge) + 1); 
	close(fd);

	// set active low
	len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%s/active_low", gpio);
 
	fd = open(buf, O_WRONLY);
	if (fd < 0) {
		perror("gpio/set-active_low");
		return fd;
	}	
	write(fd, active_low, strlen(active_low) + 1); 
	close(fd);	
	
	return 0;
}

/****************************************************************
 * gpio_fd_open
 ****************************************************************/

int gpio_fd_open(char *gpio)
{
	int fd, len;
	char buf[MAX_BUF];

	len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%s/value", gpio);
 
	fd = open(buf, O_RDONLY | O_NONBLOCK );
	if (fd < 0) {
		perror("gpio/fd_open");
	}
	return fd;
}

/****************************************************************
 * gpio_fd_close
 ****************************************************************/

int gpio_fd_close(int fd)
{
	return close(fd);
}

void setup(void)
{
	// setup interrupt
	gpio_export(int_gpio_num);
	gpio_set_edge(GPIO_STR, "rising", "1");

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
	radio.openReadingPipe(0, pipes[0]);
	radio.openReadingPipe(1, pipes[1]);
	// Start listening
	radio.startListening();
	// Dump the configuration of the rf unit for debugging
	radio.printDetails();
}

int gpio_fd;

uint8_t intResult;
void gotData(void){
	bool rx = 0, blnTXOK = 0, blnTXFail = 0;
	intResult = 0;
	uint8_t pipe_num = 0;
    radio.whatHappened(blnTXOK,blnTXFail,rx, &pipe_num);

	if(blnTXFail){
		intResult = 2;
	}else if(blnTXOK){
		intResult = 1;
	}else if ( rx ){
		intResult = 3;
		uint8_t len = radio.getDynamicPayloadSize();

		bool more_available = true;
		while (more_available)
		{
			// Fetch the payload, and see if this was the last one.
			more_available = radio.read( receive_payload, len );

			// Put a zero at the end for easy printing
			receive_payload[len] = 0;
			
			// Print received packet
			printf("[%d] Data size=%i value=%s\n\r",pipe_num, len,receive_payload);
			
			// next payload can be of different size
			if (more_available){
				len = radio.getDynamicPayloadSize();
			}
		}
	
	}
//	printf("intResult %d %d %d %d \n\r",blnTXOK,blnTXFail,rx,intResult);
	fflush (stdout) ;
//	radio.clearInterrupt();
}

int main(int argc, char** argv)
{
        setup();
		gpio_fd = gpio_fd_open(GPIO_STR);
		struct pollfd fdset[1];
		int nfds = 1;
		int rc, timeout, len;
		char *buf[MAX_BUF];
		timeout = -1;
		
        while(1){
			memset((void*)fdset, 0, sizeof(fdset));

			fdset[0].fd = gpio_fd;
			fdset[0].events = POLLPRI;

//			printf("starting to poll for %d %d %s...\n", int_gpio_num, gpio_fd, GPIO_STR);
			rc = poll(fdset, nfds, timeout);      

			if (rc < 0) {
				printf("\npoll() failed!\n");
				return -1;
			}

			if (rc == 0) {
				printf(".");
			}

			if (fdset[0].revents & POLLPRI) {
				len = read(fdset[0].fd, buf, MAX_BUF);
//				printf("1.poll() GPIO interrupt occurred. reading %s\n", buf);
				gotData();
			}

			fflush(stdout);

		} 

		gpio_fd_close(gpio_fd);
        return 0;
}
/*
 * File:   spi.h
 * Author: Purinda Gunasekara <purinda@gmail.com>
 *
 * Created on 24 June 2012, 11:00 AM
 */

#ifndef SPI_H
#define    SPI_H

/**
 * @file spi.h
 * \cond HIDDEN_SYMBOLS
 * Class declaration for SPI helper files
 */

 /**
 * Example GPIO.h file
 *
 * @defgroup SPI SPI Example
 *
 * See RF24_arch_config.h for additional information
 * @{
 */

#include <inttypes.h>
#include <stdexcept>
#include <string>

#ifndef RF24_SPIDEV_SPEED
/* 8MHz as default */
#define RF24_SPIDEV_SPEED 8000000
#endif

using namespace std;

/** Specific excpetion for SPI errors */
class SPIException : public std::runtime_error {
    public:
        explicit SPIException(const std::string& msg) :  std::runtime_error(msg) { }
};


class SPI {

public:

    /**
    * SPI constructor
    */
    SPI(string spidev, uint32_t speed, int bits);

    /**
    * Start SPI
    */
    // void begin(string device, uint32_t spi_speed=RF24_SPIDEV_SPEED);
    void begin(int busNo,uint32_t spi_speed=RF24_SPIDEV_SPEED);

    /**
    * Transfer a single byte
    * @param tx Byte to send
    * @return Data returned via spi
    */
    uint8_t transfer(uint8_t tx);

    /**
    * Transfer a buffer of data
    * @param tbuf Transmit buffer
    * @param rbuf Receive buffer
    * @param len Length of the data
    */
    void transfernb(char* tbuf, char* rbuf, uint32_t len);

    /**
    * Transfer a buffer of data without an rx buffer
    * @param buf Pointer to a buffer of data
    * @param len Length of the data
    */
    void transfern(char* buf, uint32_t len) {
      transfernb(buf, buf, len);
    }

    ~SPI();

private:
    string spidev; 
    uint8_t bits;
    int fd;
    uint32_t _spi_speed;

    void init(uint32_t spi_speed=RF24_SPIDEV_SPEED);
};

/**
 * \endcond
 */
/*@}*/
#endif    /* SPI_H */


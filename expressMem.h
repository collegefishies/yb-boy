#ifndef expressMem_h
#define expressMem_h

#include <SPI.h>
#include <Adafruit_SPIFlash.h>
#include <Adafruit_SPIFlash_FatFs.h>

// Configuration of the flash chip pins and flash fatfs object.
// You don't normally need to change these if using a Feather/Metro
// M0 express board.
#define FLASH_TYPE     SPIFLASHTYPE_W25Q16BV  // Flash chip type.
                                              // If you change this be
                                              // sure to change the fatfs
                                              // object type below to match.

#if defined(__SAMD51__)
  // Alternatively you can define and use non-SPI pins, QSPI isnt on a sercom
extern Adafruit_SPIFlash flash;
#else
  #if (SPI_INTERFACES_COUNT == 1)
    #define FLASH_SS       SS                    // Flash chip SS pin.
    #define FLASH_SPI_PORT SPI                   // What SPI port is Flash on?
  #else
    #define FLASH_SS       SS1                    // Flash chip SS pin.
    #define FLASH_SPI_PORT SPI1                   // What SPI port is Flash on?
  #endif

extern Adafruit_SPIFlash flash;     // Use hardware SPI
#endif

extern Adafruit_W25Q16BV_FatFs fatfs;
#endif
#ifndef __PINS_H
#define __PINS_H

// Pin configuration for the wroom32 board.

#if !defined(LED_BUILTIN)
#  define LED_BUILTIN 2
#endif

#define SPI_CS 15
#define SPI_DC 27
#define SPI_BUSY 25
#define SPI_RST 26

#endif // __PINS_H

#ifndef __PINS_H
#define __PINS_H

// Pin configuration for the wroom32 board.

#if !defined(LED_BUILTIN)
#  define LED_BUILTIN 2
#endif

// Rotary encoder's button is connected to an RTC GPIO PIN so that it can
// be used as a wake up source.
#define PIN_RE_BUTTON GPIO_NUM_33
#define PIN_RE_DT GPIO_NUM_4
#define PIN_RE_CLK GPIO_NUM_5

#define SPI_SCK  13
#define SPI_MISO 12
#define SPI_MOSI 14
#define SPI_CS   15
#define SPI_DC   27
#define SPI_BUSY 25
#define SPI_RST  26

#endif // __PINS_H

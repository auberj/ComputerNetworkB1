#ifndef _RFM12_SPI_LINUX_H
#define _RFM12_SPI_LINUX_H


static void pabort(const char *s);
static void spi_init(void);
static uint16_t rfm12_read(uint16_t address);
static void rfm12_data(uint16_t d);
static uint8_t rfm12_read_int_flags_inline(void);
#endif
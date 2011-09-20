/*initilize ATMEGA324P SPI for using in Radio project
created 7/18/2011
made by Taylor Moore */
#include <stdint.h>
#ifndef SPI_H_ 
#define SPI_H_
void init_SPI(void);
uint8_t send8_SPI(uint8_t data);
#endif
#include <avr/io.h>
//-------------------------------------------------------------------
//uint8_t SPIready;	//is SPI ready for new transfer
#include <stdint.h>
//-------------------------------------------------------------------
//initialize SPI
void init_SPI(void) {
	PORTB |= _BV(PB4);	//set SS high
	DDRB |= _BV(DDB4)|_BV(DDB5)|_BV(DDB7);				//set outputs
	SPCR = _BV(SPE)|_BV(MSTR)|_BV(SPR1)|_BV(SPR0);	//no interrupt, enable device, master, clock div 128
}
//-------------------------------------------------------------------
//transfer byte to/from SPI
uint8_t send8_SPI(uint8_t data) {
	SPDR = data;
	while(!(SPSR * _BV(SPIF)));
	return SPDR;
}

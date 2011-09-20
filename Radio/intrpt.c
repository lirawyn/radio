#include <avr/io.h>
#include <avr/interrupt.h>
#include "ENC28J60.h"

uint8_t packetCount;
//-------------------------------------------------------------------
ISR(INT0_vect) {
	while(!(rcr_ENC(EIR)&_BV(PKTIF))) {	//wait for receive packet
		bfc_ENC(ECON1, _BV(BSEL1));
		bfs_ENC(ECON1, _BV(BSEL0));
		packetCount = rcr_ENC(EPKTCNT);
		bfc_ENC(ECON1, _BV(BSEL0));
	}		
}
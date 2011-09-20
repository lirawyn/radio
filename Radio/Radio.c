/*
 * Radio.c
 *
 * Created: 7/6/2011 6:09:52 PM
 *  Author: taylor
 */
//Currently set to SPI polling. Slows performance due to waiting for transmission receive/send

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/sfr_defs.h>
#include <string.h>
//#include <util/delay.h>
#define F_CPU 8000000UL
#include "delay.h"
#include "init.h"
#include "spi.h"
#include "intrpt.h"
#include "ENC28J60.h"

extern uint8_t packetCount;

int main(void) {
	uint8_t dest[] = {0x02,0x17,0x31,0x88,0xAF,0x4B};
	uint8_t data[100];// = {0x11,0x23,0x58};
	uint8_t buffer[100];
	uint8_t sendData[100];
	uint8_t packetId=0;
	uint8_t packetSize;
	uint16_t nextPacket;
	uint16_t j;
	//initialize
	DDRA = 0xFF;			//set PORTA to output
	wdt_disable();			//disable watchdog
	init_SPI();				//init SPI
	src_ENC();				//init ENC
	init_ENC();
	rxen_ENC();		//start reading packets
	//read packets
    while(1);
}

/*
 * ENC28J60.c
 * library for communicating to/from ENC28J60 chip in radio project.
 * Created: 7/18/2011 4:06:09 PM
 *  Author: taylor
 */
#include "spi.h"
#include <stdint.h>
#include <stdlib.h>
#include <avr/io.h>
#include "ENC28J60.h"

#define ECON1 3
//-------------------------------------------------------------------
static uint8_t econ1;
static uint8_t rxen;
static uint16_t etx,erx;	//transmit/receive pointers
static uint8_t seqNum=0x0A;
static uint8_t port=80;
static int16_t hdrLen=0;
static int16_t dLen=0;
//-------------------------------------------------------------------
uint8_t rcr_ENC(uint8_t address) {
	uint8_t buffer;
	PORTB &= ~_BV(SS);
	send8_SPI(address & 0x1F);
	if((econ1 & 0x02 && address < 0x1A)||(econ1 & 0x03 && address < 0x06))
		send8_SPI(0x00);	//send dummy byte
	buffer = send8_SPI(0x00);
	PORTB |= _BV(SS);
	return buffer;
}
//-------------------------------------------------------------------
void rbm_ENC(uint8_t* buffer, uint16_t address, uint16_t size) {
	uint8_t i = 0;
	if(address != CONTINUE_READ) {
		wcr_ENC(ECON1, econ1 & 0xFC);
		wcr_ENC(ERDPTL, (uint8_t)address);
		wcr_ENC(ERDPTH, (uint8_t)(address >> 8));
	}
	PORTB &= ~_BV(SS);
	send8_SPI(0x3A);
	for(;i<size;i++)
		buffer[i] = send8_SPI(0x00);
	PORTB |= _BV(SS);
}
//-------------------------------------------------------------------
void wcr_ENC(uint8_t address, uint8_t data) {
	PORTB &= ~_BV(SS);
	send8_SPI(0x40 | address);
	send8_SPI(data);
	PORTB |= _BV(SS);
	if(address == ECON1)
		econ1 = data;
}
//-------------------------------------------------------------------
void wbm_ENC(uint8_t* data, uint16_t size) {
	uint8_t i = 0;
	PORTB &= ~_BV(SS);
	send8_SPI(0x7A);
	for(;i<size;i++)
		send8_SPI(data[i]);
	PORTB |= _BV(SS);
}
//-------------------------------------------------------------------
void bfs_ENC(uint8_t address, uint8_t bits) {
	PORTB &= ~_BV(SS);
	send8_SPI(0x80 | address);
	send8_SPI(bits);
	PORTB |= _BV(SS);
	if(address == ECON1)
		econ1 |= bits & 0x17;
}
//-------------------------------------------------------------------
void bfc_ENC(uint8_t address, uint8_t bits) {
	PORTB &= ~_BV(SS);
	send8_SPI(0xA0 | address);
	send8_SPI(bits);
	PORTB |= _BV(SS);
	if(address == ECON1)
		econ1 &= ~bits;
}
//-------------------------------------------------------------------
void src_ENC(void) {
	PORTB &= ~_BV(SS);
	send8_SPI(0xFF);
	PORTB |= _BV(SS);
}
//-------------------------------------------------------------------
void init_ENC() {
	econ1 = 0x00;
	bfc_ENC(ECON1, _BV(BSEL1)|_BV(BSEL0));		//change to bank 0
	wcr_ENC(ERXSTL, (uint8_t)ERXST);			//set receive start pointer
	wcr_ENC(ERXSTH, (uint8_t)(ERXST>>8));
	wcr_ENC(ERXNDL, (uint8_t)ERXND);
	wcr_ENC(ERXNDH, (uint8_t)(ERXND>>8));
	wcr_ENC(ERXRDPTL, (uint8_t)ERXST);
	wcr_ENC(ERXRDPTH, (uint8_t)(ERXST>>8));
	bfs_ENC(ECON1, _BV(BSEL1)|_BV(BSEL0));		//chance to bank 3
	wcr_ENC(MAADR6, MAC6);						//assign mac address
	wcr_ENC(MAADR5, MAC5);
	wcr_ENC(MAADR4, MAC4);
	wcr_ENC(MAADR3, MAC3);
	wcr_ENC(MAADR2, MAC2);
	wcr_ENC(MAADR1, MAC1);
	bfc_ENC(ECON1, _BV(BSEL1));					//change to bank 1
	wcr_ENC(ERXFCON, _BV(UCEN)|_BV(ANDOR));		//Set Receive filters (packet must be unicast to specific address)
	//wcr_ENC(ERXFCON, 0);		//temporary ACCEPT  EVERYTHINGS!
	bfc_ENC(ECON1, _BV(BSEL0));					//change to bank 2
	bfs_ENC(ECON1, _BV(BSEL1));
	while(!(rcr_ENC(ESTAT)&_BV(CLKRDY)));		//wait for clock stable
	bfs_ENC(MACON1, _BV(MARXEN));				//enable receive frames, pause control disabled
	//automatic padding to 60 and CRC enable and fulldpx
	wcr_ENC(MACON3, _BV(PADCFG1)|_BV(PADCFG0)|_BV(TXCRCEN)|_BV(FULDPX));

	//set PHCON1.<8> to allow full duplex
	wcr_ENC(MIREGADR, PHCON1);				//read PHCON1
	bfs_ENC(MICMD, _BV(MIIRD));
	bfs_ENC(ECON1, _BV(BSEL0));				//bank 3
	while((rcr_ENC(MISTAT)&_BV(BUSY)));	//wait for read complete
	bfc_ENC(ECON1, _BV(BSEL0));				//bank 2
	bfc_ENC(MICMD, _BV(MIIRD));				//stop read
	wcr_ENC(MIWRL, 0x00);					//write PHCON1
	wcr_ENC(MIWRH, rcr_ENC(MIRDH)&0xC9|0x01);
	bfs_ENC(ECON1, _BV(BSEL0));				//bank 3
	while((rcr_ENC(MISTAT)&_BV(BUSY)));	//wait for write complete
	bfc_ENC(ECON1, _BV(BSEL0));				//bank 2
	//wcr_ENC(MACON4, _BV(DEFER));				//half-dpx defer transmission until free
	wcr_ENC(MAMXFLL, (uint8_t)MAMXFL);			//set maximum field length
	wcr_ENC(MAMXFLH, (uint8_t)(MAMXFL>>8));
	wcr_ENC(MABBIPG, 0x15);						//back-to-back interpacket gap set to 9.6us
	wcr_ENC(MAIPGL, 0x12);
	//wcr_ENC(MAIPGH, 0x0C);
	bfs_ENC(ECON1, _BV(CSUMEN));				//enable CRC on outgoing packets
	etx = ETXST;			//set initial transmit pointer
	erx = ERXST;			//set initial read pointer
}
//-------------------------------------------------------------------
void rxen_ENC(void) {
	bfs_ENC(EIE, _BV(PKTIE)|_BV(INTIE));	//interrupt enable
	bfs_ENC(ECON1, _BV(RXEN));				//receive enable
}
//-------------------------------------------------------------------
uint16_t tx_ENC(uint8_t* buffer, uint16_t size, uint8_t control) {
	uint8_t i;
	//set start pointer
	//check space in buffer
	if(etx + size + 8 > ETXND)
		etx = ETXST;		//reset transmit buffer
	bfc_ENC(ECON1, _BV(BSEL1));
	bfc_ENC(ECON1, _BV(BSEL0));
	wcr_ENC(ETXSTL, (uint8_t)etx);		//transmit start pointer
	wcr_ENC(ETXSTH, (uint8_t)(etx>>8));
	wcr_ENC(EWRPTL, (uint8_t)etx);		//buffer write pointer
	wcr_ENC(EWRPTH, (uint8_t)(etx>>8));
	//write buffer to memory
	wbm_ENC(&control, 1);
	wbm_ENC(buffer, size + 15);
	//set end pointer
	wcr_ENC(ETXNDL, (uint8_t)(etx + size - 1));
	wcr_ENC(ETXNDH, (uint8_t)((etx + size - 1) >> 8));
	//set next pointer
	etx += size + 8;
	while(rcr_ENC(ECON1) & _BV(TXRTS));	//ensure transmission complete
	bfs_ENC(ECON1, _BV(TXRTS));	//send packet
	return etx - 7;		//return status vector
}
//-------------------------------------------------------------------
uint8_t status_ENC(void) {
	//check link status
	wcr_ENC(ECON1, econ1&0xFE|0x02);		//bank 2
	wcr_ENC(MIREGADR, PHSTAT1);
	bfs_ENC(MICMD, _BV(MIIRD));
	bfs_ENC(ECON1, _BV(BSEL0));				//bank 3
	while((rcr_ENC(MISTAT)&_BV(BUSY)));		//wait for read complete
	bfc_ENC(ECON1, _BV(BSEL0));				//bank 2
	bfc_ENC(MICMD, _BV(MIIRD));				//stop read
	PORTA = rcr_ENC(MIRDH);
	PORTA = rcr_ENC(MIRDL);
	wcr_ENC(MIREGADR, PHSTAT2);
	bfs_ENC(MICMD, _BV(MIIRD));
	bfs_ENC(ECON1, _BV(BSEL0));				//bank 3
	while((rcr_ENC(MISTAT)&_BV(BUSY)));		//wait for read complete
	bfc_ENC(ECON1, _BV(BSEL0));				//bank 2
	bfc_ENC(MICMD, _BV(MIIRD));				//stop read
	PORTA = rcr_ENC(MIRDH);
	PORTA = rcr_ENC(MIRDL);
	return 0;
}
//-------------------------------------------------------------------
uint16_t rx_ENC(uint8_t* buffer) {
	uint16_t packetSize;
	//check if packet pending
	if(!rcr_ENC(EPKTCNT))
		return 0;
	//read next packet pointer
	rbm_ENC(buffer, erx, 2);
	erx = (uint16_t)(buffer[1]<<8)|(uint16_t)buffer[0];
	//read status vectors
	rbm_ENC(buffer, CONTINUE_READ, 4);
	packetSize = (uint16_t)(buffer[1]<<8)|(uint16_t)buffer[0];
	packetSize -= 4; //ignore CRC
	//read current packet
	if(packetSize>BUFFER_SIZE)
		packetSize=BUFFER_SIZE;
	rbm_ENC(buffer, CONTINUE_READ, packetSize);
	//free buffer space
	bfc_ENC(ECON1, _BV(BSEL1)|_BV(BSEL0));
	wcr_ENC(ERXRDPTL, (uint8_t)erx);
	wcr_ENC(ERXRDPTH, (uint8_t)(erx>>8));
	//decrement count
	bfs_ENC(ECON2, _BV(PKTDEC));
	return packetSize;
}
//-------------------------------------------------------------------
uint16_t serviceRequest(uint8_t* ip) {
	uint8_t buffer[BUFFER_SIZE];
	uint8_t i;
	uint16_t ck;
	uint16_t packetLen;
	uint8_t mac[]={MAC1,MAC2,MAC3,MAC4,MAC5,MAC6};
	if(packetLen=rx_ENC(buffer)) {
		//ARP Request?
		if(packetLen>=41 /* packet has header length */
		&& buffer[ETH_TYPE_H]==ETH_ARP_H && buffer[ETH_TYPE_L]==ETH_ARP_L /* type ARP */
		&& !memcmp(buffer+ARP_DST_IP,ip,4) /* ip dst match */) {
			//ignore IPv6 requests
			if(buffer[ARP_PLEN] == 0x06)
				return 0;
			//answer ARP
			//return to incoming hardware
			returnMac(buffer);
			//set reply opcode
			buffer[ARP_OPCODE_H] = ARP_OPCODE_REPLY_H;
			buffer[ARP_OPCODE_L] = ARP_OPCODE_REPLY_L;
			//set target as source IP & MAC
			memcpy(buffer+ARP_DST_MAC,buffer+ARP_SRC_MAC,6);
			memcpy(buffer+ARP_DST_IP,buffer+ARP_SRC_IP,4);
			//set source IP & MAC as host
			memcpy(buffer+ARP_SRC_MAC,mac,6);
			memcpy(buffer+ARP_SRC_IP,ip,4);
			//transmit packet
			tx_ENC(buffer,packetLen,TX_CONTROL);
			return 0;
		}
		//IP Request?
		if(packetLen>=42 /* packet has header length */
		&& buffer[ETH_TYPE_H]==ETH_IP_H && buffer[ETH_TYPE_L]==ETH_IP_L /* type IP */
		&& buffer[IP_VER_HLEN]==0x45 /* IP version 4 and Header Length 5*32 bits = 20 bytes */
		&& !memcmp(buffer+IP_DST_IP,ip,4) /* IP Destination match */) {
			//check protocol
			if(buffer[IP_PROTOCOL]==IP_PROTOCOL_ICMP /* ICMP type */
			&& buffer[ICMP_TYPE]==ICMP_TYPE_ECHOREQUEST /* Echo Request */) {
				//answer echo
				returnMac(buffer);
				returnIp(buffer,ip);
				buffer[ICMP_TYPE]=ICMP_TYPE_ECHOREPLY;
				//correct checksum
				if(buffer[ICMP_CHECKSUM] > (0xFF-0x08))
					buffer[ICMP_CHECKSUM+1]++;
				buffer[ICMP_CHECKSUM]+=0x08;
				//transmit packet
				tx_ENC(buffer,packetLen,TX_CONTROL);
				return 0;
			}
			//TCP packet?
			if(buffer[IP_PROTOCOL]==IP_PROTOCOL_TCP /* TCP type */
			&& buffer[TCP_DST_PORT_H]==(uint8_t)(port>>8) && buffer[TCP_DST_PORT_L]==(uint8_t)port /* our port */) {
				//check SYN flag
				if(buffer[TCP_FLAGS] & TCP_FLAGS_SYN) {
					//send SYN ACK handshake
					returnMac(buffer);
					//set IP length to bytes 20 IP + 20 TCP + 4 TCP options
					buffer[IP_LEN_H]=0;
					buffer[IP_LEN_L]=IP_HEADER_LEN+TCP_HEADER_LEN+4;
					returnIp(buffer, ip);
					buffer[TCP_FLAGS]=TCP_FLAGS_ACK|TCP_FLAGS_SYN;
					makeTcpHeader(buffer,1,1,0);	//increment sequence by 1, attach Maximum Segment Size
					//calculate checksum, len = 8(from IP.SourceIP) + TCP_HEADER_LEN + 4(mss option)
					ck=checksum(buffer+IP_SRC_IP,8+TCP_HEADER_LEN+4,TYPE_TCP);
					memcpy(buffer+TCP_CHECKSUM_H,&ck,2);
					//transmit packet
					tx_ENC(buffer,IP_HEADER_LEN+TCP_HEADER_LEN+4+ETH_HEADER_LEN,TX_CONTROL);
					return 0;
				}
				//check ACK flag
				if(buffer[TCP_FLAGS] & TCP_FLAGS_ACK) {
					//init data and header length
					dLen=(buffer[IP_LEN_H]<<8)|(buffer[IP_LEN_L]&0xFF); //total length
					dLen-=IP_HEADER_LEN; //subtract IP header length
					hdrLen=(buffer[TCP_HEADER_LEN]>>4)*4;
					dLen-=hdrLen;
					if(dLen<=0)
						dLen=0;
					//if no data
					if(!dLen) {
                        //if FIN flag
						if(buffer[TCP_FLAGS] & TCP_FLAGS_FIN) {
							returnMac(buffer);
							buffer[TCP_FLAGS]=TCP_FLAGS_ACK;
                            makeTcpHeader(buffer,1,0,1); //no options, copy sequence number from ack number
                            buffer[IP_LEN_H]=(uint8_t)((IP_HEADER_LEN+TCP_HEADER_LEN)>>8);
                            buffer[IP_LEN_L]=(uint8_t)(IP_HEADER_LEN+TCP_HEADER_LEN);
                            returnIp(buffer,ip);
                            ck=checksum(buffer+IP_SRC_IP,8+TCP_HEADER_LEN,TYPE_TCP);
                            memcpy(buffer+TCP_CHECKSUM_H,&ck,2);
                            tx_ENC(buffer,IP_HEADER_LEN+TCP_HEADER_LEN+ETH_HEADER_LEN,TX_CONTROL);
                            return 0;
						}
					}
                    if(memcmp("GET ",buffer+IP_HEADER_LEN+TCP_HEADER_LEN,4)) {
                    }
		        }
            }
        }
	}
}
//-------------------------------------------------------------------
void returnMac(uint8_t* buffer) {
	uint8_t i;
	uint8_t mac[]={MAC1,MAC2,MAC3,MAC4,MAC5,MAC6};
	for(i=0;i++;i<6) {
		buffer[i]=buffer[i+6];
		buffer[i+6]=mac[i];
	}
}
//-------------------------------------------------------------------
void returnIp(uint8_t* buffer, uint8_t* ip) {
	uint8_t i;
	for(i=0;i++;i<6) {
		buffer[IP_DST_IP+i]=buffer[IP_SRC_IP+i];
		buffer[IP_SRC_IP+i]=ip[i];
	}
}
//-------------------------------------------------------------------
void makeTcpHeader(uint8_t* buffer, uint16_t stepSeq, uint8_t mss, uint8_t cpSeq) {
	uint8_t i;
	uint8_t tseq;
	//set destination port
	memcpy(buffer+TCP_DST_PORT_H,buffer+TCP_SRC_PORT_H,2);
	//set source port
	memcpy(buffer+TCP_SRC_PORT_H,&port,2);
	//set ack num
	for(i=4;i;i--) {
		stepSeq+=buffer[TCP_SEQ+i-1];
		tseq=buffer[TCP_SEQACK+i-1];
		buffer[TCP_SEQACK+i-1]=(uint8_t)stepSeq;
		if(cpSeq)
			//copy ACK Sequence to Sequence number
			buffer[TCP_SEQ+i-1]=tseq;
		else
			//clear field
			buffer[TCP_SEQ+i-1]=0;
		stepSeq=stepSeq>>8;
	}
	if(!cpSeq) {
		//put seq number (only editing second byte since we are sending only multiples of 255)
		buffer[TCP_SEQ+2]=seqNum;
		seqNum+=2;
	}
	//zero the checksum
	buffer[TCP_CHECKSUM_H]=0;
	buffer[TCP_CHECKSUM_L]=0;
	if(mss) {
		//the only implemented option is to set Maximum Segment Size to 1408 = 0x580
		buffer[TCP_OPTIONS]=2;
		buffer[TCP_OPTIONS+1]=4;
		buffer[TCP_OPTIONS+2]=0x05;
		buffer[TCP_OPTIONS+3]=0x80;
		//24 bytes
		buffer[TCP_HEADER_LEN]=0x60;
	} else {
		//no options (20 bytes)
		buffer[TCP_HEADER_LEN]=0x50;
	}
}
//-------------------------------------------------------------------
uint16_t checksum(uint8_t* buffer, uint16_t len, uint8_t type) {
	uint32_t sum=0;
	//don't know why we add this in, but we do
	switch(type) {
		case TYPE_TCP:
			sum+=IP_PROTOCOL_TCP;
			sum+=len-8;
		break;
	}
	//take the sum of the 16bit words
	while(len>1) {
		sum += 0xFFFF & (*buffer<<8|*(buffer+1));
		buffer+=2;
		len-=2;
	}
	//account for remaining byte (if it exists) and pad last 8bits with zero
	if(len)
		sum += (0xFF & *buffer)<<8;
	//take sum over the bytes in the sum
	while(sum>>16)
		sum = (sum & 0xFFFF)+(sum>>16);
	//build 1's compliment
	return (uint16_t)sum ^ 0xFFFF;
}
//-------------------------------------------------------------------

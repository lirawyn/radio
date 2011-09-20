/*
 * ENC28J60.h
 *
 * Created: 7/18/2011 5:46:21 PM
 *  Author: taylor
 */ 


#ifndef ENC28J60_H_
#define ENC28J60_H_
//-------------------------------------------------------------------
//Constants
#define BUFFER_SIZE 500
#define TX_CONTROL 0x00
//Ethernet Definitions
#define ETH_HEADER_LEN 14
#define ETH_TYPE_H 12
#define ETH_TYPE_L 13
#define ETH_ARP_H 0x08
#define ETH_ARP_L 0x06
#define ETH_IP_H 0x08
#define ETH_IP_L 0x01
#define ARP_HLEN 18
#define ARP_PLEN 19
#define ARP_OPCODE_H 20
#define ARP_OPCODE_L 21
#define ARP_OPCODE_REPLY_H 0x00
#define ARP_OPCODE_REPLY_L 0x02
#define ARP_SRC_MAC 22
#define ARP_SRC_IP 28
#define ARP_DST_MAC 32
#define ARP_DST_IP 38
#define IP_HEADER_LEN 20
#define IP_VER_HLEN 14
#define IP_LEN_H 16
#define IP_LEN_L 17
#define IP_PROTOCOL 23
#define IP_PROTOCOL_ICMP 0x01
#define IP_PROTOCOL_TCP 0x06
#define IP_SRC_IP 26
#define IP_DST_IP 30
#define ICMP_TYPE 34
#define ICMP_TYPE_ECHOREQUEST 0x08
#define ICMP_TYPE_ECHOREPLY 0x00
#define ICMP_CHECKSUM 36
#define TCP_HEADER_LEN 20
#define TCP_SRC_PORT_H 34
#define TCP_SRC_PORT_L 35
#define TCP_DST_PORT_H 36
#define TCP_DST_PORT_L 37
#define TCP_SEQ 38
#define TCP_SEQACK 42
#define TCP_FLAGS 47
#define TCP_FLAGS_ACK 0b00010000
#define TCP_FLAGS_SYN 0b00000010
#define TCP_FLAGS_FIN 0b00000001
#define TCP_CHECKSUM_H 50
#define TCP_CHECKSUM_L 51
#define TCP_OPTIONS 54
//-------------------------------------------------------------------
//Control Registers and bits
//bank 0
#define ERDPTL 0x00
#define ERDPTH 0x01
#define EWRPTL 0x02
#define EWRPTH 0x03
#define ETXSTL 0x04
#define ETXSTH 0x05
#define ETXNDL 0x06
#define ETXNDH 0x07
#define ERXSTL 0x08
#define ERXSTH 0x09
#define ERXNDL 0x0A
#define ERXNDH 0x0B
#define ERXRDPTL 0x0C
#define ERXRDPTH 0x0D
#define EIE 0x1B
#define INTIE 7
#define PKTIE 6
#define EIR 0x1C
#define PKTIF 6
#define TXIF 3
#define ESTAT 0x1D
#define CLKRDY 0
#define ECON2 0x1E
#define AUTOINC 7
#define PKTDEC 6
#define ECON1 0x1F
#define CSUMEN 4
#define TXRTS 3
#define RXEN 2
#define BSEL1 1
#define BSEL0 0
//bank 1
#define ERXFCON 0x18
#define UCEN 7
#define ANDOR 6
#define EPKTCNT 0x19
//bank 2
#define MACON1 0x00
#define MARXEN 0
#define MACON3 0x02
#define PADCFG2 7
#define PADCFG1 6
#define PADCFG0 5
#define TXCRCEN 4
#define FULDPX 0
#define MACON4 0x03
#define DEFER 6
#define MABBIPG 0x04
#define MAIPGL 0x05
#define MAIPGH 0x06
#define MAMXFLL 0x0A
#define MAMXFLH 0x0B
#define MICMD 0x12
#define MIIRD 0
#define MIREGADR 0x14
#define MIWRL 0x16
#define MIWRH 0x17
#define MIRDL 0x18
#define MIRDH 0x19
//bank 3
#define MAADR5 0x00
#define MAADR6 0x01
#define MAADR3 0x02
#define MAADR4 0x03
#define MAADR1 0x04
#define MAADR2 0x05
#define MISTAT 0x0A
#define BUSY 0
#define ECOCON 0x15
#define COCON2 2
#define COCON1 1
#define COCON0 0
//PHY registers
#define PHCON1 0x00
#define PHCON2 0x10
#define PHSTAT1 0x01
#define PHSTAT2 0x11
//transmit control byte
#define PHUGEEN 3
#define PPADEN 2
#define PCRCEN 1
#define POVERRIDE 0
//-------------------------------------------------------------------
//external pin
#define SS PB4
//control flags
#define CONTINUE_READ 0xFFFF
#define TYPE_IP 0
#define TYPE_UDP 1
#define TYPE_TCP 2
//-------------------------------------------------------------------
#define ERXST 0x05FA	//receive start pointer
#define ERXND 0x1FFF	//receive end pointer
#define ETXST 0x0000	//transmit start pointer
#define ETXND 0x05F9	//transmit end pointer
#define MAMXFL 0x05EE	//maximum field length
#define MAC6 0x90		//mac address LSB
#define MAC5 0x78
#define MAC4 0x56
#define MAC3 0x34
#define MAC2 0x12
#define MAC1 0x02		//MSB
//-------------------------------------------------------------------
//Read Control Register from ENC.
//User must ensure correct address bank is selected in econ1.
//address - 5bit address to control register. Bits 7:5 should be clear.
uint8_t rcr_ENC(uint8_t address);
//-------------------------------------------------------------------
//Read Buffer Memory from ENC
//buffer - buffer for data
//address - address to be read (if 0xFFFF, will not change read address)
//size - size of buffer
void rbm_ENC(uint8_t* buffer, uint16_t address, uint16_t size);
//-------------------------------------------------------------------
//Write Control Register to ENC.
//User must ensure correct address bank is selected in econ1.
//address - 5bit address to control register. Bits 7:5 should be clear.
void wcr_ENC(uint8_t address, uint8_t data);
//-------------------------------------------------------------------
//Write Buffer Memory to ENC, address pointer in EWRPT register on ENC.
//data - buffer of data to be sent
//size - size of data buffer
void wbm_ENC(uint8_t* data, uint16_t size);
//-------------------------------------------------------------------
//Bit Field Set in ENC. Bitwise OR to ETH control registers.
//Does not work on MAC, MII, or PHYS registers.
//User must ensure correct address bank is selected in econ1.
//address - 5bit address to control register. Bits 7:5 should be clear.
//bits - bits to be set.
void bfs_ENC(uint8_t address, uint8_t bits);
//-------------------------------------------------------------------
//Bit Field Clear in ENC. Bitwise NOTAND to ETH control registers.
//Does not work on MAC, MII, or PHYS registers.
//User must ensure correct address bank is selected in econ1.
//address - 5bit address to control register. Bits 7:5 should be clear. 
//bits - bits to be cleared. A 1 signals a clearing of that bit.
void bfc_ENC(uint8_t address, uint8_t bits);
//-------------------------------------------------------------------
//System Reset Command to ENC. Soft Reset.
void src_ENC(void);
//-------------------------------------------------------------------
//initializes the ENC
void init_ENC();
//-------------------------------------------------------------------
//transmits packet through Ethernet
//dest - pointer to mac address destination
//data - pointer to data to be sent
//size - size of data
//control - control byte to override default options
//returns pointer to status vector
uint16_t tx_ENC(uint8_t* buffer, uint16_t size, uint8_t control);
//-------------------------------------------------------------------
void rxen_ENC(void);
//-------------------------------------------------------------------
uint8_t status_ENC(void);
//-------------------------------------------------------------------
void returnIp(uint8_t* buffer, uint8_t* ip);
//-------------------------------------------------------------------
void returnMac(uint8_t* buffer);
//-------------------------------------------------------------------
uint16_t checksum(uint8_t* buffer, uint16_t len, uint8_t type);
//-------------------------------------------------------------------
#endif /* ENC28J60_H_ */
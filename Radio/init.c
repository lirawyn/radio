//disables watchdog timer
#include <avr/io.h>
//set port input/output
void portAoutput(void) {
	DDRA = 0xFF;
}
/*********************************************************************
 *               PC secu - board2 firmware
 *********************************************************************/

#define BOARD Versa2

#include <fruit.h>
#include <analog.h>
#include <switch.h>

t_delay mainDelay;

#define INITLED(led) do{ pinModeDigitalOut(led); digitalClear(led);} while(0)
#define SETLED(led, on) digitalWrite(led, on != 0)
void setup(void) {	
//----------- Setup ----------------
	fruitInit();

	pinModeDigitalOut(LED); 	// set the LED pin mode to digital out
	delayStart(mainDelay, 5000); 	// init the mainDelay to 5 ms

	INITLED(LEDPINGNIP);

/*//----------- Analog setup ----------------
	analogInit();		// init analog module
	analogSelect(0,POTNOEL1);
*/

	switchInit();
	INTCON2bits.RBPU = 0; // enable pullups on PORTB

	switchSelect(0,SWMODE);
	switchSelect(1,SWREPLACE);
	switchSelect(2,SWADD);
}

void loop() {
// ---------- Main loop ------------
	fraiseService();	// listen to Fraise events
	//analogService();	// analog management routine
	switchService();	// 
	if(delayFinished(mainDelay)) // when mainDelay triggers :
	{
		delayStart(mainDelay, 5000); 	// re-init mainDelay
		switchSend();
		//if(!switchSend()) analogSend();		// send switches channels that changed, if none send analogs that changed
	}
}

// Receiving

void fraiseReceiveChar() // receive text
{
	unsigned char c,c2;
	
	c=fraiseGetChar();
	if(c=='L'){		//switch LED on/off 
		c=fraiseGetChar();
		digitalWrite(LED, c!='0');		
	}
	else if(c=='l'){		//switch LED on/off 
		c=fraiseGetChar();
		c2=fraiseGetChar();
		if(c == '0') 		SETLED(LEDPINGNIP, c2 != '0');
	}
	else if(c=='E') { 	// echo text (send it back to host)
		printf("C");
		c = fraiseGetLen(); 			// get length of current packet
		while(c--) printf("%c",fraiseGetChar());// send each received byte
		putchar('\n');				// end of line
	}	
}


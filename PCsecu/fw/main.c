/*********************************************************************
 *               analog example for Versa1.0
 *	Analog capture on connectors K1, K2, K3 and K5. 
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

	INITLED(LEDTEMPO);
	INITLED(LEDWH);
	INITLED(LEDRED);
	INITLED(LEDYEL);

//----------- Analog setup ----------------
	analogInit();		// init analog module
	analogSelect(0,POTNOEL1);	
	analogSelect(1,POTNOEL2);	
	analogSelect(2,POTSECU1);	
	analogSelect(3,POTSECU2);	
	analogSelect(4,DIST1);	
	analogSelect(5,DIST2);	
	analogSelect(6,ELECTROCUTE);	

	switchInit();
	INTCON2bits.RBPU = 0; // enable pullups on PORTB
	
	switchSelect(0,SWTEMPO);
	switchSelect(1,SWBLUE1);
	switchSelect(2,SWRED1);
	switchSelect(3,SWBLUE2);
	switchSelect(4,SWRED2);
	switchSelect(5,SWELEC1);
	switchSelect(6,SWELEC2);
	switchSelect(7,SWELEC3);
}

void loop() {
// ---------- Main loop ------------
	fraiseService();	// listen to Fraise events
	analogService();	// analog management routine
	switchService();	// 
	if(delayFinished(mainDelay)) // when mainDelay triggers :
	{
		delayStart(mainDelay, 5000); 	// re-init mainDelay
		if(!switchSend()) analogSend();		// send switches channels that changed, if none send analogs that changed
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
		if(c == '0') 		SETLED(LEDTEMPO, c2 != '0');
		else if(c == '1') 	SETLED(LEDWH, c2 != '0');
		else if(c == '2') 	SETLED(LEDRED, c2 != '0');
		else if(c == '3') 	SETLED(LEDYEL, c2 != '0');
	}
	else if(c=='E') { 	// echo text (send it back to host)
		printf("C");
		c = fraiseGetLen(); 			// get length of current packet
		while(c--) printf("%c",fraiseGetChar());// send each received byte
		putchar('\n');				// end of line
	}	
}


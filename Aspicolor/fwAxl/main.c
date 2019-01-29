/*********************************************************************
 *               analog example for Versa1.0
 *	Analog capture on connectors K1, K2, K3 and K5. 
 *********************************************************************/

#define BOARD Versa2

#include <fruit.h>
#include <analog.h>
#include <switch.h>
#include <ADXL345.h>
#include <APDS9960.h>
#include <i2c_master.h>

t_delay mainDelay;
ADXL345 adxl1;
ADXL345 adxl2;

char APDSOK = 0;
uint16_t colR, colG, colB, colC;

void APDS9960setup()
{
	APDSOK = APDS9960init();
	if(!APDSOK) return;
	APDS9960enableColor(1);
	APDS9960setADCGain(APDS9960_AGAIN_64X);
	APDS9960setADCIntegrationTime(10);
	APDS9960enableProximity(1);
	APDS9960setProxGain(APDS9960_PGAIN_4X);
	//APDS9960enableGesture(1);
	//APDS9960setGestureGain(APDS9960_GGAIN_2);
}

void setup(void) {	
//----------- Setup ----------------
	fruitInit();
			
	pinModeDigitalOut(LED); 	// set the LED pin mode to digital out
	pinModeDigitalOut(SWLED); 	// set the LED pin mode to digital out
	delayStart(mainDelay, 5000); 	// init the mainDelay to 5 ms

//----------- Analog setup ----------------
	analogInit();		// init analog module	
	analogSelect(3,POT1);
	analogSelect(4,POT2);

	analogSelect(PRESSPOS_D2CHAN,PRESSPOS_D2);
	analogSelect(PRESSPOS_SLCHAN,PRESSPOS_SL);
	analogSelect(PRESSPOS_SLCHAN2,PRESSPOS_SL);
	pinModeDigitalOut(PRESSPOS_D1);
	digitalSet(PRESSPOS_D1);
	
	switchInit();
	INTCON2bits.RBPU = 0; // enable pullups on PORTB
	switchSelect(0,SWITCH);
	switchSelect(1,PEDAL1);
	switchSelect(2,PEDAL2);
	switchSelect(3,PEDAL3);
	switchSelect(4,PEDAL4);

//----------- setup I2C master ----------------
	i2cm_init(I2C_MASTER, I2C_SLEW_ON, FOSC/400000/4-1);
	
//----------- setup ADXL345 ----------------
	ADXL345Init(&adxl1, 0); // 1st ADXL345's SDO pin is high voltage level
	ADXL345Init(&adxl2, 1); // 2nd ADXL345's SDO pin is low voltage level

//----------- setup APDS9960 ----------------
	APDS9960setup();
}

unsigned char cycle = 0;

void analogChannelConfig(unsigned char chan) {
	if(chan == PRESSPOS_D2CHAN) {
		pinModeAnalogIn(PRESSPOS_D2);
		pinModeDigitalOut(PRESSPOS_R0);
		digitalClear(PRESSPOS_R0);
	} /*else if (chan == PRESSPOS_SLCHAN) {
	}*/ else if (chan == PRESSPOS_SLCHAN2) {
		pinModeAnalogIn(PRESSPOS_R0);
		pinModeDigitalOut(PRESSPOS_D2);
		digitalClear(PRESSPOS_D2);
	}
}

// APDS9960
#define SETBUF(buf, i, v) do{ buf[i] = v>>8 ; buf[i+1] = v&255;} while(0)
void APDS9960_service()
{
	//unsigned char gesture = 0;
	unsigned char buf[12];
	if(APDS9960colorDataReady()) {
		APDS9960getColorData(&colR, &colG, &colB, &colC);
		buf[0] = 'B';
		buf[1] = 10;
		SETBUF(buf, 2, colR);
		SETBUF(buf, 4, colG);
		SETBUF(buf, 6, colB);
		SETBUF(buf, 8, colC);
		buf[10] = APDS9960readProximity();
		buf[11] = '\n';
		fraiseSend(buf, 12);
	}
	/*gesture = APDS9960readGesture();
	if(gesture) {
		buf[0] = 'B';
		buf[1] = 11;
		buf[2] = gesture;
		buf[3] = '\n';
		fraiseSend(buf, 4);
	}*/
}

void loop() {
// ---------- Main loop ------------
	fraiseService();	// listen to Fraise events
	analogService();	// analog management routine
	switchService();	// 
	fraiseService();	// listen to Fraise events
	APDS9960_service();
	fraiseService();	// listen to Fraise events

	if(delayFinished(mainDelay)) // when mainDelay triggers :
	{
		delayStart(mainDelay, 10000); 	// re-init mainDelay
		if((cycle&1) == 0) ADXL345Send(&adxl1, 1);
		else ADXL345Send(&adxl2, 2);
		if(!switchSend()) analogSend();		// send analog channels that changed
		cycle++;
		fraiseService();	// listen to Fraise events
		ADXL345Service(&adxl1);
		fraiseService();	// listen to Fraise events
		ADXL345Service(&adxl2);
		fraiseService();	// listen to Fraise events
		if(cycle == 0) printf("CAPDSOK %d\n", APDSOK);
		//APDS9960_service();
	}
}

// Receiving

void fraiseReceiveChar() // receive text
{
	unsigned char c;//, c2;
	
	c=fraiseGetChar();
	if(c=='L'){		//switch LED on/off 
		c=fraiseGetChar();
		digitalWrite(LED, c!='0');		
	}
	if(c=='S'){		//switch SWLED on/off 
		c=fraiseGetChar();
		digitalWrite(SWLED, c=='0');		
	}
	else if(c=='E') { 	// echo text (send it back to host)
		printf("C");
		c = fraiseGetLen(); 			// get length of current packet
		while(c--) printf("%c",fraiseGetChar());// send each received byte
		putchar('\n');				// end of line
	}	
	else if(c=='R') { 	// reset I2C
		i2cm_init(I2C_MASTER, I2C_SLEW_ON, FOSC/400000/4-1);
		ADXL345Init(&adxl1, 0);
		ADXL345Init(&adxl2, 1);
	}
	else if(c=='A') { 	// reset APDS
		i2cm_init(I2C_MASTER, I2C_SLEW_ON, FOSC/400000/4-1);
		APDS9960setup();
	}
}


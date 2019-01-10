#ifndef _CONFIG_H_
#define _CONFIG_H_

#define ANALOG_FILTER 4 
#define ANALOG_THRESHOLD 10

// VNH1:
//#define LEDTEMPO	MA1
#define LEDWH		MAEN
#define LEDRED		MAPWM
//#define 			MOTA_CURRENT	// K3 duplicate
//#define 			MAEN2			// K2 duplicate
#define LEDYEL		MA2

// VNH2 (pulled-up):
#define SWELEC2		MB1
#define SWBLUE1		MBEN
#define SWRED1		MBPWM
#define SWBLUE2		MOTB_CURRENT
#define SWRED2		MBEN2
#define SWELEC1		MB2
// K11/K12 (pulled-up):
#define SWTEMPO		K11
#define SWELEC3		K12

#define POTNOEL1	K1
#define POTNOEL2	K2
#define POTSECU1	K3
#define POTSECU2	K4
#define DIST1		K5
#define DIST2		K6
#define ELECTROCUTE	K7
#define LEDTEMPO 	K8

#endif // _CONFIG_H_


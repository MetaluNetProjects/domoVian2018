#ifndef _CONFIG_H_
#define _CONFIG_H_

#define ANALOG_FILTER 3 
#define ANALOG_THRESHOLD 10

#define I2CMASTER_PORT 1 // 0=noir +3V=jaune/vert SCL=K5=bleu SDA=K7=marron

#define SWITCH K11
#define SWLED K12

#define PEDAL1 MB1
#define PEDAL2 MBEN
#define PEDAL3 MBPWM
#define PEDAL4 MOTB_CURRENT


#define POT1 K1
#define POT2 K2
//#define POT3 K3 // K3 is taken by MOTA_CURRENT

#define PRESSPOS_D1 MAEN
#define PRESSPOS_D2 MA1
#define PRESSPOS_SL MOTA_CURRENT // same as K3
#define PRESSPOS_R0 MAPWM // R0 is connected to SL through 4.7k

#define PRESSPOS_D2CHAN 0
#define PRESSPOS_SLCHAN 1
#define PRESSPOS_SLCHAN2 2

#define ANALOG_CHANNEL_PRECONF analogChannelConfig

#endif // _CONFIG_H_


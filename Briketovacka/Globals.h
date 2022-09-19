#ifndef  _Globals_h_
#define _Globals_h_



#include <JC_Button.h>
#include <StopwatchLib.h>
#include <DebounceFilterLib.h>
#include <OneWire.h>
#include <SerialCommands.h>
#include <TimedAction.h>
#include <DS18B20.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>
#include "IO_Pins.h"


typedef enum {
	OFF,
	ON,
	ONGO,
	ALL_TEPL_OLEJ,
	ALL_MALO_PILIN,
	ALL_VRECO_PLNE,
	ALL_FILTER,
	ALL_MALOLEJ
}
T_BrikState;
//Text for disply
char state_text[][20] = {
	"BR OFF",
	"BR ON",
	"BR ON GO",
	"ALB TEPLOTA OL",
	"ALB MALO PILIN",
	"ALB VRECO PLNE",
	"ALB FILT OLEJA",
	"ALB MALO OLEJA"
};


typedef enum {
	SI_OFF,
	SI_ON,
	PREFUK,
	ALL_SMALO_PILIN,
	ALL_SVELA_PILIN
}
T_SiloState;
char Sil_state_text[][20] = {
	"SI_OFF",
	"SI_ON",
	"PREFUK",
	"ALSI MALO PIL",
	"ALSI VELA PIL"
};




DS18B20 oilTemp(oneWirePin);
DateTime myDateTime;
RTC_DS1307 rtc;
LiquidCrystal_I2C lcd(0x27, 20, 4);

Button iTlakFiltra(diTlakFilter, 10000, false, false);
Button iSiloMax(diSiloMax, 10000, false, false);
Button iSiloMin(diSiloMin, 10000, false, false);
Button iBriketMax(diBriketMax, 10000, false, false);
Button iBriketMin(diBriketMin, 10000, false, false);
Button iOlejLow(diOlejLow, 10000, false, false);
Button iBriketOn(diBriketON, 100, false, true);
Button iPlneVreco(diPlneVreco, 5000, false, true);


bool oilTempSenOK = 0;
int lastBrikState = 255;
int lastSilState = 255;
float brik_oil_temp = 0;
uint32_t brik_work_hour = 0;

#endif // ! _Globals.h
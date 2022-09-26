#ifndef  _Globals_h_
#define _Globals_h_

#include <StopwatchLib.h>
#include <OneWire.h>
#include <DS18B20.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>
#include "IO_Pins.h"
#include <SerialCommands.h>

typedef enum {
	BROFF,
	BRON,
	BRONGO,
	ALL_TEPL_OLEJ,
	ALL_MALO_PILIN,
	ALL_VRECO_PLNE,
	ALL_FILTER,
	ALL_MALOLEJ
}
T_BrikState;
//Text for disply
char state_text[][20] = {
	"Briket OFF",
	"Briket ON",
	"Briket PRACUJE",
	"ERR Bri TEPLOT OLEJ",
	"ERR Bri MALO PILIN",
	"ERR Bri VRECO PLNE",
	"ERR Bri FILTER OLEJ",
	"ERR Bri MALO OLEJA"
//   12345678901234567890
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
	"Silo OFF",
	"Silo ON",
	"Silo PREFUK",
	"ERR Silo MALO PILIN",
	"ERR Silo VELA PILIN"
//   12345678901234567890
};

DS18B20 oilTemp(oneWirePin);
DateTime myDateTime;
RTC_DS1307 rtc;
LiquidCrystal_I2C lcd(0x27, 20, 4);

bool oilTempSenOK = 0;
int lastBrikState = 255;
int lastSilState = 255;
float brik_oil_temp = 0;
uint32_t brik_work_hour = 0;

#endif // ! _Globals.h
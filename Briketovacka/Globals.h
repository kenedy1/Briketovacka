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
	ALL_MALOLEJ,
	ALL_BSENSOR
}
T_BrikState;
//Text for disply
char state_text[][20] = {
	"Briket:  OFF",
	"Briket:  ON",
	"Briket:  PRACUJE",
	"ERR Bri TEPLOT OLEJ",
	"ERR Bri MALO PILIN",
	"ERR Bri VRECO PLNE",
	"ERR Bri FILTER OLEJ",
	"ERR Bri MALO OLEJA",
	"ERR Bri SENSOR PORU"
//   12345678901234567890
};

typedef enum {
	SI_OFF,
	SI_ON,
	PREFUK,
	ALL_SMALO_PILIN,
	ALL_SVELA_PILIN,
	ALL_SSENSOR
}
T_SiloState;
char Sil_state_text[][20] = {
	"Silo:    OFF",
	"Silo:    ON",
	"Silo:    PREFUK",
	"ERR Silo MALO PILIN",
	"ERR Silo VELA PILIN",
	"ERR Silo SENSOR"
//   12345678901234567890
};
char InpText[][7] = {
	"B__S_X",
	"B__S__",
	"B__SMX",
	"B__SM_",
	"B_XS_X",
	"B_XS__",
	"B_XSMX",
	"B_XSM_",
	"BM_S_X",
	"BM_S__",
	"BM_SMX",
	"BM_SM_",
	"BMXS_X",
	"BMXS__",
	"BMXSMX",
	"BMXSM_"
};



DS18B20 oilTemp(oneWirePin);
DateTime myDateTime;

RTC_DS1307 rtc;
LiquidCrystal_I2C lcd(0x27, 20, 4);

bool oilTempSenOK = FALSE;
bool chladIsOn = FALSE;
bool odlStartIsOn = FALSE;
int lastBrikState = 255;
int lastSilState = 255;
float brik_oil_temp = 0;
uint32_t brik_work_hour = 0;
static T_BrikState Brik_State = BROFF;
static T_SiloState Silo_Stae = SI_OFF;

#endif // ! _Globals.h
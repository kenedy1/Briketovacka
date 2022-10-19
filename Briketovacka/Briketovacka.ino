
/*
 Name:		Briketovacka.ino
 Created:	9/18/2022 5:48:11 PM
 Author:	Marian Murín
*/



#include <JC_Button.h>
#include <StopwatchLib.h>
#include <DebounceFilterLib.h>
#include <OneWire.h>
#include <SerialCommands.h>
#include <TimedAction.h>
#include <DS18B20.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "Commands.h"
#include "Globals.h"
#include <RTClib.h>
#include "IO_Pins.h"

//--------------------------------------
//Diplay actual time from RTC on LCD
void printTime(uint8_t row) {
	byte BM = 0;
	byte BX = 0;
	byte SM = 0;
	byte SX = 0;
	byte _inpState = 0;

	BM = iBriketMin.isReleased();
	BX = iBriketMax.isReleased();
	SM = iSiloMin.isReleased();
	SX = iSiloMax.isReleased();
	_inpState = SX | (SM << 1) | (BX << 2) | (BM << 3);
	lcd.setCursor(12, 4);
	if (_inpState < 16) lcd.print(InpText[_inpState]);



	if (rtc.isrunning()) { //RTC is OK
		myDateTime = rtc.now();
		lcd.setCursor(0, row);
		lcd.print("Time: ");
		lcd.print(myDateTime.hour(), DEC);
		lcd.print(":");
		lcd.print(myDateTime.minute(), DEC);
		
		Serial.print(F("Time "));
		printDigits(myDateTime.hour());
		//Serial.print(F(":"));
		printDigits(myDateTime.minute());
		Serial.println(" ");
	}
	else 
	{   //No RTC
		lcd.setCursor(0, row);
		lcd.print("Err no RTC");
		Serial.println(F("ERROR NO RTC found"));
	}
}

//--------------------------------------
//print state of state machine
void printState(T_BrikState _state, T_SiloState _sil_state) {
	if (lastBrikState != _state) {
		Serial.println(state_text[int8_t(_state)]);
		lcd.setCursor(0, 1);
		lcd.print("                    ");
		lcd.setCursor(0, 1);
		lcd.print(state_text[int8_t(_state)]);
	}
	lastBrikState = _state;
	if (lastSilState != _sil_state) {
		Serial.println(Sil_state_text[int8_t(_sil_state)]);
		lcd.setCursor(0, 2);
		lcd.print("                    ");
		lcd.setCursor(0, 2);
		lcd.print(Sil_state_text[int8_t(_sil_state)]);
	}
	lastSilState = _sil_state;
}
//--------------------------------------
//Print leading 0 in time
void printDigits(int digits) {
	// utility function for digital clock display: prints preceding colon and leading 0
	Serial.print(":");
	if (digits < 10)
		Serial.print('0');
	Serial.print(digits);
}


void printTimeAct()
{
printTime(0);

Serial.print("BrStates> ");
Serial.print(Brik_State);
Serial.print("   silo>");
Serial.println(Silo_Stae);
}
//--------------------------------------
// Get oil temp from onewire 
void getOilTemp() {
	if (oilTemp.getNumberOfDevices() == 1) {
		oilTemp.doConversion();
		brik_oil_temp = oilTemp.getTempC();
		lcd.setCursor(0, 3);
		lcd.print("            ");
		lcd.setCursor(0, 3);
		lcd.print("Oil temp:");
		lcd.print((int)brik_oil_temp, DEC);
		Serial.print("Oil Temp>");
		Serial.println(brik_oil_temp);
	}
	else {
		lcd.setCursor(0, 3);
		lcd.print("NO TEMP Sensor");
		Serial.println(F("ERROR No oil temperature sensor found."));
	}
	
}
//--------------------------------------
//Timed actions
TimedAction oilTemtAction = TimedAction(5000, getOilTemp);
TimedAction readInputsAction = TimedAction(100, read_all_inputs);
TimedAction prinTimeAction = TimedAction(5000, printTimeAct);
//--------------------------------------
// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(115200);
	Wire.setClock(100000);
	delay(200);
	Serial.flush();
	Serial.println(F("Autor: Marian Murin"));
	Serial.println(F("BUILD PATH:"));
	Serial.println(F(__FILE__));
	Serial.println(F("BUILD DATE:"));
	Serial.println(F(__DATE__));
	Serial.println(F("BUILD TIME:"));
	Serial.println(F(__TIME__));
	Serial.println(F("____Start____"));
	Serial.println(F("Set IO pins"));
	setupIOpins();
	delay(2000);
	if (!rtc.begin()) Serial.println("Couldn't find RTC");

	int dsCount = oilTemp.getNumberOfDevices();
	oilTemp.selectNext();
	uint8_t  dsaddr[8];
	oilTemp.getAddress(dsaddr);

	lcd.begin(20, 4);
	lcd.init();      // initialize the lcd
	lcd.clear();
	lcd.backlight();
	lcd.setCursor(6, 0);
	lcd.print("START");
	lcd.setCursor(0, 1);
	if (dsCount == 0) {
		oilTempSenOK = 1;
		lcd.print("No oil Temp sens");
		Serial.println("No oil Temp sens");
	}
	if (dsCount == 1) {
		oilTempSenOK = 1;
		oilTemp.doConversion();
		Serial.println("Oil temp sen found");
		lcd.print("Temp sen OK");
		lcd.setCursor(0, 2);
		for (uint8_t i = 0; i < 8; i++) {
			lcd.print(dsaddr[i]);
			Serial.print(" ");
			Serial.print(dsaddr[i]);
		}
		Serial.println("Adress of DS18b20");
		Serial.print("Temperature: ");
		Serial.print(oilTemp.getTempC());
		Serial.print(" C / ");
	}
	_delay_ms(1000);
	lcd.clear();
	printTime(0);
	Serial.println("Init finished OK");

	oilTemtAction.enable();
	readInputsAction.enable();

	myCommands.SetDefaultHandler(&cmd_unrecognized);
	myCommands.AddCommand(&scmd_set_hour);
	myCommands.AddCommand(&scmd_set_minuts);
	myCommands.AddCommand(&scmd_set_year);
	myCommands.AddCommand(&scmd_set_mount);
	myCommands.AddCommand(&scmd_set_day);
	myCommands.AddCommand(&scmd_set_whour);
	myCommands.AddCommand(&scmd_info);
	myCommands.AddCommand(&scmd_help);
}
//--------------------------------------
//Check cooling fan and temp for valve.
void checkChladenie() {
	//Control coolin fan
	if (!chladIsOn) 
	{
		if (brik_oil_temp > TEMP_CHLAD_ON) 
		{
			digitalWrite(doChladenie, 0);
			chladIsOn = TRUE;
		}
	}
	else
	{
		if (brik_oil_temp < TEMP_CHLAD_OFF) 
		{
			digitalWrite(doChladenie, 1);
			chladIsOn = FALSE;
		}	
	}
	//Control odlahceny start valve
	if (!odlStartIsOn) 
	{
		if (brik_oil_temp < TEMP_OIL_LOW) 
		{
			digitalWrite(doOdlahSTART, 0);
			odlStartIsOn = TRUE;
		}
	}
	else
	{
		if (brik_oil_temp >  (TEMP_OIL_LOW + 3)) 
		{
			digitalWrite(doOdlahSTART, 1);
			odlStartIsOn = FALSE;
		}	
	}
		
	
}
//--------------------------------------
// the loop function runs over and over again until power down or reset
void loop()
{
	myCommands.ReadSerial();
	
	bool ON_presed = 0;
	while (1)
	{
		myCommands.ReadSerial();
		readInputsAction.check();
		oilTemtAction.check();
		prinTimeAction.check();
		myCommands.ReadSerial();
		printState(Brik_State, Silo_Stae);
		//Briketovacka state machine
		
		switch (Brik_State)
		{
		case BROFF:  //Briketovacka off
			Silo_Stae = SI_OFF;
			do_all_off();
			if (iBriketOn.releasedFor(1000)) {
				ON_presed = 1;
				break;
			}
			if (ON_presed && iBriketOn.pressedFor(200)) {
				ON_presed = 0;
				Brik_State = BRON;
				Silo_Stae = SI_ON;
			}
			break;

		case BRON:	//Briketovacka ON
			checkChladenie();
			if (iBriketOn.releasedFor(1000)) ON_presed = 1;

			if (iBriketOn.pressedFor(200) && ON_presed)
			{
				ON_presed = 0;
				Brik_State = BROFF;
				break;
			}

			if (iOlejLow.pressedFor(10000)) 
			{
				Brik_State = ALL_MALOLEJ;
			}

			if (iBriketMax.releasedFor(3000) && iBriketMin.pressedFor(3000))
				Brik_State = ALL_BSENSOR;

			if (iBriketMin.pressedFor(5000) && iSiloMin.pressedFor(5000)) 
			{
				Brik_State = ALL_MALO_PILIN;
				break;
			}

			if (iTlakFiltra.pressedFor(10000))
			{
				Brik_State = ALL_FILTER;
				break;
			}

			if (brik_oil_temp > TEMP_OIL_BRIK_MAX)
			{
				Brik_State = ALL_TEPL_OLEJ;
			}
			digitalWrite(doAlarm, 1);
			Brik_State = BRONGO;
			break;

		case BRONGO: //Briketovacka ON and GO 2
			checkChladenie();
			if (iBriketOn.releasedFor(1000)) ON_presed = 1;

			if (iBriketOn.pressedFor(200) && ON_presed)
			{
				ON_presed = 0;
				Brik_State = BROFF;
				break;
			}
			if (iBriketMax.releasedFor(3000) && iBriketMin.pressedFor(3000))
				Brik_State = ALL_BSENSOR;

			if (iOlejLow.pressedFor(10000))
			{
				Brik_State = ALL_MALOLEJ;
			}

			if (iBriketMin.pressedFor(5000) && iSiloMin.pressedFor(5000))
			{
				Brik_State = ALL_MALO_PILIN;
				break;
			}

			if (iTlakFiltra.pressedFor(10000))
			{
				Brik_State = ALL_FILTER;
				break;
			}

			if (brik_oil_temp > TEMP_OIL_BRIK_MAX)
			{
				Brik_State = ALL_TEPL_OLEJ;
				break;
			}
			if (iPlneVreco.pressedFor(5000))
			{
				Brik_State = ALL_VRECO_PLNE;
				break;
			}
			digitalWrite(doBriketON, 0);
			digitalWrite(doAlarm, 1);
			break;

		case ALL_TEPL_OLEJ:  //err teplota oleja
			if (iBriketOn.releasedFor(1000)) ON_presed = 1;

			if (iBriketOn.pressedFor(2000) && ON_presed)
			{
				ON_presed = 0;
				Brik_State = BROFF;
				break;
			}
			checkChladenie();
			digitalWrite(doAlarm, 0);
			digitalWrite(doBriketON, 1);
			if (brik_oil_temp < TEMP_CHLAD_ON) Brik_State = BRONGO;

			break;
		case ALL_MALO_PILIN:  //err malo piln
			checkChladenie();
			digitalWrite(doAlarm, 0);
			digitalWrite(doBriketON, 1);
			if (iBriketOn.releasedFor(1000)) ON_presed = 1;

			if (iBriketOn.pressedFor(2000) && ON_presed)
			{
				ON_presed = 0;
				Brik_State = BROFF;
				break;
			}

			if (iBriketMin.releasedFor(20000)) Brik_State = BRONGO;

			break;
		case ALL_VRECO_PLNE: //err plne vreco
			checkChladenie();
			digitalWrite(doAlarm, 0);
			digitalWrite(doBriketON, 1);
			if (iBriketOn.releasedFor(1000)) ON_presed = 1;

			if (iBriketOn.pressedFor(2000) && ON_presed)
			{
				ON_presed = 0;
				Brik_State = BROFF;
				break;
			}

			if (ON_presed && iBriketOn.pressedFor(100)  && iPlneVreco.releasedFor(1000))
			{
				digitalWrite(doAlarm, 1);
				ON_presed = 0;
				Brik_State = BRONGO;
				break;
			}
			break;

		case ALL_FILTER: //err filter oleja

			if (iBriketOn.releasedFor(1000)) ON_presed = 1;
		

			if (iBriketOn.pressedFor(2000) && ON_presed)
			{
				ON_presed = 0;
				Brik_State = BROFF;
				break;
			}

			digitalWrite(doAlarm, 0);
			digitalWrite(doBriketON, 1);
			break;

		case ALL_MALOLEJ: //err malo oleja
			if (iBriketOn.releasedFor(1000)) ON_presed = 1;

			if (iBriketOn.pressedFor(2000) && ON_presed)
			{
				ON_presed = 0;
				Brik_State = BROFF;
				break;
			}

			digitalWrite(doAlarm, 0);
			digitalWrite(doBriketON, 1);
			break;
		case ALL_BSENSOR:
			if (iBriketOn.releasedFor(1000)) ON_presed = 1;

			if (iBriketOn.pressedFor(2000) && ON_presed)
			{
				ON_presed = 0;
				Brik_State = BROFF;
				break;
			}

			digitalWrite(doAlarm, 0);
			digitalWrite(doBriketON, 1);
			break;
			break;
		default:
			break;
		}
		//silo state machine
		switch (Silo_Stae) 
		{
		case SI_OFF: //silo OFF
			digitalWrite(doPrefukON, 1);
			break;
		case SI_ON: //silo ON
			if (iSiloMax.releasedFor(3000) && iSiloMin.pressedFor(3000)) Silo_Stae = ALL_SSENSOR;

			if (iBriketMin.pressedFor(10000) && iSiloMin.releasedFor(10000)) Silo_Stae = PREFUK;
			if (iBriketMax.releasedFor(10000) && iSiloMax.releasedFor(10000)) Silo_Stae = ALL_SVELA_PILIN;
			digitalWrite(doPrefukON, 1); 
			break;

		case PREFUK: //silo prefuk ON
			if (iSiloMax.releasedFor(3000) && iSiloMin.pressedFor(3000)) Silo_Stae = ALL_SSENSOR;
			if (iBriketMax.releasedFor(10000) || iSiloMin.pressedFor(10000)) Silo_Stae = SI_ON;
			if (iBriketMin.pressedFor(10000) && iSiloMin.pressedFor(10000)) Silo_Stae = ALL_SMALO_PILIN;
			if (iBriketMax.releasedFor(10000) && iSiloMax.releasedFor(10000)) Silo_Stae = ALL_SVELA_PILIN;
			digitalWrite(doPrefukON, 0);
			break;

		case ALL_SMALO_PILIN: //err malo pilin
			
			if (iSiloMin.releasedFor(100000)) {
				digitalWrite(doAlarm, 1);
				Silo_Stae = SI_ON;
			}
			digitalWrite(doPrefukON, 1);
			digitalWrite(doAlarm, 0);
			break;

		case ALL_SVELA_PILIN: //err plne silo
			
			if (iBriketMin.pressedFor(10000)) {
				digitalWrite(doAlarm, 1);
				Silo_Stae = PREFUK;
			}
			digitalWrite(doPrefukON, 1);
			digitalWrite(doAlarm, 0);
			break;

		case ALL_SSENSOR:
			digitalWrite(doPrefukON, 1);
			digitalWrite(doAlarm, 0);
			
			break;

		default:
			break;

		}
	}
}

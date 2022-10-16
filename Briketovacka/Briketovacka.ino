
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
	
	if (rtc.isrunning()) { //RTC is OK
		myDateTime = rtc.now();
		lcd.setCursor(0, row);
		lcd.print(myDateTime.hour(), DEC);
		lcd.print(":");
		lcd.print(myDateTime.minute(), DEC);
		Serial.print(F("Time >"));
		printDigits(myDateTime.hour());
		Serial.print(F(":"));
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
//--------------------------------------
// Get oil temp from onewire 
void getOilTemp() {
	if (oilTemp.getNumberOfDevices() == 1) {
		oilTemp.doConversion();
		brik_oil_temp = oilTemp.getTempC();
		lcd.setCursor(0, 3);
		lcd.print(brik_oil_temp, DEC);
		Serial.print("Oil Temp>");
		Serial.println(brik_oil_temp);
	}
	else {
		lcd.setCursor(0, 3);
		lcd.print("NO TEMP");
		Serial.println(F("ERROR No oil temperature sensor found."));
	}
	printTime(0);
}
//--------------------------------------
//Timed actions
TimedAction oilTemtAction = TimedAction(1000, getOilTemp);
TimedAction readInputsAction = TimedAction(50, read_all_inputs);
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
		lcd.print("Oil temp sen OK");
		lcd.setCursor(0, 2);
		for (uint8_t i = 0; i < 8; i++) {
			lcd.print(" ");
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

	myCommands.SetDefaultHandler(cmd_unrecognized);
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
//Check cooling fan and temp.
void checkChladenie() {
	if (!digitalRead(doChladenie)) {
		if (brik_oil_temp > TEMP_CHLAD_ON) digitalWrite(doChladenie, 1);
	}
	else if (brik_oil_temp > TEMP_CHLAD_OFF) digitalWrite(doChladenie, 0);
	if (brik_oil_temp < TEMP_OIL_LOW) digitalWrite(doOdlahSTART, 1);
	else digitalWrite(doOdlahSTART, 0);
}
//--------------------------------------
// the loop function runs over and over again until power down or reset
void loop()
{
	myCommands.ReadSerial();
	static T_BrikState Brik_State = BROFF;
	static T_SiloState Silo_Stae = SI_OFF;
	bool ON_presed = 0;
	while (1)
	{
		myCommands.ReadSerial();
		readInputsAction.check();
		oilTemtAction.check();
		myCommands.ReadSerial();
		printState(Brik_State, Silo_Stae);
		//Briketovacka state machine
		Serial.print("BrStates> ");
		Serial.print(Brik_State);
		Serial.print("   silo>");
		Serial.println(Silo_Stae);
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

			if (iBriketMin.pressedFor(20000) && iSiloMin.pressedFor(20000)) 
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

			if (iOlejLow.pressedFor(10000))
			{
				Brik_State = ALL_MALOLEJ;
			}

			if (iBriketMin.pressedFor(20000) && iSiloMin.pressedFor(20000))
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
			digitalWrite(doPrefukON, 1);
			if (iBriketMin.pressedFor(10000) && iSiloMin.releasedFor(10000)) Silo_Stae = PREFUK;
			if (iBriketMax.pressedFor(2000) && iSiloMax.pressedFor(2000)) Silo_Stae = ALL_SVELA_PILIN;
			break;

		case PREFUK: //silo prefuk ON
			digitalWrite(doPrefukON, 0);
			if (iBriketMax.pressedFor(5000) || iSiloMin.pressedFor(5000))Silo_Stae = SI_ON;
			if (iBriketMin.pressedFor(10000) && iSiloMin.pressedFor(10000)) Silo_Stae = ALL_SMALO_PILIN;
			if (iBriketMax.pressedFor(2000) && iSiloMax.releasedFor(5000)) Silo_Stae = ALL_SVELA_PILIN;
			break;

		case ALL_SMALO_PILIN: //err malo pilin
			digitalWrite(doPrefukON, 1);
			digitalWrite(doAlarm, 0);
			if (iSiloMin.releasedFor(100000)) {
				digitalWrite(doAlarm, 1);
				Silo_Stae = SI_ON;
			}
			break;

		case ALL_SVELA_PILIN: //err plne silo
			digitalWrite(doPrefukON, 1);
			digitalWrite(doAlarm, 0);
			if (iBriketMin.pressedFor(10000)) {
				digitalWrite(doAlarm, 1);
				Silo_Stae = PREFUK;
			}
			break;

		default:
			break;

		}
	}
}

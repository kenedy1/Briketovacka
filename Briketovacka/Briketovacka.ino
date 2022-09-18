/*
 Name:		Briketovacka.ino
 Created:	9/18/2022 5:48:11 PM
 Author:	Majo
*/



#include <OneWire.h>
#include <SerialCommands.h>
#include <TimedAction.h>
#include <DS18B20.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include <RTClib.h>
#include "IO_Pins.h"
char ser_comm_buf[32];
SerialCommands myCommands(&Serial, ser_comm_buf, sizeof(ser_comm_buf), "\r\n", " ");
DS18B20 oilTemp(oneWirePin);
DateTime myDateTime;
RTC_DS1307 rtc;
LiquidCrystal_I2C lcd(0x27, 20, 4);
bool oilTempOK = 0;


void printTime(uint8_t row) {
	myDateTime = rtc.now();
	lcd.setCursor(0, row);
	lcd.print(myDateTime.hour());
	lcd.print(":");
	lcd.print(myDateTime.minute());

}

// the setup function runs once when you press reset or power the board
void setup() {
	setupIOpins();
	rtc.begin();
	int dsCount = oilTemp.getNumberOfDevices();
	oilTemp.selectNext();
	uint8_t  dsaddr[8];
	oilTemp.getAddress(dsaddr);

	Serial.begin(115200);
	Serial.println("____Start____");
	lcd.init();                      // initialize the lcd 
	lcd.backlight();
	lcd.setCursor(6, 0);
	lcd.print("START");
	lcd.setCursor(0, 1);
	if (dsCount == 0){
		lcd.print("No oil Temp sens");
		Serial.println("No oil Temp sens");
	}
		
	if (dsCount == 1) {
		oilTempOK = 1;
		Serial.println("Oil temp sen found");
		lcd.print("Oil temp sen found");
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
}


// the loop function runs over and over again until power down or reset
void loop() {
	myCommands.ReadSerial();


  
}

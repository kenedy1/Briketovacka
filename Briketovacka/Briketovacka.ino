/*
 Name:		Briketovacka.ino
 Created:	9/18/2022 5:48:11 PM
 Author:	Majo
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <USI_TWI_Master.h>
#include <TinyWireM.h>
#include <TinyRTClib.h>

DateTime myDateTime;
RTC_DS1307 rtc;
LiquidCrystal_I2C lcd(0x27, 20, 4);
// the setup function runs once when you press reset or power the board
void setup() {
	rtc.begin();
	Serial.begin(115200);
	Serial.println("____Start____");
	lcd.init();                      // initialize the lcd 
	lcd.backlight();
	lcd.setCursor(6, 0);
	lcd.print("START");
	_delay_ms(500);
	myDateTime = rtc.now();
	

}

void printDateTime(uint8_t row) {
	myDateTime = rtc.now();
	lcd.setCursor(0, row);
	lcd.print(myDateTime.hour());
	lcd.print(":");
	lcd.print(myDateTime.minute());
}


// the loop function runs over and over again until power down or reset
void loop() {
  
}

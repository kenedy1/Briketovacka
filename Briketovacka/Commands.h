#ifndef _Commands_h_
#define _Commands_h_
#include "Globals.h"
#include <RTClib.h>

void cmd_unrecognized(SerialCommands* sender, const char* cmd)
{
	sender->GetSerial()->print(F("Unrecognized command ["));
	sender->GetSerial()->print(cmd);
	sender->GetSerial()->println("]");
	sender->GetSerial()->println(F("Press ? for help."));
}

void cmd_help(SerialCommands* sender)
{
	sender->GetSerial()->println(F("h        -print help"));
	sender->GetSerial()->println(F("H hour   -set time hour"));
	sender->GetSerial()->println(F("M minuts -set time minuts"));
	sender->GetSerial()->println(F("Y year   -set date year"));
	sender->GetSerial()->println(F("T mount  -set time mount"));
	sender->GetSerial()->println(F("A day    -set time day"));
	sender->GetSerial()->println(F("W hour   -set working time hour"));
	sender->GetSerial()->println(F("I        -print info hour"));
}

void printActTime(SerialCommands* sender) {
	DateTime _t_now = rtc.now();

	sender->GetSerial()->print(F("Current Date / Time: "));
	sender->GetSerial()->print(_t_now.day());
	sender->GetSerial()->print("/");
	sender->GetSerial()->print(_t_now.month());
	sender->GetSerial()->print("/");
	sender->GetSerial()->print(_t_now.year());
	sender->GetSerial()->print("  ");
	sender->GetSerial()->print(_t_now.hour());
	sender->GetSerial()->print(":");
	sender->GetSerial()->print(_t_now.minute());
	sender->GetSerial()->print(":");
	sender->GetSerial()->println(_t_now.second());
}


void cmd_set_hour(SerialCommands* sender)
{
	char* _hour_str = sender->Next();
	if (_hour_str == NULL)
	{
		sender->GetSerial()->println(F("ERROR no hour"));
		return;
	}
	int hour = atoi(_hour_str);
	if (hour > 23 || hour < 0) {
		sender->GetSerial()->println(F("ERROR not in range 0 - 23"));
		return;
	}
	DateTime _t_now = rtc.now();
	rtc.adjust(DateTime(_t_now.year(), _t_now.month(), _t_now.day(), hour, _t_now.minute(), 0));
	printActTime(sender);
}
void cmd_set_minuts(SerialCommands* sender)
{
	char* _minut_str = sender->Next();
	if (_minut_str == NULL)
	{
		sender->GetSerial()->println(F("ERROR no minut"));
		return;
	}
	int minut = atoi(_minut_str);
	if (minut > 59 || minut < 0) {
		sender->GetSerial()->println(F("ERROR not in range 0 - 59"));
		return;
	}
	DateTime _t_now = rtc.now();
	rtc.adjust(DateTime(_t_now.year(), _t_now.month(), _t_now.day(), _t_now.hour(), minut, 0));
	printActTime(sender);
}
void cmd_set_year(SerialCommands* sender)
{
	char* _year_str = sender->Next();
	if (_year_str == NULL)
	{
		sender->GetSerial()->println(F("ERROR no year"));
		return;
	}
	int year = atoi(_year_str);
	if (year > 2099 || year < 2000) {
		sender->GetSerial()->println(F("ERROR not in range 2000 - 2099"));
		return;
	}
	DateTime _t_now = rtc.now();
	rtc.adjust(DateTime(year, _t_now.month(), _t_now.day(), _t_now.hour(), _t_now.minute(), 0));
	_t_now = rtc.now();
	printActTime(sender);
}
void cmd_set_mount(SerialCommands* sender)
{
	char* _mount_str = sender->Next();
	if (_mount_str == NULL)
	{
		sender->GetSerial()->println(F("ERROR no mount"));
		return;
	}
	int mounth = atoi(_mount_str);
	if (mounth > 12 || mounth < 1) {
		sender->GetSerial()->println(F("ERROR not in range 1 - 12"));
		return;
	}
	DateTime _t_now = rtc.now();
	rtc.adjust(DateTime(_t_now.year(), mounth, _t_now.day(), _t_now.hour(), _t_now.minute(), 0));
	_t_now = rtc.now();
	printActTime(sender);
}
void cmd_set_day(SerialCommands* sender)
{
	char* _day_str = sender->Next();
	if (_day_str == NULL)
	{
		sender->GetSerial()->println(F("ERROR no day"));
		return;
	}
	int day = atoi(_day_str);
	if (day > 31 || day < 1) {
		sender->GetSerial()->println(F("ERROR not in range 1 - 31"));
		return;
	}
	DateTime _t_now = rtc.now();
	rtc.adjust(DateTime(_t_now.year(), _t_now.month(), day, _t_now.hour(), _t_now.minute(), 0));
	_t_now = rtc.now();
	printActTime(sender);
}

void cmd_set_whour(SerialCommands* sender)
{
	char* _whour_str = sender->Next();
	if (_whour_str == NULL)
	{
		sender->GetSerial()->println(F("ERROR no work hour"));
		return;
	}
	int _whour = atoi(_whour_str);
	if (_whour > 10000 || _whour < 0) {
		sender->GetSerial()->println(F("ERROR not in range 0 - 10000"));
		return;
	}
	brik_work_hour = _whour;
	sender->GetSerial()->print("Work hours >");
	sender->GetSerial()->println(_whour);

}
void cmd_print_info(SerialCommands* sender)
{
	printActTime(sender);
	sender->GetSerial()->print("Work hours >");
	sender->GetSerial()->println();

}



#endif // !_Commands_h_


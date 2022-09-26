#ifndef _IO_Pins_h_
#define _IO_Pins_h_

#include <JC_Button.h>
#include <StopwatchLib.h>
#include <DebounceFilterLib.h>
//temperature for allarms an cooling
#define   TEMP_CHLAD_ON      48
#define   TEMP_CHLAD_OFF     40
#define   TEMP_OIL_LOW       20
#define   TEMP_OIL_BRIK_MAX  60
// digitalne vstupy
#define   diTlakFilter	22
#define   diSiloMax		23
#define   diSiloMin		24
#define   diBriketMax	26
#define   diBriketMin	25
#define   diOlejLow		27
#define   diBriketON	28
#define   diPlneVreco	29

// digitalne vystupy
#define   doPrefukON	31
#define   doBriketON	32
#define   doOdlahSTART	33
#define   doChladenie	34
#define   doAlarm		35

// one wire pin
#define   oneWirePin    40
//setings for imputs
//					input pin, time debounc,pullup,invert						
Button iTlakFiltra(diTlakFilter, 10000, false, false);
Button iSiloMax(diSiloMax, 10000, false, false);
Button iSiloMin(diSiloMin, 10000, false, false);
Button iBriketMax(diBriketMax, 10000, false, false);
Button iBriketMin(diBriketMin, 10000, false, false);
Button iOlejLow(diOlejLow, 10000, false, false);
Button iBriketOn(diBriketON, 500, false, true);
Button iPlneVreco(diPlneVreco, 5000, false, true);
//setup all inputs
void setupIOpins() {
	//set inputs pins
	pinMode(diTlakFilter, INPUT);
	pinMode(diSiloMax, INPUT);
	pinMode(diSiloMin, INPUT);
	pinMode(diBriketMax, INPUT);
	pinMode(diBriketMin, INPUT);
	pinMode(diOlejLow, INPUT);
	pinMode(diBriketON, INPUT);
	pinMode(diPlneVreco, INPUT);
	iTlakFiltra.begin();
	iSiloMax.begin();
	iSiloMin.begin();
	iBriketMax.begin();
	iBriketMin.begin();
	iOlejLow.begin();
	iBriketOn.begin();
	iPlneVreco.begin();
	//set output pins
	pinMode(doPrefukON, OUTPUT);
	pinMode(doBriketON, OUTPUT);
	pinMode(doOdlahSTART, OUTPUT);
	pinMode(doChladenie, OUTPUT);
	pinMode(doAlarm, OUTPUT);
}
// read state of all input
void read_all_inputs() {
	iTlakFiltra.read();
	iSiloMax.read();
	iSiloMin.read();
	iBriketMax.read();
	iBriketMin.read();
	iOlejLow.read();
	iBriketOn.read();
	iPlneVreco.read();

}
//all outputs to off state
void do_all_off() {
	digitalWrite(doBriketON, 0);
	digitalWrite(doChladenie, 0);
	digitalWrite(doOdlahSTART, 0);
	digitalWrite(doPrefukON, 0);
	digitalWrite(doAlarm, 0);
}

#endif

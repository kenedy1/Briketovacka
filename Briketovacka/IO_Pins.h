#pragma once

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

// one wire
#define   oneWirePin    40

void setupIOpins() {
	//set inputs pins
	pinMode(diTlakFilter,INPUT);
	pinMode(diSiloMax,INPUT);
	pinMode(diSiloMin,INPUT);
	pinMode(diBriketMax, INPUT);
	pinMode(diBriketMin, INPUT);
	pinMode(diOlejLow, INPUT);
	pinMode(diBriketON, INPUT);
	pinMode(diPlneVreco, INPUT);

	//set output pins
	pinMode(doPrefukON, OUTPUT);
	pinMode(doBriketON, OUTPUT);
	pinMode(doOdlahSTART, OUTPUT);
	pinMode(doChladenie, OUTPUT);
	pinMode(doAlarm, OUTPUT);
}

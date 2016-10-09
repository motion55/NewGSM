#ifndef _NEWGSM_H
#define _NEWGSM_H

#include <SoftwareSerial.h>
#include "GSM.h"

class NEWGSM : public virtual GSM {

private:
     int configandwait(char* pin);
     int setPIN(char *pin);
     int changeNSIPmode(char);
public:
     NEWGSM();
     ~NEWGSM();
	 static NEWGSM& getInstance() {
		 static NEWGSM _gsm; return _gsm;
	 };
	 int getCCI(char* cci);
     int getIMEI(char* imei);
     int sendSMS(const char* to, const char* msg);
	 int readCellData(int &mcc, int &mnc, long &lac, long &cellid);
	 boolean readSMS(char* msg, int msglength, char* number, int nlength);
     boolean readCall(char* number, int nlength);
     boolean call(char* number, unsigned int milliseconds);
     char forceON();
	 virtual int read(char* result, int resultlength);
};

//extern NEWGSM gsm;

#endif


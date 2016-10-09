/*
ComPort.h
*/

#ifndef _ComPort_h_
#define _ComPort_h_

#include <inttypes.h>
#include <Stream.h>
#include <SoftwareSerial.h>
#include "WideTextFinder.h"

#define _COMPORT_SS_RXPIN_	2
#define _COMPORT_SS_TXPIN_	3


/******************************************************************************
* Definitions
******************************************************************************/

class ComPort : public Stream
{
private:
	SoftwareSerial* _SW_Serial;
	HardwareSerial* _HW_Serial;

public:
  // public methods
  ComPort();
  ~ComPort();

  void SelectHardwareSerial(HardwareSerial *HW_Serial);
  void SelectSoftwareSerial(uint8_t receivePin, uint8_t transmitPin);
  int peek();
  void begin(long speed);
  virtual size_t write(uint8_t dat);
  virtual int read();
  virtual int available();
  virtual void flush();
};

#endif

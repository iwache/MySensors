/*
  Encoder.h - Firmata library
  Copyright (C) 2013 Norbert Truchsess. All rights reserved.
  Copyright (C) 2014 Nicolas Panel. All rights reserved.
  Copyright (C) 2015 Jeff Hoefs. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  See file LICENSE.txt for further informations on licensing terms.

  Provide encoder feature based on PJRC implementation.
  See http://www.pjrc.com/teensy/td_libs_Encoder.html for more informations

  FirmataEncoder handles instructions and is able to automatically report positions.
  See protocol details in "examples/SimpleFirmataEncoder/SimpleFirmataEncoder.ino"
*/

#ifndef Encoder_h
#define Encoder_h

#include "../../MyFirmataClass.h"
#include "../../MyFirmataFeature.h"

#define MAX_ENCODERS                5 // arbitrary value, may need to adjust
#define ENCODER_ATTACH              (0x00)
#define ENCODER_REPORT_POSITION     (0x01)
#define ENCODER_REPORT_POSITIONS    (0x02)
#define ENCODER_RESET_POSITION      (0x03)
#define ENCODER_REPORT_AUTO         (0x04)
#define ENCODER_DETACH              (0x05)

class Encoder :public MyFirmataFeature
{
public:
	Encoder();
	Encoder(uint8_t pin1, uint8_t pin2);
	//~Encoder(); => never destroy in practice

	// MyFirmataFeature implementation
	void handleCapability(byte pin);
	void setPinMode(byte pin, int mode);
	boolean handleSysex(byte command, byte argc, byte *argv);
	void update();

	// real Encoder methods implementation
	int32_t read();
	void write(int32_t position);

private:
	byte _encoderNum = 255;
	byte _pinANum;
	byte _pinBNum;
	int32_t _position;

	// wait for Firmata connection established and then attach this instance
	bool _attachRequested; 

	void attachEncoder();
	void detachEncoder();
	void reportPosition();
	void reportPositions();
	void resetPosition();
	void toggleAutoReport(byte report);
	boolean isEncoderAttached(byte encoderNum);
	bool isReportingEnabled();
	bool isAttached(byte encoderNum);
};

#endif

/*
  Encoder.cpp - Firmata library v0.1.0 - 2015-11-22
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
*/

//#include <MyFirmataClass.h>
#include "Encoder.h"
#include <string.h>
#include "../../MyFirmataClass.h"

static bool encoders[MAX_ENCODERS];
//static int32_t positions[MAX_ENCODERS];
static byte autoReport = 0x02;

/* Constructors */
Encoder::Encoder()
{
}

Encoder::Encoder(uint8_t pin1, uint8_t pin2)
{
	static byte _instanceCount;

	_pinANum = pin1;
	_pinBNum = pin2;
	_encoderNum = _instanceCount++;
	_attachRequested = true;

	MyFirmata.addFeature(*this);
}

int32_t Encoder::read()
{
	reportPosition();
	return _position;
}

void Encoder::write(int32_t position)
{
	resetPosition();
	reportPosition();
}

void Encoder::setPinMode(byte pin, int mode)
{
	MyFirmata.setPinMode(pin, mode);
}

void Encoder::handleCapability(byte pin)
{
  //if (IS_PIN_INTERRUPT(pin)) {
  //  MyFirmata.write(PIN_MODE_ENCODER);
  //  MyFirmata.write(28); //28 bits used for absolute position
  //}
}


/* Handle ENCODER_DATA (0x61) sysex commands
 * See protocol details in "examples/SimpleFirmataEncoder/SimpleFirmataEncoder.ino"
*/
boolean Encoder::handleSysex(byte command, byte argc, byte *argv)
{
	if (command == ENCODER_DATA)
	{
		byte encoderNum;
		int32_t position;
		byte direction;

		if (argc != 5)
			return false;

		encoderNum = argv[0] & 0x2F;
		if (_encoderNum != encoderNum)
			return false;

		direction = (argv[0] >> 6) & 0x01;
		position = (argv[4] << 7) | argv[3];
		position = (position << 7) | argv[2];
		position = (position << 7) | argv[1];
		_position = direction ? -position : position;

		return true;
	}
	return false;
}

void Encoder::update()
{
	if (_attachRequested) {

		attachEncoder();
		reportPosition();

		_attachRequested = false;
	}
}

void Encoder::attachEncoder()
{
	byte buffer[7];

	buffer[0] = (START_SYSEX);
	buffer[1] = (ENCODER_DATA);
	buffer[2] = (ENCODER_ATTACH);
	buffer[3] = (_encoderNum);
	buffer[4] = (_pinANum);
	buffer[5] = (_pinBNum);
	buffer[6] = (END_SYSEX);
	MyFirmata.write(buffer, 7);
}

void Encoder::detachEncoder()
{
	if (isAttached(_encoderNum))
	{
		byte buffer[5];

		buffer[0] = (START_SYSEX);
		buffer[1] = (ENCODER_DATA);
		buffer[2] = (ENCODER_DETACH);
		buffer[3] = (_encoderNum);
		buffer[4] = (END_SYSEX);
		MyFirmata.write(buffer, 5);

		encoders[_encoderNum] = false;
	}
}

void Encoder::reportPosition()
{
	byte buffer[5];

	buffer[0] = (START_SYSEX);
	buffer[1] = (ENCODER_DATA);
	buffer[2] = (ENCODER_REPORT_POSITION);
	buffer[3] = (_encoderNum);
	buffer[4] = (END_SYSEX);
	MyFirmata.write(buffer, 5);
}

void Encoder::reportPositions()
{
	byte buffer[4];

	buffer[0] = (START_SYSEX);
	buffer[1] = (ENCODER_DATA);
	buffer[2] = (ENCODER_REPORT_POSITIONS);
	buffer[3] = (END_SYSEX);
	MyFirmata.write(buffer, 4);
}

void Encoder::resetPosition()
{
	byte buffer[5];

	buffer[0] = (START_SYSEX);
	buffer[1] = (ENCODER_DATA);
	buffer[2] = (ENCODER_RESET_POSITION);
	buffer[3] = (_encoderNum);
	buffer[4] = (END_SYSEX);

	MyFirmata.write(buffer, 5);
}

void Encoder::toggleAutoReport(byte report)
{
	autoReport = report;
	byte buffer[5];

	buffer[0] = (START_SYSEX);
	buffer[1] = (ENCODER_DATA);
	buffer[2] = (ENCODER_REPORT_AUTO);
	buffer[3] = (report);
	buffer[4] = (END_SYSEX);
	MyFirmata.write(buffer, 5);
}

//void Encoder::reset()
//{
//  byte encoder;
//  for (encoder = 0; encoder < MAX_ENCODERS; encoder++)
//  {
//    detachEncoder(encoder);
//  }
//  autoReport = 0x02;
//}

boolean Encoder::isEncoderAttached(byte encoderNum)
{
	return isAttached(encoderNum);
}

bool Encoder::isReportingEnabled()
{
	return autoReport > 0;
}

bool Encoder::isAttached(byte encoderNum)
{
	return encoderNum < MAX_ENCODERS && encoders[encoderNum];
}


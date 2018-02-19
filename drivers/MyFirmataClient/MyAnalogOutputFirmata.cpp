/*
  MyAnalogOutputFirmata.h - Firmata library
  Copyright (C) 2006-2008 Hans-Christoph Steiner.  All rights reserved.
  Copyright (C) 2010-2011 Paul Stoffregen.  All rights reserved.
  Copyright (C) 2009 Shigeru Kobayashi.  All rights reserved.
  Copyright (C) 2013 Norbert Truchsess. All rights reserved.
  Copyright (C) 2009-2015 Jeff Hoefs.  All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  See file LICENSE.txt for further informations on licensing terms.

  Last updated December 23rd, 2016
*/

#include "MyAnalogOutputFirmata.h"

MyAnalogOutputFirmata *MyAnalogOutputFirmataInstance;

MyAnalogOutputFirmata::MyAnalogOutputFirmata()
{
	MyAnalogOutputFirmataInstance = this;
//	MyFirmata.addFeature(*MyAnalogOutputFirmataInstance);
}

boolean MyAnalogOutputFirmata::handleSysex(byte command, byte argc, byte* argv)
{
	return false;
}

void MyAnalogOutputFirmata::update()
{
	return;
}

void MyAnalogOutputFirmata::analogWritePort(byte analogPin, int value)
{
	byte message[3];

	message[0] = (byte)(ANALOG_MESSAGE | analogPin);
	message[1] = (byte)(value & 0x7F);
	message[2] = (byte)(value >> 7);
	MyFirmata.write(message, 3);
}

void MyAnalogOutputFirmata::setPinMode(byte pin, int mode)
{
	MyFirmata.setPinMode(pin, mode);
}

void MyAnalogOutputFirmata::setPinValue(byte analogPin, int value)
{
	int port = analogPin;
	if (port < 16) {
		
		if (!_pwmPins[analogPin]) {
			setPinMode(analogPin, PIN_MODE_PWM);
			_pwmPins[analogPin] = true;
		}		
		analogWritePort(analogPin, value);
	}
}

void MyAnalogOutputFirmata::handleCapability(byte pin)
{
  // if (IS_PIN_PWM(pin)) {
    // Firmata.write(PIN_MODE_PWM);
    // Firmata.write(DEFAULT_PWM_RESOLUTION);
  // }
}

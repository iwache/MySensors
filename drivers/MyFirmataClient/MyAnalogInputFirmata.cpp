/*
  MyAnalogInputFirmata.h - Firmata library
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

  Last updated by Jeff Hoefs: November 22nd, 2015
*/

//#include "MyAnalogFirmata.h"
#include "MyAnalogInputFirmata.h"

MyAnalogInputFirmata *MyAnalogInputFirmataInstance;

void myAnalogMessageCallback(byte analogPin, int value)
{
  MyAnalogInputFirmataInstance->setAnalogPort(analogPin, value);
}

MyAnalogInputFirmata::MyAnalogInputFirmata()
{
  MyAnalogInputFirmataInstance = this;
//	MyFirmata.addFeature(*this);
  MyFirmata.attach(ANALOG_MESSAGE, myAnalogMessageCallback);
}

void MyAnalogInputFirmata::setAnalogPort(byte analogPin, int value)
{
	if (analogPin < 16) {
		_analogPorts[analogPin] = value;
	}
}

void MyAnalogInputFirmata::setPinMode(byte pin, int mode)
{
	MyFirmata.setPinMode(pin, mode);
}

void MyAnalogInputFirmata::reportAnalogPort(byte port)
{
	byte message[2];

	message[0] = (byte)(REPORT_ANALOG | port);
	message[1] = 1;
	MyFirmata.write(message, 2);

	_reportPorts[port] = true;
}

int MyAnalogInputFirmata::getPinValue(byte analogPin)
{
	int value = 0;
	if (analogPin < 16) {
		value = _analogPorts[analogPin];
		if (!_reportPorts[analogPin]) {
			setPinMode(analogPin, PIN_MODE_ANALOG);
			reportAnalogPort(analogPin);
		}
	}
	return value;
}

boolean MyAnalogInputFirmata::handleSysex(byte command, byte argc, byte* argv)
{
	return false;
}

void MyAnalogInputFirmata::update()
{
	return;
}

void MyAnalogInputFirmata::handleCapability(byte pin)
{
  // if (IS_PIN_ANALOG(pin)) {
    // MyFirmata.write((byte)INPUT);
    // MyFirmata.write(1);
    // MyFirmata.write((byte)PIN_MODE_PULLUP);
    // MyFirmata.write(1);
  // }
}

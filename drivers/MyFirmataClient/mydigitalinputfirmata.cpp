/*
  MyDigitalInputFirmata.cpp - Firmata library
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

  Last updated by Jeff Hoefs: November 15th, 2015
*/

#include "MyDigitalInputFirmata.h"

MyDigitalInputFirmata *MyDigitalInputFirmataInstance;

void myDigitalMessageCallback(byte port, int value)
{
	MyDigitalInputFirmataInstance->setDigitalPort(port, value);
}

MyDigitalInputFirmata::MyDigitalInputFirmata()
{
	MyDigitalInputFirmataInstance = this;
//	MyFirmata.addFeature(*this);
	MyFirmata.attach(DIGITAL_MESSAGE, myDigitalMessageCallback);
}

void MyDigitalInputFirmata::setDigitalPort(byte port, int value)
{
	if (port < 16) {
		_digitalPorts[port] = value;
	}
}

void MyDigitalInputFirmata::setPinMode(byte pin, int mode)
{
	MyFirmata.setPinMode(pin, mode);

	byte port = (pin >> 3) & 0x0F;
	if (!_reportPorts[port])
		reportDigitalPort(port);
}

void MyDigitalInputFirmata::reportDigitalPort(byte port)
{
	byte message[2];

	message[0] = (byte)(REPORT_DIGITAL | port);
	message[1] = 1;
	MyFirmata.write(message, 2);

	_reportPorts[port] = true;
}

bool MyDigitalInputFirmata::getPinValue(byte pin)
{
	int port = (pin >> 3) & 0x0F;
	
	byte value = _digitalPorts[port] & (1 << (pin & 0x07));
	return value != 0;
}

boolean MyDigitalInputFirmata::handleSysex(byte command, byte argc, byte* argv)
{
	return false;
}

void MyDigitalInputFirmata::update()
{
	return;
}

void MyDigitalInputFirmata::handleCapability(byte pin)
{
  // if (IS_PIN_DIGITAL(pin)) {
    // Firmata.write((byte)INPUT);
    // Firmata.write(1);
    // Firmata.write((byte)PIN_MODE_PULLUP);
    // Firmata.write(1);
  // }
}


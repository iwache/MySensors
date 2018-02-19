/*
  MyDigitalOutputFirmata.cpp - Firmata library
  Copyright (C) 2006-2008 Hans-Christoph Steiner.  All rights reserved.
  Copyright (C) 2010-2011 Paul Stoffregen.  All rights reserved.
  Copyright (C) 2009 Shigeru Kobayashi.  All rights reserved.
  Copyright (C) 2013 Norbert Truchsess. All rights reserved.
  Copyright (C) 2009-2016 Jeff Hoefs.  All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  See file LICENSE.txt for further informations on licensing terms.

  Last updated by Jeff Hoefs: February 16th, 2016
*/

#include "MyDigitalOutputFirmata.h"

MyDigitalOutputFirmata *MyDigitalOutputFirmataInstance;

MyDigitalOutputFirmata::MyDigitalOutputFirmata()
{
  MyDigitalOutputFirmataInstance = this;
  MyFirmata.addFeature(*MyDigitalOutputFirmataInstance);
}

boolean MyDigitalOutputFirmata::handleSysex(byte command, byte argc, byte* argv)
{
  return false;
}

void MyDigitalOutputFirmata::update()
{
	return;
}

void MyDigitalOutputFirmata::digitalWritePort(byte port, int value)
{
	byte message[3];

	message[0] = (byte)(DIGITAL_MESSAGE | port);
	message[1] = (byte)(value & 0x7F);
	message[2] = (byte)(value >> 7);
	MyFirmata.write(message, 3);
	digitalPorts[port] = value;
}

void MyDigitalOutputFirmata::setPinMode(byte pin, int mode)
{
	MyFirmata.setPinMode(pin, mode);
}

void MyDigitalOutputFirmata::setPinValue(byte pin, bool value)
{
	int port = (pin >> 3) & 0x0F;
	if (value == 0)
		digitalPorts[port] &= ~(1 << (pin & 0x07));
	else
		digitalPorts[port] |= (1 << (pin & 0x07));

	digitalWritePort(port, digitalPorts[port]);
}

void MyDigitalOutputFirmata::handleCapability(byte pin)
{
	//if (IS_PIN_DIGITAL(pin)) {
	//  MyFirmata.write((byte)OUTPUT);
	//  MyFirmata.write(1);
	//}
}

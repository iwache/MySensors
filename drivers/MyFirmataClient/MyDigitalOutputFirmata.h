/*
  MyDigitalOutputFirmata.h - Firmata library
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

#ifndef MyDigitalOutputFirmata_h
#define MyDigitalOutputFirmata_h

#include "MyFirmataClass.h"
#include "MyFirmataFeature.h"

class MyDigitalOutputFirmata : public MyFirmataFeature
{
  public:
	MyDigitalOutputFirmata();

	void handleCapability(byte pin);

	void setPinMode(byte pin, int mode);
	void setPinValue(byte pin, bool value);
	void digitalWritePort(byte port, int value);
	boolean handleSysex(byte command, byte argc, byte* argv);
	void update();

  private:
	int digitalPorts[16]; // all binary channels
};

#endif

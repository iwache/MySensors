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
*/

#ifndef MyAnalogOutputFirmata_h
#define MyAnalogOutputFirmata_h

#include "MyFirmataClass.h"
#include "MyFirmataFeature.h"

class MyAnalogOutputFirmata: public MyFirmataFeature
{
  public:
    MyAnalogOutputFirmata();
	
	void handleCapability(byte pin);

	void setPinMode(byte pin, int mode);
	void setPinValue(byte pin, int value);
	void analogWritePort(byte analogPin, int value);
	boolean handleSysex(byte command, byte argc, byte* argv);
	void update();
  private:
	bool _pwmPins[16]; // pin was set to pin mode PIN_MODE_PWM
};

#endif

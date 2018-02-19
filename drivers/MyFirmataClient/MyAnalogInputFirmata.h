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
*/

#ifndef MyAnalogInputFirmata_h
#define MyAnalogInputFirmata_h

#include "MyFirmataClass.h"
#include "MyFirmataFeature.h"

void myAnalogMessageCallback(byte analogPin, int value);

class MyAnalogInputFirmata: public MyFirmataFeature
{
  public:
    MyAnalogInputFirmata();
	
	void setPinMode(byte pin, int mode);
	int getPinValue(byte analogPin);
	void setAnalogPort(byte analogPin, int value);

    void handleCapability(byte pin);
    boolean handleSysex(byte command, byte argc, byte* argv);
    void update();

  private:
	int _analogPorts[16]; // all analog input ports (one signal pin is one port)
	bool _reportPorts[16]; // 1 = report this port, 0 = silence

	void reportAnalogPort(byte port);
};

#endif

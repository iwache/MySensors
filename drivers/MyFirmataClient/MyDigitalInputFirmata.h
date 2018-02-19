/*
  MyDigitalInputFirmata.h - Firmata library
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

#ifndef MyDigitalInputFirmata_h
#define MyDigitalInputFirmata_h

#include "MyFirmataClass.h"
#include "MyFirmataFeature.h"

void myDigitalMessageCallback(byte port, int value);

class MyDigitalInputFirmata: public MyFirmataFeature
{
  public:
    MyDigitalInputFirmata();

	void setPinMode(byte pin, int mode);
	bool getPinValue(byte pin);
	void setDigitalPort(byte port, int value);

    void handleCapability(byte pin);
    boolean handleSysex(byte command, byte argc, byte* argv);
    void update();

  private:
	int _digitalPorts[16]; // all binary input ports 
	bool _reportPorts[16]; // 1 = report this port, 0 = silence

	void reportDigitalPort(byte port);
};

#endif

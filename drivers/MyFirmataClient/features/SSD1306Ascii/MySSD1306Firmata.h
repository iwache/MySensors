/*
  MySSD1306Firmata.h - Firmata library
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

#ifndef MySSD1306Firmata_h
#define MySSD1306Firmata_h

#include "../../MyFirmataClass.h"
#include "../../MyFirmataFeature.h"
#include "../../MyEncoder7Bit.h"

#define OLED_DATA               0x63 // control a SSD1306 OLED (proposal! belongs later on into line 76 of c:\Users\Vagrant\Source\Repos\libraries\ConfigurableFirmata\src\configurablefirmata.h)

#define MAX_OLEDS                5 // arbitrary value, may need to adjust
#define OLED_ATTACH              (0x00)
#define OLED_RESET	             (0x01)
#define OLED_WRITE_DISPLAY       (0x02)
#define OLED_WRITE_DISPLAY_BUF   (0x03)
#define OLED_DETACH              (0x04)

class MySSD1306Firmata: public MyFirmataFeature
{
  public:
    MySSD1306Firmata();
	//~MySSD1306Firmata(); => never destroy in practice
	
	// MyFirmataFeature implementation
	void handleCapability(byte pin);
	void setPinMode(byte pin, int mode);
	boolean handleSysex(byte command, byte argc, byte* argv);
	void update();

	// real SSD1306 Firmata methods implementation
	void attachOled(byte pinCs, byte pinDc, byte pinRst);
	void detachOled();
	void resetOled();
	void writeDisplay(byte data, byte mode);
	void writeDisplayBuffer(const uint8_t *buffer, size_t size);

  private:
	byte _oledNum = 255;
	byte _pinCs;
	byte _pinDc;
	byte _pinRst;

	void _attachOled();
	bool _isAttached(byte oledNum);

};

#endif

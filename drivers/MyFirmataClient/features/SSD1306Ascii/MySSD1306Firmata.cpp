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

  Last updated December 23rd, 2016
*/

#include "MySSD1306Firmata.h"

static bool oleds[MAX_OLEDS];


MySSD1306Firmata::MySSD1306Firmata()
{
	static byte _instanceCount;

	_oledNum = _instanceCount++;
	MyFirmata.addFeature(*this);
}

void MySSD1306Firmata::handleCapability(byte pin)
{
	// do nothing
}

void MySSD1306Firmata::setPinMode(byte pin, int mode)
{
	// do nothing
}

boolean MySSD1306Firmata::handleSysex(byte command, byte argc, byte* argv)
{
	return false;
}

void MySSD1306Firmata::update()
{
	return;
}

void MySSD1306Firmata::attachOled(byte pinCs, byte pinDc, byte pinRst)
{
	_pinCs = pinCs;
	_pinDc = pinDc;
	_pinRst = pinRst;
	_attachOled();
}

void MySSD1306Firmata::detachOled()
{
	if (_isAttached(_oledNum))
	{
		byte buffer[5];

		buffer[0] = (START_SYSEX);
		buffer[1] = (OLED_DATA);
		buffer[2] = (OLED_DETACH);
		buffer[3] = (_oledNum);
		buffer[4] = (END_SYSEX);
		MyFirmata.write(buffer, 5);

		oleds[_oledNum] = false;
	}
}

void MySSD1306Firmata::resetOled()
{
	if (_isAttached(_oledNum))
	{
		byte buffer[5];

		buffer[0] = (START_SYSEX);
		buffer[1] = (OLED_DATA);
		buffer[2] = (OLED_RESET);
		buffer[3] = (_oledNum);
		buffer[4] = (END_SYSEX);
		MyFirmata.write(buffer, 5);
	}
}
	
void MySSD1306Firmata::writeDisplay(byte data, byte mode)
{
	if (_isAttached(_oledNum))
	{
		byte buffer[9];

		buffer[0] = (START_SYSEX);
		buffer[1] = (OLED_DATA);
		buffer[2] = (OLED_WRITE_DISPLAY);
		buffer[3] = (_oledNum);
		buffer[4] = (byte)(data >> 7);
		buffer[5] = (byte)(data & 0x7F);
		buffer[6] = (byte)(mode >> 7);
		buffer[7] = (byte)(mode & 0x7F);
		buffer[8] = (END_SYSEX);
		MyFirmata.write(buffer, 9);
	}
}

void MySSD1306Firmata::writeDisplayBuffer(const uint8_t *buffer, size_t size)
{
	if (_isAttached(_oledNum))
	{
		byte outData[64];

		outData[0] = (START_SYSEX);
		outData[1] = (OLED_DATA);
		outData[2] = (OLED_WRITE_DISPLAY_BUF);
		outData[3] = (_oledNum);
		MyEncoder7Bit.startBinaryWrite(outData, 4);
		for (size_t i = 0; i < size; i++) {
			MyEncoder7Bit.writeBinary(buffer[i]);
		}
		int pos = MyEncoder7Bit.endBinaryWrite();
		outData[pos++] = (END_SYSEX);
		MyFirmata.write(outData, pos);
	}
}

bool MySSD1306Firmata::_isAttached(byte oledNum)
{ 
	return oledNum < MAX_OLEDS && oleds[oledNum];
}

void MySSD1306Firmata::_attachOled()
{
	byte buffer[8];

	buffer[0] = (START_SYSEX);
	buffer[1] = (OLED_DATA);
	buffer[2] = (OLED_ATTACH);
	buffer[3] = (_oledNum);
	buffer[4] = (_pinCs);
	buffer[5] = (_pinDc);
	buffer[6] = (_pinRst);
	buffer[7] = (END_SYSEX);
	MyFirmata.write(buffer, 8);

	oleds[_oledNum] = true;
}

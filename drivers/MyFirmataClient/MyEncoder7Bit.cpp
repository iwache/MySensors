/*
  MyEncoder7Bit.cpp - Firmata library
  Copyright (C) 2012-2013 Norbert Truchsess. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  See file LICENSE.txt for further informations on licensing terms.
*/

#include "MyEncoder7Bit.h"
#include "MyFirmataClass.h"

MyEncoder7BitClass::MyEncoder7BitClass()
{
	previous = 0;
	shift = 0;
}

void MyEncoder7BitClass::startBinaryWrite(byte *outData, int outPos)
{
	_outData = outData;
	_outPos = outPos;

	previous = 0;
	shift = 0;
}

int MyEncoder7BitClass::endBinaryWrite()
{
	if (shift > 0) {
		_write(previous);
	}
	return _outPos;
}

void MyEncoder7BitClass::_write(byte data)
{
	_outData[_outPos++] = data;
}

void MyEncoder7BitClass::writeBinary(byte data)
{
	if (shift == 0) {
		_write(data & 0x7f);
		shift++;
		previous = data >> 7;
	}
	else {
		_write(((data << shift) & 0x7f) | previous);
		if (shift == 6) {
			_write(data >> 1);
			shift = 0;
		}
		else {
			shift++;
			previous = data >> (8 - shift);
		}
	}
}

void MyEncoder7BitClass::readBinary(int outBytes, byte *inData, byte *outData)
{
	for (int i = 0; i < outBytes; i++) {
		int j = i << 3;
		int pos = j / 7;
		byte shift = j % 7;
		outData[i] = (inData[pos] >> shift) | ((inData[pos + 1] << (7 - shift)) & 0xFF);
	}
}

MyEncoder7BitClass MyEncoder7Bit;

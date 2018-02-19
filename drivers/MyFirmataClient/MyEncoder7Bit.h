/*
  MyEncoder7Bit.h - Firmata library
  Copyright (C) 2012-2013 Norbert Truchsess. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  See file LICENSE.txt for further informations on licensing terms.
*/

#ifndef MyEncoder7Bit_h
#define MyEncoder7Bit_h
#include <Arduino.h>

#define num7BitOutbytes(a)(((a)*7)>>3)

class MyEncoder7BitClass
{
public:
	MyEncoder7BitClass();
	void startBinaryWrite(byte *outData, int outPos);
	int endBinaryWrite();
	void writeBinary(byte data);
	void readBinary(int outBytes, byte *inData, byte *outData);

private:
	void _write(byte data);

	byte previous;
	int shift;
	byte *_outData;
	int _outPos;
};

extern MyEncoder7BitClass MyEncoder7Bit;

#endif

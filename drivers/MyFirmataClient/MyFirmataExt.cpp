/*
  MyFirmataExt.h - MyFirmata library
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

#include "MyFirmataClass.h"
#include "MyFirmataExt.h"

MyFirmataExt *MyFirmataExtInstance;

//void myHandleSetPinModeCallback(byte pin, int mode)
//{
//  if (!MyFirmataExtInstance->handlePinMode(pin, mode) && mode != PIN_MODE_IGNORE) {
//    MyFirmata.sendString("Unknown pin mode"); // TODO: put error msgs in EEPROM
//  }
//}

void myHandleSysexCallback(byte command, byte argc, byte* argv)
{
  if (!MyFirmataExtInstance->handleSysex(command, argc, argv)) {
	  logDebug("Unhandled sysex command %u\n", command);
  }
}

MyFirmataExt::MyFirmataExt()
{
  MyFirmataExtInstance = this;
//  MyFirmata.attach(SET_PIN_MODE, myHandleSetPinModeCallback);
  MyFirmata.attach((byte)START_SYSEX, myHandleSysexCallback);
  numFeatures = 0;
}

void MyFirmataExt::handleCapability(byte pin)
{

}

void MyFirmataExt::setPinMode(byte pin, int mode)
{
	MyFirmata.setPinMode(pin, mode);
}

boolean MyFirmataExt::handleSysex(byte command, byte argc, byte* argv)
{
  switch (command) {

    case PIN_STATE_QUERY:
    //  if (argc > 0) {
    //    byte pin = argv[0];
    //    if (pin < TOTAL_PINS) {
    //      MyFirmata.write(START_SYSEX);
    //      MyFirmata.write(PIN_STATE_RESPONSE);
    //      MyFirmata.write(pin);
    //      MyFirmata.write(MyFirmata.getPinMode(pin));
    //      int pinState = MyFirmata.getPinState(pin);
    //      MyFirmata.write((byte)pinState & 0x7F);
    //      if (pinState & 0xFF80) MyFirmata.write((byte)(pinState >> 7) & 0x7F);
    //      if (pinState & 0xC000) MyFirmata.write((byte)(pinState >> 14) & 0x7F);
    //      MyFirmata.write(END_SYSEX);
    //      return true;
    //    }
    //  }
    //  break;
    case CAPABILITY_QUERY:
    //  MyFirmata.write(START_SYSEX);
    //  MyFirmata.write(CAPABILITY_RESPONSE);
    //  for (byte pin = 0; pin < TOTAL_PINS; pin++) {
    //    if (MyFirmata.getPinMode(pin) != PIN_MODE_IGNORE) {
    //      for (byte i = 0; i < numFeatures; i++) {
    //        features[i]->handleCapability(pin);
    //      }
    //    }
    //    MyFirmata.write(127);
    //  }
    //  MyFirmata.write(END_SYSEX);
    //  return true;
    default:
      for (byte i = 0; i < numFeatures; i++) {
        if (features[i]->handleSysex(command, argc, argv)) {
          return true;
        }
      }
      break;
  }
  return false;
}

void MyFirmataExt::addFeature(MyFirmataFeature &capability)
{
  if (numFeatures < MAX_FEATURES) {
    features[numFeatures++] = &capability;
  }
}

void MyFirmataExt::update()
{
  for (byte i = 0; i < numFeatures; i++) {
    features[i]->update();
  }
}

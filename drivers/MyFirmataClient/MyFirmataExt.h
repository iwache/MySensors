/*
  MyFirmataExt.h - Firmata library
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

#ifndef MyFirmataExt_h
#define MyFirmataExt_h

#include "MyFirmataClass.h"
#include "MyFirmataFeature.h"

#define MAX_FEATURES TOTAL_PIN_MODES + 1

//void myHandleSetPinModeCallback(byte pin, int mode);

void myHandleSysexCallback(byte command, byte argc, byte* argv);

class MyFirmataExt: public MyFirmataFeature
{
  public:
    MyFirmataExt();
    void handleCapability(byte pin); //empty method
    void setPinMode(byte pin, int mode);

    boolean handleSysex(byte command, byte argc, byte* argv);
    void addFeature(MyFirmataFeature &capability);

    void update();

  private:
    MyFirmataFeature *features[MAX_FEATURES];
    byte numFeatures;
};

#endif

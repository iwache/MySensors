/*
 * The MySensors Arduino library handles the wireless radio link and protocol
 * between your home built sensors/actuators and HA controller of choice.
 * The sensors forms a self healing radio network with optional repeaters. Each
 * repeater and gateway builds a routing tables in EEPROM which keeps track of the
 * network topology allowing messages to be routed to nodes.
 *
 * Created by Henrik Ekblad <henrik.ekblad@mysensors.org>
 * Copyright (C) 2013-2017 Sensnology AB
 * Full contributor list: https://github.com/mysensors/Arduino/graphs/contributors
 *
 * Documentation: http://www.mysensors.org
 * Support Forum: http://forum.mysensors.org
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 */

#include "GPIO.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

// Declare a single default instance
GPIOClass GPIO = GPIOClass();

GPIOClass::GPIOClass()
{
}

GPIOClass::GPIOClass(const GPIOClass& other)
{
}

GPIOClass::~GPIOClass()
{
}

void GPIOClass::_pinMode(uint8_t pin, uint8_t mode)
{
#if defined(MY_FIRMATA_CLIENT)	
	switch (mode)
	{
	case INPUT:
		digitalInputFeature.setPinMode(pin, INPUT);
	case INPUT_PULLUP:
		digitalInputFeature.setPinMode(pin, PIN_MODE_PULLUP);
		break;
	case OUTPUT:
		digitalOutputFeature.setPinMode(pin, OUTPUT);
	default:
		break;
	}
#endif
}

void GPIOClass::_digitalWrite(uint8_t pin, uint8_t value)
{
#if defined(MY_FIRMATA_CLIENT)
	digitalOutputFeature.setPinValue(pin, value != 0);
#endif
}

uint8_t GPIOClass::_digitalRead(uint8_t pin)
{
#if defined(MY_FIRMATA_CLIENT)
	return digitalInputFeature.getPinValue(pin);
#else
	return 0;
#endif
}

uint16_t GPIOClass::_analogRead(uint8_t pin)
{
#if defined(MY_FIRMATA_CLIENT)
	return analogInputFeature.getPinValue(pin);
#else
	return 0;
#endif
}

void GPIOClass::_analogWrite(uint8_t pin, uint16_t value)
{
#if defined(MY_FIRMATA_CLIENT)
	analogOutputFeature.setPinValue(pin, value);
#endif
}

uint8_t GPIOClass::_digitalPinToInterrupt(uint8_t pin)
{
	return pin;
}

GPIOClass& GPIOClass::operator=(const GPIOClass& other)
{
	if (this != &other) {
	}
	return *this;
}

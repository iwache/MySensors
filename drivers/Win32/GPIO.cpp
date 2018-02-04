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
//#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
//#include <unistd.h>
#include "log.h"

// Declare a single default instance
GPIOClass GPIO = GPIOClass();

GPIOClass::GPIOClass()
{
}

GPIOClass::GPIOClass(const GPIOClass& other)
{
	lastPinNum = other.lastPinNum;

	exportedPins = new uint8_t[lastPinNum + 1];
	for (int i = 0; i < lastPinNum + 1; ++i) {
		exportedPins[i] = other.exportedPins[i];
	}
}

GPIOClass::~GPIOClass()
{
	delete [] exportedPins;
}

void GPIOClass::pinMode(uint8_t pin, uint8_t mode)
{
	if (pin > lastPinNum) {
		return;
	}
	exportedPins[pin] = 1;
}

void GPIOClass::digitalWrite(uint8_t pin, uint8_t value)
{
	if (pin > lastPinNum) {
		return;
	}
	if (0 == exportedPins[pin]) {
		pinMode(pin, OUTPUT);
	}
}

uint8_t GPIOClass::digitalRead(uint8_t pin)
{
	if (pin > lastPinNum) {
		return 0;
	}
	if (0 == exportedPins[pin]) {
		pinMode(pin, INPUT);
	}
	
	uint8_t i = 0;
	return i;
}

uint8_t GPIOClass::digitalPinToInterrupt(uint8_t pin)
{
	return pin;
}

GPIOClass& GPIOClass::operator=(const GPIOClass& other)
{
	if (this != &other) {
		lastPinNum = other.lastPinNum;

		exportedPins = new uint8_t[lastPinNum + 1];
		for (int i = 0; i < lastPinNum + 1; ++i) {
			exportedPins[i] = other.exportedPins[i];
		}
	}
	return *this;
}

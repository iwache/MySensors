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

#include "Arduino.h"
#include <stdlib.h>

TimingWrapper timingWrapper;
#if defined(MY_PROCESS_SYNCHRONIZATION)
ProcessSynchronizationWrapper processSynchronizationWrapper;
#endif

void yield(void) {}

unsigned long millis(void)
{
#if defined(MY_PROCESS_SYNCHRONIZATION)
	return processSynchronizationWrapper.millis();
#else
	return timingWrapper.millis();
#endif
}

unsigned long micros()
{
	return timingWrapper.micros();
}

void _delay_milliseconds(unsigned int millis)
{
	_sleep(millis);
}

void _delay_milliseconds_and_proc_sync(unsigned int millis)
{
#if defined(MY_PROCESS_SYNCHRONIZATION)
	processSynchronizationWrapper.wait(millis);
#else
	_sleep(millis);
#endif
}

void _delay_microseconds(unsigned int micro)
{
	//nanosleep(&sleeper, NULL);
	// This seems not possible in Windows
}

void randomSeed(unsigned long seed)
{
	if (seed != 0) {
		srand(seed);
	}
}

long randMax(long howbig)
{
	if (howbig == 0) {
		return 0;
	}
	return rand() % howbig;
}

long randMinMax(long howsmall, long howbig)
{
	if (howsmall >= howbig) {
		return howsmall;
	}
	long diff = howbig - howsmall;
	return randMax(diff) + howsmall;
}

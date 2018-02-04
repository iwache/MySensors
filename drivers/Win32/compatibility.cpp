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


#include <time.h>
#include <time.h>
#include <stdlib.h>

#include <windows.h>

//#include <winsock.h>
//#include <winnt.h>
//#include <minwindef.h>
//#include <minwinbase.h>
//#include <timezoneapi.h>
//#include <sysinfoapi.h>

#include "Arduino.h"

const __int64 DELTA_EPOCH_IN_MICROSECS = 11644473600000000;

int gettimeofday(struct timeval *tv)
{
	FILETIME ft;
	__int64 tmpres = 0;
	TIME_ZONE_INFORMATION tz_winapi;
	int rez = 0;

	ZeroMemory(&ft, sizeof(ft));
	ZeroMemory(&tz_winapi, sizeof(tz_winapi));

	GetSystemTimeAsFileTime(&ft);

	tmpres = ft.dwHighDateTime;
	tmpres <<= 32;
	tmpres |= ft.dwLowDateTime;

	/*converting file time to unix epoch*/
	tmpres /= 10;  /*convert into microseconds*/
	tmpres -= DELTA_EPOCH_IN_MICROSECS;
	tv->tv_sec = (__int32)(tmpres*0.000001);
	tv->tv_usec = (tmpres % 1000000);

	return 0;
}

// For millis()
static unsigned long millis_at_start = 0;

void yield(void) {}

unsigned long millis(void)
{
	timeval curTime;

	if (millis_at_start == 0) {
		gettimeofday(&curTime);
		millis_at_start = curTime.tv_sec;
	}

	gettimeofday(&curTime);
	return ((curTime.tv_sec - millis_at_start) * 1000) + (curTime.tv_usec / 1000);

	return 0;
}

unsigned long micros()
{
	timeval curTime;

	if (millis_at_start == 0) {
		gettimeofday(&curTime);
		millis_at_start = curTime.tv_sec;
	}

	gettimeofday(&curTime);
	return ((curTime.tv_sec - millis_at_start) * 1000000) + (curTime.tv_usec);

	return 0;
}

void _delay_milliseconds(unsigned int millis)
{
	Sleep(millis);
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

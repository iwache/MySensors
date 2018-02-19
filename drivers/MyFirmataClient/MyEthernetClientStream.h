/*
  MyEthernetClientStream.h
  An Arduino-Stream that wraps an instance of Client reconnecting to
  the remote-ip in a transparent way. A disconnected client may be
  recognized by the returnvalues -1 from calls to peek or read and
  a 0 from calls to write.

  Copyright (C) 2013 Norbert Truchsess. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  See file LICENSE.txt for further informations on licensing terms.

  Last updated June 18th, 2016
 */

#ifndef MYETHERNETCLIENTSTREAM_H
#define MYETHERNETCLIENTSTREAM_H

#include <Arduino.h>
#include <inttypes.h>
#include <Stream.h>
#include <Client.h>

//#define SERIAL_DEBUG
//#include "utility/firmataDebug.h"
#include <log.h>

#define MILLIS_RECONNECT 5000

class MyEthernetClientStream : public Stream
{
  public:
    MyEthernetClientStream(Client &client, const char* host, uint16_t port);
    int available();
    int read();
    int peek();
    void flush();
    size_t write(uint8_t);
	size_t write(const uint8_t *buf, size_t size);

  private:
    Client &client;
    const char* host;
    uint16_t port;
    bool connected;
    uint32_t time_connect;
    bool maintain();
    void stop();
};


/*
 * MyEthernetClientStream.cpp
 * Copied here as a hack to linker issues with 3rd party board packages that don't properly
 * implement the Arduino network APIs.
 */
MyEthernetClientStream::MyEthernetClientStream(Client &client, const char* host, uint16_t port)
  : client(client),
    host(host),
    port(port),
    connected(false)
{
}

int
MyEthernetClientStream::available()
{
  return maintain() ? client.available() : 0;
}

int
MyEthernetClientStream::read()
{
  return maintain() ? client.read() : -1;
}

int
MyEthernetClientStream::peek()
{
  return maintain() ? client.peek() : -1;
}

void MyEthernetClientStream::flush()
{
  if (maintain())
    client.flush();
}

size_t
MyEthernetClientStream::write(uint8_t c)
{
  return maintain() ? client.write(c) : 0;
}

size_t 
MyEthernetClientStream::write(const uint8_t *buf, size_t size)
{
	return maintain() ? client.write(buf, size) : 0;
}

void
MyEthernetClientStream::stop()
{
  client.stop();
  connected = false;
  time_connect = millis();
}

bool
MyEthernetClientStream::maintain()
{
  if (client && client.connected())
    return true;

  if (connected) {
    stop();
  }
  // if the client is disconnected, attempt to reconnect every 5 seconds
  else if (millis() - time_connect >= MILLIS_RECONNECT) {
	  connected = client.connect(host, port) == 1;
    if (!connected) {
      time_connect = millis();
      logDebug("connection failed. attempting to reconnect...\n");
    } else {
		logDebug("connected\n");
    }
  }
  return connected;
}

#endif /* MYETHERNETCLIENTSTREAM_H */

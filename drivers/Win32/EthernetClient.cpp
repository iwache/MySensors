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
 *
 * Based on Arduino ethernet library, Copyright (c) 2010 Arduino LLC. All right reserved.
 */

#include <Arduino.h>

#include "EthernetClient.h"
#include <cstdio>
#include <cstring>
#include <errno.h>
#include "log.h"

EthernetClient::EthernetClient() : _sock(-1)
{
}

EthernetClient::EthernetClient(int sock) : _sock(sock)
{
}

EthernetClient::~EthernetClient()
{
	// ToDo: do we really need this
}

int EthernetClient::connect(const char* host, uint16_t port)
{
	close();

	int result = ethernetWrapper.clientConnect(host, port, &_sock);
	if (result != 1) {
		logError("connect: %d %s - %s\n", ethernetWrapper.clientErrorCode(_sock), 
			ethernetWrapper.clientSocketErrorCode(_sock), ethernetWrapper.clientErrorMessage(_sock));
	}
	return result;

	//result = _client->connect(host, port);
	//if (result == 1) {
	//	_sock = _client->getSocketNumber();
	//} else {
	//	_sock = -1;
	//	logError("connect: %d %s - %s\n", _client->getErrorCode(), _client->getSocketErrorCode(), _client->getErrorMessage());
	//}
	//return result;

	//struct addrinfo hints, *servinfo, *localinfo, *p;
	//int rv;
	//char s[INET6_ADDRSTRLEN];
	//char port_str[6];
	//bool use_bind = (_srcip != 0);

	//close();

	//memset(&hints, 0, sizeof hints);
	//hints.ai_family = AF_UNSPEC;
	//hints.ai_socktype = SOCK_STREAM;

	//sprintf(port_str, "%hu", port);
	//if ((rv = getaddrinfo(host, port_str, &hints, &servinfo)) != 0) {
	//	logError("getaddrinfo: %s\n", gai_strerror(rv));
	//	return -1;
	//}
	//if (use_bind) {
	//	if ((rv = getaddrinfo(_srcip.toString().c_str(), port_str, &hints, &localinfo)) != 0) {
	//		logError("getaddrinfo: %s\n", gai_strerror(rv));
	//		return -1;
	//	}
	//}

	//// loop through all the results and connect to the first we can
	//for (p = servinfo; p != NULL; p = p->ai_next) {
	//	if ((_sock = socket(p->ai_family, p->ai_socktype,
	//	                    p->ai_protocol)) == -1) {
	//		logError("socket: %s\n", strerror(errno));
	//		continue;
	//	}

	//	if (use_bind) {
	//		if (::bind(_sock, localinfo->ai_addr, localinfo->ai_addrlen) == -1) {
	//			close();
	//			logError("bind: %s\n", strerror(errno));
	//			return -1;
	//		}
	//	}

	//	if (::connect(_sock, p->ai_addr, p->ai_addrlen) == -1) {
	//		close();
	//		logError("connect: %s\n", strerror(errno));
	//		continue;
	//	}

	//	break;
	//}

	//if (p == NULL) {
	//	logError("failed to connect\n");
	//	return -1;
	//}

	//void *addr = &(((struct sockaddr_in*)p->ai_addr)->sin_addr);
	//inet_ntop(p->ai_family, addr, s, sizeof s);
	//logDebug("connected to %s\n", s);

	//freeaddrinfo(servinfo); // all done with this structure
	//if (use_bind) {
	//	freeaddrinfo(localinfo); // all done with this structure
	//}

	//return 1;
}

int EthernetClient::connect(IPAddress ip, uint16_t port)
{
	return connect(ip.toString().c_str(), port);
}

size_t EthernetClient::write(uint8_t b)
{
	return write(&b, 1);
}

size_t EthernetClient::write(const uint8_t *buf, size_t size)
{
	if (_sock == -1) {
		return 0;
	}
	size_t bytes = 0;
	while (size > 0) {
		int rc = ethernetWrapper.clientWrite(_sock, buf + bytes, size);
		if (rc == -1) {
			logError("send: %s\n", strerror(errno));
			close();
			break;
		}
		bytes += rc;
		size -= rc;
	}
	return bytes;
}

size_t EthernetClient::write(const char *str)
{
	if (str == NULL) {
		return 0;
	}
	return write((const uint8_t *)str, strlen(str));
}

size_t EthernetClient::write(const char *buffer, size_t size)
{
	if (size == 0) {
		return 0;
	}
	return write((const uint8_t *)buffer, size);
}

int EthernetClient::available()
{
	return ethernetWrapper.clientAvailable(_sock);
}

int EthernetClient::read()
{
	uint8_t b;
	if (read((unsigned char*)&b, 1) >= 0 ) {
		// read worked
		return b;
	} else {
		// No data available
		return -1;
	}
}

int EthernetClient::read(uint8_t *buf, size_t bytes)
{
	return ethernetWrapper.clientRead(_sock, buf, bytes);
}

int EthernetClient::peek()
{
	return ethernetWrapper.clientPeek(_sock);
}

void EthernetClient::flush()
{
	ethernetWrapper.clientFlush(_sock);
}

void EthernetClient::stop()
{
	if (_sock != -1) {
		ethernetWrapper.clientClose(_sock);
		_sock = -1;
	}
	return;

	if (_sock == -1) {
		return;
	}
	ethernetWrapper.clientStop(_sock);
	_sock = -1;

	//// attempt to close the connection gracefully (send a FIN to other side)
	//shutdown(_sock, SD_BOTH);


	//unsigned long startTime, curTime;
	//startTime = millis();

	//// wait up to a second for the connection to close
	//uint8_t s;
	//do {
	//	s = status();
	//	if (s == ETHERNETCLIENT_W5100_CLOSED) {
	//		break; // exit the loop
	//	}
	//	Sleep(1000);
	//	curTime = millis();
	//} while (curTime - startTime < 1000000);

	//// if it hasn't closed, close it forcefully
	//if (s != ETHERNETCLIENT_W5100_CLOSED) {
	//	closesocket(_sock);
	//}
}

uint8_t EthernetClient::status()
{
	if (_sock == -1) {
		return ETHERNETCLIENT_W5100_CLOSED;
	} else {
		return 	ethernetWrapper.clientStatus(_sock);
	}

	//struct tcp_info tcp_info;
	//int tcp_info_length = sizeof(tcp_info);

	//if ( getsockopt( _sock, SOL_TCP, TCP_INFO, (void *)&tcp_info,
	//                 (socklen_t *)&tcp_info_length ) == 0 ) {
	//	switch (tcp_info.tcpi_state) {
	//	case TCP_ESTABLISHED:
	//		return ETHERNETCLIENT_W5100_ESTABLISHED;
	//	case TCP_SYN_SENT:
	//		return ETHERNETCLIENT_W5100_SYNSENT;
	//	case TCP_SYN_RECV:
	//		return ETHERNETCLIENT_W5100_SYNRECV;
	//	case TCP_FIN_WAIT1:
	//	case TCP_FIN_WAIT2:
	//		return ETHERNETCLIENT_W5100_FIN_WAIT;
	//	case TCP_TIME_WAIT:
	//		return TCP_TIME_WAIT;
	//	case TCP_CLOSE:
	//		return ETHERNETCLIENT_W5100_CLOSED;
	//	case TCP_CLOSE_WAIT:
	//		return ETHERNETCLIENT_W5100_CLOSING;
	//	case TCP_LAST_ACK:
	//		return ETHERNETCLIENT_W5100_LAST_ACK;
	//	case TCP_LISTEN:
	//		return ETHERNETCLIENT_W5100_LISTEN;
	//	case TCP_CLOSING:
	//		return ETHERNETCLIENT_W5100_CLOSING;
	//	}
	//}
}

uint8_t EthernetClient::connected()
{
	return ethernetWrapper.clientConnected(_sock);
}

void EthernetClient::close()
{
	if (_sock != -1) {
		ethernetWrapper.clientClose(_sock);
		_sock = -1;
	}
}

void EthernetClient::bind(IPAddress ip)
{
	//_srcip = ip;
}

int EthernetClient::getSocketNumber()
{
	return _sock;
}

// the next function allows us to use the client returned by
// EthernetServer::available() as the condition in an if-statement.

EthernetClient::operator bool()
{
	return _sock != -1;
}

bool EthernetClient::operator==(const EthernetClient& rhs)
{
	return _sock == rhs._sock && _sock != -1 && rhs._sock != -1;
}

EthernetWrapper ethernetWrapper;
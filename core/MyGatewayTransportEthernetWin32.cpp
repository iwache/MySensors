/*
 * The MySensors Arduino library handles the wireless radio link and protocol
 * between your home built sensors/actuators and HA controller of choice.
 * The sensors forms a self healing radio network with optional repeaters. Each
 * repeater and gateway builds a routing tables in EEPROM which keeps track of the
 * network topology allowing messages to be routed to nodes.
 *
 * Created by Tomas Hozza <thozza@gmail.com>
 * Copyright (C) 2015  Tomas Hozza
 * Full contributor list: https://github.com/mysensors/Arduino/graphs/contributors
 *
 * Documentation: http://www.mysensors.org
 * Support Forum: http://forum.mysensors.org
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 */

#include "MyGatewayTransport.h"
#include "drivers/Win32/EthernetClient.h"
#include "drivers/Win32/EthernetServer.h"
#include "drivers/Win32/IPAddress.h"

// global variables
extern MyMessage _msgTmp;

#if defined(MY_CONTROLLER_IP_ADDRESS)
IPAddress _ethernetControllerIP(MY_CONTROLLER_IP_ADDRESS);
#endif

#if defined(MY_IP_ADDRESS)
IPAddress _ethernetGatewayIP(MY_IP_ADDRESS);
#if defined(MY_IP_GATEWAY_ADDRESS)
IPAddress _gatewayIp(MY_IP_GATEWAY_ADDRESS);
#endif /* End of MY_IP_GATEWAY_ADDRESS */
#if defined(MY_IP_SUBNET_ADDRESS)
IPAddress _subnetIp(MY_IP_SUBNET_ADDRESS);
#endif /* End of MY_IP_SUBNET_ADDRESS */
#endif /* End of MY_IP_ADDRESS */

uint16_t _ethernetGatewayPort = MY_PORT;
MyMessage _ethernetMsg;

#define ARRAY_SIZE(x)  (sizeof(x)/sizeof(x[0]))

typedef struct {
	char string[MY_GATEWAY_MAX_RECEIVE_LENGTH];
	uint8_t idx;
} inputBuffer;

#if defined(MY_GATEWAY_CLIENT_MODE)
#if defined(MY_USE_UDP)
EthernetUDP _ethernetServer;
#endif /* End of MY_USE_UDP */
#else /* Else part of MY_GATEWAY_CLIENT_MODE */
EthernetServer _ethernetServer(_ethernetGatewayPort, MY_GATEWAY_MAX_CLIENTS);
#endif /* End of MY_GATEWAY_CLIENT_MODE */

#if defined(MY_GATEWAY_CLIENT_MODE)
static inputBuffer inputString;
#if defined(MY_USE_UDP)
// Nothing to do here
#else
static EthernetClient client = EthernetClient();
#endif /* End of MY_USE_UDP */
#else /* Else part of MY_GATEWAY_CLIENT_MODE */
static EthernetClient clients[MY_GATEWAY_MAX_CLIENTS];
static bool clientsConnected[MY_GATEWAY_MAX_CLIENTS];
static inputBuffer inputString[MY_GATEWAY_MAX_CLIENTS];
#endif /* End of MY_GATEWAY_CLIENT_MODE */

bool gatewayTransportInit(void)
{
#if defined(MY_GATEWAY_CLIENT_MODE)
#if defined(MY_USE_UDP)
	_ethernetServer.begin(_ethernetGatewayPort);
#else /* Else part of MY_USE_UDP */
#if defined(MY_IP_ADDRESS)
	client.bind(_ethernetGatewayIP);
#endif /* End of MY_IP_ADDRESS */
#if defined(MY_CONTROLLER_URL_ADDRESS)
	if (client.connect(MY_CONTROLLER_URL_ADDRESS, MY_PORT)) {
#else
	if (client.connect(_ethernetControllerIP, MY_PORT)) {
#endif /* End of MY_CONTROLLER_URL_ADDRESS */
		GATEWAY_DEBUG(PSTR("GWT:TIN:ETH OK\n"));
		gatewayTransportSend(buildGw(_msgTmp, I_GATEWAY_READY).set(MSG_GW_STARTUP_COMPLETE));
		presentNode();
	} else {
		client.stop();
		GATEWAY_DEBUG(PSTR("!GWT:TIN:ETH FAIL\n"));
	}
#endif /* End of MY_USE_UDP */
#else /* Else part of MY_GATEWAY_CLIENT_MODE */
#if defined(MY_IP_ADDRESS)
	_ethernetServer.begin(_ethernetGatewayIP);
#else
	// we have to use pointers due to the constructor of EthernetServer
	_ethernetServer.begin();
#endif /* End of MY_IP_ADDRESS */
#endif /* End of MY_GATEWAY_CLIENT_MODE */

	return true;
}

bool gatewayTransportSend(MyMessage &message)
{
	int nbytes = 0;
	char *_ethernetMsg = protocolFormat(message);

	setIndication(INDICATION_GW_TX);

#if defined(MY_GATEWAY_CLIENT_MODE)
#if defined(MY_USE_UDP)
#if defined(MY_CONTROLLER_URL_ADDRESS)
	_ethernetServer.beginPacket(MY_CONTROLLER_URL_ADDRESS, MY_PORT);
#else
	_ethernetServer.beginPacket(_ethernetControllerIP, MY_PORT);
#endif /* End of MY_CONTROLLER_URL_ADDRESS */
	_ethernetServer.write(_ethernetMsg, strlen(_ethernetMsg));
	// returns 1 if the packet was sent successfully
	nbytes = _ethernetServer.endPacket();
#else /* Else part of MY_USE_UDP */
	if (!client.connected()) {
		client.stop();
#if defined(MY_CONTROLLER_URL_ADDRESS)
		if (client.connect(MY_CONTROLLER_URL_ADDRESS, MY_PORT)) {
#else
		if (client.connect(_ethernetControllerIP, MY_PORT)) {
#endif /* End of MY_CONTROLLER_URL_ADDRESS */
			GATEWAY_DEBUG(PSTR("GWT:TPS:ETH OK\n"));
			_w5100_spi_en(false);
			gatewayTransportSend(buildGw(_msgTmp, I_GATEWAY_READY).set(MSG_GW_STARTUP_COMPLETE));
			_w5100_spi_en(true);
			presentNode();
		} else {
			// connecting to the server failed!
			GATEWAY_DEBUG(PSTR("!GWT:TPS:ETH FAIL\n"));
			_w5100_spi_en(false);
			return false;
		}
	}
	nbytes = client.write((const uint8_t*)_ethernetMsg, strlen(_ethernetMsg));
#endif /* End of MY_USE_UDP */
#else /* Else part of MY_GATEWAY_CLIENT_MODE */
	// Send message to connected clients
	nbytes = _ethernetServer.write(_ethernetMsg);
#endif /* End of MY_GATEWAY_CLIENT_MODE */
	return (nbytes > 0);
}

#if defined(MY_USE_UDP)
// Nothing to do here
#else
#if !defined(MY_GATEWAY_CLIENT_MODE)
bool _readFromClient(uint8_t i)
{
	while (clients[i].connected() && clients[i].available()) {
		const char inChar = clients[i].read();
		if (inputString[i].idx < MY_GATEWAY_MAX_RECEIVE_LENGTH - 1) {
			// if newline then command is complete
			if (inChar == '\n' || inChar == '\r') {
				// Add string terminator and prepare for the next message
				inputString[i].string[inputString[i].idx] = 0;
				GATEWAY_DEBUG(PSTR("GWT:RFC:C=%" PRIu8 ",MSG=%s\n"), i, inputString[i].string);
				inputString[i].idx = 0;
				if (protocolParse(_ethernetMsg, inputString[i].string)) {
					return true;
				}

			} else {
				// add it to the inputString:
				inputString[i].string[inputString[i].idx++] = inChar;
			}
		} else {
			// Incoming message too long. Throw away
			GATEWAY_DEBUG(PSTR("!GWT:RFC:C=%" PRIu8 ",MSG TOO LONG\n"), i);
			inputString[i].idx = 0;
			// Finished with this client's message. Next loop() we'll see if there's more to read.
			break;
		}
	}
	return false;
}
#else /* Else part of !MY_GATEWAY_CLIENT_MODE */
bool _readFromClient(void)
{
	while (client.connected() && client.available()) {
		const char inChar = client.read();
		if (inputString.idx < MY_GATEWAY_MAX_RECEIVE_LENGTH - 1) {
			// if newline then command is complete
			if (inChar == '\n' || inChar == '\r') {
				// Add string terminator and prepare for the next message
				inputString.string[inputString.idx] = 0;
				GATEWAY_DEBUG(PSTR("GWT:RFC:MSG=%s\n"), inputString.string);
				inputString.idx = 0;
				if (protocolParse(_ethernetMsg, inputString.string)) {
					return true;
				}

			} else {
				// add it to the inputString:
				inputString.string[inputString.idx++] = inChar;
			}
		} else {
			// Incoming message too long. Throw away
			GATEWAY_DEBUG(PSTR("!GWT:RFC:MSG TOO LONG\n"));
			inputString.idx = 0;
			// Finished with this client's message. Next loop() we'll see if there's more to read.
			break;
		}
	}
	return false;
}
#endif /* End of !MY_GATEWAY_CLIENT_MODE */
#endif /* End of MY_USE_UDP */

bool gatewayTransportAvailable(void)
{
#if defined(MY_GATEWAY_CLIENT_MODE)
#if defined(MY_USE_UDP)
	int packet_size = _ethernetServer.parsePacket();

	if (packet_size) {
		//GATEWAY_DEBUG(PSTR("UDP packet available. Size:%" PRIu8 "\n"), packet_size);
		_ethernetServer.read(inputString.string, MY_GATEWAY_MAX_RECEIVE_LENGTH);
		inputString.string[packet_size] = 0;
		GATEWAY_DEBUG(PSTR("GWT:TSA:UDP MSG=%s\n"), inputString.string);
		const bool ok = protocolParse(_ethernetMsg, inputString.string);
		if (ok) {
			setIndication(INDICATION_GW_RX);
		}
		return ok;
	}
#else /* Else part of MY_USE_UDP */
	if (!client.connected()) {
		client.stop();
#if defined(MY_CONTROLLER_URL_ADDRESS)
		if (client.connect(MY_CONTROLLER_URL_ADDRESS, MY_PORT)) {
#else
		if (client.connect(_ethernetControllerIP, MY_PORT)) {
#endif /* End of MY_CONTROLLER_URL_ADDRESS */
			GATEWAY_DEBUG(PSTR("GWT:TSA:ETH OK\n"));
			gatewayTransportSend(buildGw(_msgTmp, I_GATEWAY_READY).set(MSG_GW_STARTUP_COMPLETE));
			presentNode();
		} else {
			GATEWAY_DEBUG(PSTR("!GWT:TSA:ETH FAIL\n"));
			return false;
		}
	}
	if (_readFromClient()) {
		setIndication(INDICATION_GW_RX);
		return true;
	}
#endif /* End of MY_USE_UDP */
#else /* Else part of MY_GATEWAY_CLIENT_MODE */
	// ESP8266: Go over list of clients and stop any that are no longer connected.
	// If the server has a new client connection it will be assigned to a free slot.
	bool allSlotsOccupied = true;
	for (uint8_t i = 0; i < ARRAY_SIZE(clients); i++) {
		if (!clients[i].connected()) {
			if (clientsConnected[i]) {
				GATEWAY_DEBUG(PSTR("GWT:TSA:C=%" PRIu8 ",DISCONNECTED\n"), i);
				clients[i].stop();
			}
			//check if there are any new clients
			if (_ethernetServer.hasClient()) {
				clients[i] = _ethernetServer.available();
				inputString[i].idx = 0;
				GATEWAY_DEBUG(PSTR("GWT:TSA:C=%" PRIu8 ",CONNECTED\n"), i);
				gatewayTransportSend(buildGw(_msgTmp, I_GATEWAY_READY).set(MSG_GW_STARTUP_COMPLETE));
				// Send presentation of locally attached sensors (and node if applicable)
				presentNode();
			}
		}
		bool connected = clients[i].connected() != 0;
		clientsConnected[i] = connected;
		allSlotsOccupied &= connected;
	}
	if (allSlotsOccupied && _ethernetServer.hasClient()) {
		//no free/disconnected spot so reject
		GATEWAY_DEBUG(PSTR("!GWT:TSA:NO FREE SLOT\n"));
		EthernetClient c = _ethernetServer.available();
		c.stop();
	}
	// Loop over clients connect and read available data
	for (uint8_t i = 0; i < ARRAY_SIZE(clients); i++) {
		if (_readFromClient(i)) {
			setIndication(INDICATION_GW_RX);
			return true;
		}
	}
#endif /* End of MY_GATEWAY_CLIENT_MODE */
	return false;
}

MyMessage& gatewayTransportReceive(void)
{
	// Return the last parsed message
	return _ethernetMsg;
}

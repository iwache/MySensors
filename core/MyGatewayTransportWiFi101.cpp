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

// global variables
extern MyMessage _msgTmp;

#if defined(MY_CONTROLLER_IP_ADDRESS)
IPAddress _ethernetControllerIP(MY_CONTROLLER_IP_ADDRESS);
#endif

uint16_t _ethernetGatewayPort = MY_PORT;
MyMessage _ethernetMsg;

#define ARRAY_SIZE(x)  (sizeof(x)/sizeof(x[0]))

typedef struct {
	// Suppress the warning about unused members in this struct because it is used through a complex
	// set of preprocessor directives
	// cppcheck-suppress unusedStructMember
	char string[MY_GATEWAY_MAX_RECEIVE_LENGTH];
	// cppcheck-suppress unusedStructMember
	uint8_t idx;
} inputBuffer;

// Some re-defines to make code more readable below
#define EthernetServer WiFiServer
#define EthernetClient WiFiClient
#define EthernetUDP WiFiUDP

#if defined(MY_GATEWAY_CLIENT_MODE)
#if defined(MY_USE_UDP)
EthernetUDP _ethernetServer;
#endif /* End of MY_USE_UDP */
#else /* Else part of MY_GATEWAY_CLIENT_MODE */
EthernetServer _ethernetServer(_ethernetGatewayPort);
#endif /* End of MY_GATEWAY_CLIENT_MODE */

#if defined(MY_GATEWAY_CLIENT_MODE)
static inputBuffer inputString;
#if defined(MY_USE_UDP)
// Nothing to do here
#else
static EthernetClient client = EthernetClient();
#endif /* End of MY_USE_UDP */
#else
static EthernetClient clients[MY_GATEWAY_MAX_CLIENTS];
static inputBuffer inputString[MY_GATEWAY_MAX_CLIENTS];
#endif /* End of MY_GATEWAY_CLIENT_MODE */

uint8_t wifi101Status = WL_IDLE_STATUS;

bool gatewayTransportInit(void)
{
	// check for the presence of the shield:
	if (WiFi.status() == WL_NO_SHIELD)
	{
		GATEWAY_DEBUG(PSTR("!GWT:TIN:NO WIFI101 SHIELD\n"));
		return false;  // don't continue
	}

#if defined(MY_IP_ADDRESS)
	IPAddress ethernetGatewayIP(MY_IP_ADDRESS);
#if defined(MY_IP_DNS_ADDRESS)
	IPAddress dnsIp(MY_IP_DNS_ADDRESS);
#if defined(MY_IP_GATEWAY_ADDRESS)
	IPAddress gatewayIp(MY_IP_GATEWAY_ADDRESS);
#if defined(MY_IP_SUBNET_ADDRESS)
	IPAddress subnetIp(MY_IP_SUBNET_ADDRESS);
	WiFi.config(ethernetGatewayIP, dnsIp, gatewayIp, subnetIp);
#else /* Else of MY_IP_SUBNET_ADDRESS */
	WiFi.config(ethernetGatewayIP, dnsIp, gatewayIp);
#endif /* End of MY_IP_SUBNET_ADDRESS */	
#else /* Else of MY_IP_GATEWAY_ADDRESS */
	WiFi.config(ethernetGatewayIP, dnsIp);
#endif /* End of MY_IP_GATEWAY_ADDRESS */	
#else /* Else of MY_IP_DNS_ADDRESS */
	WiFi.config(ethernetGatewayIP);
#endif /* Else of MY_IP_DNS_ADDRESS */	
#endif /* End of MY_IP_ADDRESS */	

	// attempt to connect to Wifi network:
	while (wifi101Status != WL_CONNECTED)
	{
		GATEWAY_DEBUG(PSTR("GWT:TIN:CONNECTING...\n"));
		// Connect to WPA/WPA2 network. Change this line if using open or WEP network:
		wifi101Status = WiFi.begin(MY_WIFI_SSID, MY_WIFI_PASSWORD);
		delay(10000);
	}
	IPAddress localIP = WiFi.localIP();
	GATEWAY_DEBUG(PSTR("GWT:TIN:IP=%" PRIu8 ".%" PRIu8 ".%" PRIu8 ".%" PRIu8 "\n"),
	              localIP[0], localIP[1], localIP[2], localIP[3]);

#if defined(MY_GATEWAY_CLIENT_MODE)
#if defined(MY_USE_UDP)
	_ethernetServer.begin(_ethernetGatewayPort);
#else /* Else part of MY_USE_UDP */
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
	// we have to use pointers due to the constructor of EthernetServer
	_ethernetServer.begin();
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
			gatewayTransportSend(buildGw(_msgTmp, I_GATEWAY_READY).set(MSG_GW_STARTUP_COMPLETE));
			presentNode();
		} else {
			// connecting to the server failed!
			GATEWAY_DEBUG(PSTR("!GWT:TPS:ETH FAIL\n"));
			return false;
		}
	}
	nbytes = client.write((const uint8_t*)_ethernetMsg, strlen(_ethernetMsg));
#endif /* End of MY_USE_UDP */
#else /* Else part of MY_GATEWAY_CLIENT_MODE */
	// Send message to connected clients
	for (uint8_t i = 0; i < ARRAY_SIZE(clients); i++) {
		if (clients[i] && clients[i].connected()) {
			nbytes += clients[i].write((uint8_t*)_ethernetMsg, strlen(_ethernetMsg));
		}
	}
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
	//check if there is a new client
	EthernetClient client = _ethernetServer.available();
	bool newclient = client;
	if (newclient) {
		// check if this new client is already connected
		for (uint8_t i = 0; i < ARRAY_SIZE(clients); i++) {
			if (clients[i] == client) {
				// this client was already connected
				newclient = false;
				break;
			}
		}
	}
	// Go over list of clients and stop any that are no longer connected.
	// If the server has a new client connection it will be assigned to a free slot.
	for (uint8_t i = 0; i < ARRAY_SIZE(clients); i++) {
		if (!clients[i] || !clients[i].connected()) {
			if (clients[i]) {
				GATEWAY_DEBUG(PSTR("GWT:TSA:C=%" PRIu8 ",DISCONNECTED\n"), i);
				clients[i].stop();
			}
			
			if (newclient) {
				clients[i] = client;
				// mark new client as assigned
				newclient = false;
				inputString[i].idx = 0;
				GATEWAY_DEBUG(PSTR("GWT:TSA:C=%" PRIu8 ",CONNECTED\n"), i);
				gatewayTransportSend(buildGw(_msgTmp, I_GATEWAY_READY).set(MSG_GW_STARTUP_COMPLETE));
				// Send presentation of locally attached sensors (and node if applicable)
				presentNode();
			}
		}
		bool connected = clients[i] && clients[i].connected();
	}
	if (newclient) {
		//no free/disconnected spot so reject
		GATEWAY_DEBUG(PSTR("!GWT:TSA:NO FREE SLOT\n"));
		client.stop();
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

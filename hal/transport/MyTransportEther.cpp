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
 ********************************************************************************/

#include "MyTransportHAL.h"
#include <EthernetClient.h>

// We only use SYS_PACK in this application
#define ETHER_PACKET 0x55
#define ETHER_ADDR 0xAA

// Receiving header information
unsigned char _header[7];

// Reception state machine control and storage variables
unsigned char _recPhase;
unsigned char _recPos;
unsigned char _recCommand;
unsigned char _recLen;
unsigned char _recStation;
unsigned char _recSender;
unsigned char _recNoAck;
unsigned char _recCS;
unsigned char _recCalcCS;

unsigned char _nodeId;
unsigned char _data[MAX_MESSAGE_LENGTH];
uint8_t _packet_len;
unsigned char _packet_from;
bool _packet_received;

// Packet wrapping characters, defined in standard ASCII table
#define SOH 1
#define STX 2
#define ETX 3
#define EOT 4

EthernetClient _dev;

#define ETHER_BUFFER_SIZE 64

unsigned char _sendBuffer[ETHER_BUFFER_SIZE];
unsigned char _sendBufferCount;

unsigned char _receiveBuffer[ETHER_BUFFER_SIZE];
unsigned char _receiveBufferLength;
unsigned char _receiveBufferPos;

void _sendBufferAdd(unsigned char val)
{
	_sendBuffer[_sendBufferCount++] = val;
}

unsigned char _receiveBufferGet()
{
	return _receiveBuffer[_receiveBufferPos++];
}

//Reset the state machine and release the data pointer
void _serialReset()
{
	_recPhase = 0;
	_recPos = 0;
	_recLen = 0;
	_recCommand = 0;
	_recNoAck = 0;
	_recCS = 0;
	_recCalcCS = 0;
}

// This is the main reception state machine.  Progress through the states
// is keyed on either special control characters, or counted number of bytes
// received.  If all the data is in the right format, and the calculated
// checksum matches the received checksum, AND the destination station is
// our station ID, then look for a registered command that matches the
// command code.  If all the above is true, execute the command's
// function.
bool _socketProcess()
{
	unsigned char i;

	if (_receiveBufferPos == _receiveBufferLength) {
		int available = _dev.available();

		if (available == 0) {
			return false;
		}

		if (available > ETHER_BUFFER_SIZE) {
			available = ETHER_BUFFER_SIZE;
		}
		_receiveBufferPos = 0;
		_receiveBufferLength = _dev.read(_receiveBuffer, available);
	}

	while (_receiveBufferPos < _receiveBufferLength) {
		unsigned char inch = _receiveBufferGet();

		switch (_recPhase) {
			// Case 0 looks for the header. Bytes arrive in the TCP client interface and get
			// shifted through a header buffer. When the start and end characters in
			// the buffer match the SOH/STX pair and valid Ether command, 
			// save the header information and progress to the next state.
		case 0:
			memcpy(&_header[0], &_header[1], 6);
			_header[6] = inch;
			if ((_header[0] == SOH) && (_header[1] == ETHER_PACKET) && (_header[6] == STX) 
				&& ((_header[2] == BROADCAST_ADDRESS) && (_header[3] == BROADCAST_ADDRESS) || (_header[2] != _header[3]))) {
				// Packet header received
				_recCalcCS = 0;
				_recCommand = _header[1];
				_recStation = _header[2];
				_recSender = _header[3];
				_recNoAck = _header[4];
				_recLen = _header[5];

				logDebug("Packet header received: from %u; len %u\n", _recSender, _recLen);

				for (i = 1; i < 6; i++) {
					_recCalcCS += _header[i];
				}
				_recPhase = 1;
				_recPos = 0;

				////Check if we should process this message
				////We reject the message if we are the sender
				////We reject if we are not the receiver and message is not a broadcast
				//if ((_recSender == _nodeId) ||
				//	(_recStation != _nodeId &&
				//		_recStation != BROADCAST_ADDRESS)) {
				//	logDebug("Packet process rejected: to %u; my node id %u\n", _recStation, _nodeId);

				//	_serialReset();
				//	break;
				//}

				if (_recLen == 0) {
					_recPhase = 2;
				}
			}
			break;

			// Case 1 receives the data portion of the packet.  Read in "_recLen" number
			// of bytes and store them in the _data array.
		case 1:
			_data[_recPos++] = inch;
			_recCalcCS += inch;
			if (_recPos == _recLen) {
				_recPhase = 2;
			}
			break;

			// After the data comes a single ETX character.  Do we have it?  If not,
			// reset the state machine to default and start looking for a new header.
		case 2:
			// Packet properly terminated?
			if (inch == ETX) {
				_recPhase = 3;
			} else {
				_serialReset();
			}
			break;

			// Next comes the checksum.  We have already calculated it from the incoming
			// data, so just store the incoming checksum byte for later.
		case 3:
			_recCS = inch;
			_recPhase = 4;
			break;

			// The final state - check the last character is EOT and that the checksum matches.
			// If that test passes, then look for a valid command callback to execute.
			// Execute it if found.
		case 4:
			if (inch == EOT) {
				if (_recCS == _recCalcCS) {
					_packet_from = _recSender;
					_packet_len = _recLen;
					_packet_received = true;

					logDebug("Packet accepted\n");

					//break;
				} else {
					logDebug("Packet has wrong checksum\n");
				}
			}
			//Clear the data
			_serialReset();
			//Return true, we have processed one command
			return true;
		}
	}
	return true;
}

bool transportSend(const uint8_t to, const void* data, const uint8_t len, const bool noACK)
{
	const char *datap = static_cast<char const *>(data);
	unsigned char i;
	unsigned char cs = 0;
	unsigned char noAck = noACK ? 0 : 1;

	logDebug("Transport send to: %u len: %u\n", to, len);

	_sendBufferCount = 0;
	_sendBufferAdd(SOH); // Start of header by writing SOH
	_sendBufferAdd(ETHER_PACKET); // ETHER Data packet telegram
	cs += ETHER_PACKET;
	_sendBufferAdd(to); // Destination address
	cs += to;
	_sendBufferAdd(_nodeId); // Source address
	cs += _nodeId;
	_sendBufferAdd(noAck); // Packet acknowledge flag
	cs += noAck;
	_sendBufferAdd(len); // Length of text
	cs += len;
	_sendBufferAdd(STX); // Start of text
	for (i = 0; i < len; i++) {
		_sendBufferAdd(datap[i]); // Text bytes
		cs += datap[i];
	}
	_sendBufferAdd(ETX); // End of text
	_sendBufferAdd(cs);
	_sendBufferAdd(EOT); // End of transmission
	_dev.write(_sendBuffer, _sendBufferCount);
	_dev.flush();

	return true;
}

bool transportInit(void)
{
	if (_dev.connected()) {
		_dev.flush();
		_dev.close();
		_serialReset();

		_nodeId = BROADCAST_ADDRESS;
	}

	if (_dev.connect(MY_ETHER_SERVER, MY_ETHER_SERVER_PORT)) {
		
		logDebug("Connected to My ETHER Server\n");
		
		_serialReset();
		return true;
	}
	return false;
}

void transportSetAddress(const uint8_t address) 
{
	_nodeId = address;

	logDebug("Send address to ETHER server: %u\n", _nodeId);

	_dev.write(SOH); // Start of header by writing SOH
	_dev.write(ETHER_ADDR); // Set ETHER address telegram
	_dev.write(_nodeId); // Source address
	_dev.write(EOT); // End of transmission
	_dev.flush();
}

uint8_t transportGetAddress(void)
{
	return _nodeId;
}

bool transportAvailable(void)
{
	_socketProcess();
	return _packet_received;
}

bool transportSanityCheck(void)
{
	// not implemented yet
	return true;
}

uint8_t transportReceive(void* data)
{
	if (_packet_received) {
		memcpy(data,_data,_packet_len);
		_packet_received = false;
		return _packet_len;
	} else {
		return (0);
	}
}

void transportPowerDown(void)
{
	// Nothing to shut down here
}

void transportPowerUp(void)
{
	// Nothing to power up here
}

void transportSleep(void)
{
	// not possible
}

void transportStandBy(void)
{
	// not possible
}

int16_t transportGetSendingRSSI(void)
{
	// not implemented
	return INVALID_RSSI;
}

int16_t transportGetReceivingRSSI(void)
{
	// not implemented
	return INVALID_RSSI;
}

int16_t transportGetSendingSNR(void)
{
	// not implemented
	return INVALID_SNR;
}

int16_t transportGetReceivingSNR(void)
{
	// not implemented
	return INVALID_SNR;
}

int16_t transportGetTxPowerPercent(void)
{
	// not implemented
	return static_cast<int16_t>(100);
}

int16_t transportGetTxPowerLevel(void)
{
	// not implemented
	return static_cast<int16_t>(100);
}

bool transportSetTxPowerPercent(const uint8_t powerPercent)
{
	// not possible
	(void)powerPercent;
	return false;
}

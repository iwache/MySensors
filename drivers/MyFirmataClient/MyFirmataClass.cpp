/*
  MyFirmataClass.cpp - ConfigurableFirmata library v2.10.0 - 2017-6-16
  Copyright (c) 2006-2008 Hans-Christoph Steiner.  All rights reserved.
  Copyright (c) 2013 Norbert Truchsess. All rights reserved.
  Copyright (c) 2013-2017 Jeff Hoefs. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  See file LICENSE.txt for further informations on licensing terms.
*/

//******************************************************************************
//* Includes
//******************************************************************************

#include "MyFirmataClass.h"
#if !defined(WIN32)
#include "HardwareSerial.h"
#endif

extern "C" {
#include <string.h>
#include <stdlib.h>
}

//******************************************************************************
//* Support Functions
//******************************************************************************

/**
 * Split a 16-bit byte into two 7-bit values and write each value.
 * @param value The 16-bit value to be split and written separately.
 */
void MyFirmataClass::sendValueAsTwo7bitBytes(int value)
{
  MyFirmataStream->write(value & 0x7F); // LSB
  MyFirmataStream->write(value >> 7 & 0x7F); // MSB
}

/**
 * A helper method to write the beginning of a Sysex message transmission.
 */
void MyFirmataClass::startSysex(void)
{
  MyFirmataStream->write(START_SYSEX);
}

/**
 * A helper method to write the end of a Sysex message transmission.
 */
void MyFirmataClass::endSysex(void)
{
  MyFirmataStream->write(END_SYSEX);
}

//******************************************************************************
//* Constructors
//******************************************************************************

/**
 * The Firmata class.
 * An instance named "Firmata" is created automatically for the user.
 */
MyFirmataClass::MyFirmataClass()
{
  firmwareVersionCount = 0;
  firmwareVersionVector = 0;

//  systemReset();
  parsingSysex = false;
}

//******************************************************************************
//* Public Methods
//******************************************************************************

/**
 * Initialize the default Serial transport at the default baud of 57600.
 */
void MyFirmataClass::begin(void)
{
  begin(57600);
}

/**
 * Initialize the default Serial transport and override the default baud.
 * Sends the protocol version to the host application followed by the firmware version and name.
 * blinkVersion is also called. To skip the call to blinkVersion, call Firmata.disableBlinkVersion()
 * before calling Firmata.begin(baud).
 * @param speed The baud to use. 57600 baud is the default value.
 */
void MyFirmataClass::begin(long speed)
{
  //Serial.begin(speed);
  //MyFirmataStream = &Serial;
}

/**
 * Reassign the Firmata stream transport.
 * @param s A reference to the Stream transport object. This can be any type of
 * transport that implements the Stream interface. Some examples include Ethernet, WiFi
 * and other UARTs on the board (Serial1, Serial2, etc).
 */
void MyFirmataClass::begin(Stream &s)
{
	MyFirmataStream = &s;
}


void MyFirmataClass::update()
{
	myFirmataExt.update();

	//	digitalInput.report();

	while (MyFirmata.available()) {
		MyFirmata.processInput();
	}

	// if (reporting.elapsed()) {
	// analogInput.report();
	// }
}

void MyFirmataClass::addFeature(MyFirmataFeature &capability)
{
	myFirmataExt.addFeature(capability);
}



/**
 * Send the Firmata protocol version to the Firmata host application.
 */
//void MyFirmataClass::printVersion(void)
//{
//  MyFirmataStream->write(REPORT_VERSION);
//  MyFirmataStream->write(FIRMATA_PROTOCOL_MAJOR_VERSION);
//  MyFirmataStream->write(FIRMATA_PROTOCOL_MINOR_VERSION);
//}

/**
 * Sends the firmware name and version to the Firmata host application. The major and minor version
 * numbers are the first 2 bytes in the message. The following bytes are the characters of the
 * firmware name.
 */
//void MyFirmataClass::printFirmwareVersion(void)
//{
//	byte i;
//
//	if (firmwareVersionCount) { // make sure that the name has been set before reporting
//		startSysex();
//		MyFirmataStream->write(REPORT_FIRMWARE);
//		MyFirmataStream->write(firmwareVersionVector[0]); // major version number
//		MyFirmataStream->write(firmwareVersionVector[1]); // minor version number
//		for (i = 2; i < firmwareVersionCount; ++i) {
//			sendValueAsTwo7bitBytes(firmwareVersionVector[i]);
//		}
//		endSysex();
//	}
//}

/**
 * Sets the name and version of the firmware. This is not the same version as the Firmata protocol
 * (although at times the firmware version and protocol version may be the same number).
 * @param name A pointer to the name char array
 * @param major The major version number
 * @param minor The minor version number
 */
void MyFirmataClass::setFirmwareNameAndVersion(const char *name, byte major, byte minor)
{
  const char *firmwareName;
  const char *extension;

  // parse out ".cpp" and "applet/" that comes from using __FILE__
  extension = strstr(name, ".cpp");
  firmwareName = strrchr(name, '/');

  if (!firmwareName) {
    // windows
    firmwareName = strrchr(name, '\\');
  }
  if (!firmwareName) {
    // user passed firmware name
    firmwareName = name;
  } else {
    firmwareName ++;
  }

  if (!extension) {
    firmwareVersionCount = (byte)strlen(firmwareName) + 2;
  } else {
    firmwareVersionCount = extension - firmwareName + 2;
  }

  // in case anyone calls setFirmwareNameAndVersion more than once
  free(firmwareVersionVector);

  firmwareVersionVector = (byte *) malloc(firmwareVersionCount + 1);
  firmwareVersionVector[firmwareVersionCount] = 0;
  firmwareVersionVector[0] = major;
  firmwareVersionVector[1] = minor;
  strncpy((char *)firmwareVersionVector + 2, firmwareName, firmwareVersionCount - 2);
}

//------------------------------------------------------------------------------
// Input Stream Handling

/**
 * A wrapper for Stream::available()
 * @return The number of bytes remaining in the input stream buffer.
 */
int MyFirmataClass::available(void)
{
  return MyFirmataStream->available();
}


void MyFirmataClass::two7bitArrayToStr(unsigned char *buffer, byte sysexBytesLength)
{
	byte bufferLength = sysexBytesLength / 2;
	byte i = 1;
	byte j = 0;
	while (j < bufferLength) {
		// The string length will only be at most half the size of the
		// stored input buffer so we can decode the string within the buffer.
		buffer[j] += (buffer[i] << 7);
		i++;
		j++;
		buffer[j] = buffer[i];
		i++;
	}
	// Make sure string is null terminated. This may be the case for data
	// coming from client libraries in languages that don't null terminate
	// strings.
	if (buffer[j - 1] != '\0') {
		buffer[j] = '\0';
	}
}

/**
 * Process incoming sysex messages. Handles REPORT_FIRMWARE and STRING_DATA internally.
 * Calls callback function for STRING_DATA and all other sysex messages.
 * @private
 */
void MyFirmataClass::processSysexMessage(void)
{
  switch (storedInputData[0]) { //first byte in buffer is command
    case REPORT_FIRMWARE:
//      printFirmwareVersion();
		two7bitArrayToStr(&storedInputData[3], sysexBytesRead - 3);
		logDebug("Firmware %s version %u.%u\n", &storedInputData[3], storedInputData[1], storedInputData[2]);
      break;
    case STRING_DATA:
		two7bitArrayToStr(&storedInputData[1], sysexBytesRead - 1);
		logDebug("STRING_DATA: %s\n", &storedInputData[1]);
		
		if (currentStringCallback) {
        byte bufferLength = (sysexBytesRead - 1) / 2;
        byte i = 1;
        byte j = 0;
        while (j < bufferLength) {
          // The string length will only be at most half the size of the
          // stored input buffer so we can decode the string within the buffer.
          storedInputData[j] = storedInputData[i];
          i++;
          storedInputData[j] += (storedInputData[i] << 7);
          i++;
          j++;
        }
        // Make sure string is null terminated. This may be the case for data
        // coming from client libraries in languages that don't null terminate
        // strings.
        if (storedInputData[j - 1] != '\0') {
          storedInputData[j] = '\0';
        }
        (*currentStringCallback)((char *)&storedInputData[0]);
      }
      break;
    default:
      if (currentSysexCallback)
        (*currentSysexCallback)(storedInputData[0], sysexBytesRead - 1, storedInputData + 1);
  }
}


/**
 * Read a single int from the input stream. If the value is not = -1, pass it on to parse(byte)
 */
void MyFirmataClass::processInput(void)
{
  int inputData = MyFirmataStream->read(); // this is 'int' to handle -1 when no data
  if (inputData != -1) {
    parse(inputData);
  }
}

/**
 * Parse data from the input stream.
 * @param inputData A single byte to be added to the parser.
 */
void MyFirmataClass::parse(byte inputData)
{
	int command;

	if (parsingSysex) {
		if (inputData == END_SYSEX) {
			//stop sysex byte
			parsingSysex = false;
			//fire off handler function
			processSysexMessage();
		}
		else {
			//normal data byte - add to buffer
			storedInputData[sysexBytesRead] = inputData;
			sysexBytesRead++;
		}
	}
	else if ((waitForData > 0) && (inputData < 128)) {
		waitForData--;
		storedInputData[waitForData] = inputData;
		if ((waitForData == 0) && executeMultiByteCommand) { // got the whole message
			switch (executeMultiByteCommand) {
			case ANALOG_MESSAGE:
				// handle received analog value from channel 0-15
				if (currentAnalogCallback) {
					(*currentAnalogCallback)(multiByteChannel,
						(storedInputData[0] << 7)
						+ storedInputData[1]);
				}
				break;
			case DIGITAL_MESSAGE:
				// handle received digital port value (14-bit) from channel 0-15
				if (currentDigitalCallback) {
					(*currentDigitalCallback)(multiByteChannel,
						(storedInputData[0] << 7)
						+ storedInputData[1]);
				}
				break;
				//case SET_PIN_MODE:
				//  setPinMode(storedInputData[1], storedInputData[0]);
				//  break;
				//case SET_DIGITAL_PIN_VALUE:
				//  if (currentPinValueCallback)
				//    (*currentPinValueCallback)(storedInputData[1], storedInputData[0]);
				//  break;
			//case REPORT_ANALOG:
			//	if (currentReportAnalogCallback)
			//		(*currentReportAnalogCallback)(multiByteChannel, storedInputData[0]);
			//	break;
			//case REPORT_DIGITAL:
			//	if (currentReportDigitalCallback)
			//		(*currentReportDigitalCallback)(multiByteChannel, storedInputData[0]);
			//	break;
			case REPORT_VERSION:
				//			setVersion(storedInputData[1], storedInputData[0]);
				////        Firmata.printVersion();
				logDebug("Firmata protocol version %u.%u\n", storedInputData[1], storedInputData[0]);
				break;
			default:
				break;
			}
			executeMultiByteCommand = 0;
		}
	} else {
		// remove channel info from command byte if less than 0xF0
		if (inputData < 0xF0) {
			command = inputData & 0xF0;
			multiByteChannel = inputData & 0x0F;
		}
		else {
			command = inputData;
			// commands in the 0xF* range don't use channel data
		}
		switch (command) {
		case ANALOG_MESSAGE:
		case DIGITAL_MESSAGE:
		case REPORT_VERSION:
//case SET_PIN_MODE:
//case SET_DIGITAL_PIN_VALUE:
			waitForData = 2; // two data bytes needed
			executeMultiByteCommand = command;
			break;
		case REPORT_ANALOG:
		case REPORT_DIGITAL:
			waitForData = 1; // one data byte needed
			executeMultiByteCommand = command;
			break;
		case START_SYSEX:
			parsingSysex = true;
			sysexBytesRead = 0;
			break;
//		case SYSTEM_RESET:
////        systemReset();
//			break;
		default:
			break;
		}
	}
}

/**
 * @return Returns true if the parser is actively parsing data.
 */
boolean MyFirmataClass::isParsingMessage(void)
{
  return (waitForData > 0 || parsingSysex);
}

/**
 * @return Returns true if the SYSTEM_RESET message is being executed
 */
boolean MyFirmataClass::isResetting(void)
{
  return resetting;
}

//------------------------------------------------------------------------------
// Output Stream Handling

/**
 * Send an analog message to the Firmata host application. The range of pins is limited to [0..15]
 * when using the ANALOG_MESSAGE. The maximum value of the ANALOG_MESSAGE is limited to 14 bits
 * (16384). To increase the pin range or value, see the documentation for the EXTENDED_ANALOG
 * message.
 * @param pin The analog pin to send the value of (limited to pins 0 - 15).
 * @param value The value of the analog pin (0 - 1024 for 10-bit analog, 0 - 4096 for 12-bit, etc).
 * The maximum value is 14-bits (16384).
 */
void MyFirmataClass::sendAnalog(byte pin, int value)
{
  // pin can only be 0-15, so chop higher bits
  MyFirmataStream->write(ANALOG_MESSAGE | (pin & 0xF));
  sendValueAsTwo7bitBytes(value);
}

/* (intentionally left out asterix here)
 * STUB - NOT IMPLEMENTED
 * Send a single digital pin value to the Firmata host application.
 * @param pin The digital pin to send the value of.
 * @param value The value of the pin.
 */
void MyFirmataClass::sendDigital(byte pin, int value)
{
  /* TODO add single pin digital messages to the protocol, this needs to
   * track the last digital data sent so that it can be sure to change just
   * one bit in the packet.  This is complicated by the fact that the
   * numbering of the pins will probably differ on Arduino, Wiring, and
   * other boards.  The DIGITAL_MESSAGE sends 14 bits at a time, but it is
   * probably easier to send 8 bit ports for any board with more than 14
   * digital pins.
   */

  // TODO: the digital message should not be sent on the serial port every
  // time sendDigital() is called.  Instead, it should add it to an int
  // which will be sent on a schedule.  If a pin changes more than once
  // before the digital message is sent on the serial port, it should send a
  // digital message for each change.

  //    if(value == 0)
  //        sendDigitalPortPair();
}


/**
 * Send an 8-bit port in a single digital message (protocol v2 and later).
 * Send 14-bits in a single digital message (protocol v1).
 * @param portNumber The port number to send. Note that this is not the same as a "port" on the
 * physical microcontroller. Ports are defined in order per every 8 pins in ascending order
 * of the Arduino digital pin numbering scheme. Port 0 = pins D0 - D7, port 1 = pins D8 - D15, etc.
 * @param portData The value of the port. The value of each pin in the port is represented by a bit.
 */
void MyFirmataClass::sendDigitalPort(byte portNumber, int portData)
{
  MyFirmataStream->write(DIGITAL_MESSAGE | (portNumber & 0xF));
  MyFirmataStream->write((byte)portData % 128); // Tx bits 0-6
  MyFirmataStream->write(portData >> 7);  // Tx bits 7-13
}

/**
 * Send a sysex message where all values after the command byte are packet as 2 7-bit bytes
 * (this is not always the case so this function is not always used to send sysex messages).
 * @param command The sysex command byte.
 * @param bytec The number of data bytes in the message (excludes start, command and end bytes).
 * @param bytev A pointer to the array of data bytes to send in the message.
 */
void MyFirmataClass::sendSysex(byte command, byte bytec, byte *bytev)
{
  byte i;
  startSysex();
  MyFirmataStream->write(command);
  for (i = 0; i < bytec; i++) {
    sendValueAsTwo7bitBytes(bytev[i]);
  }
  endSysex();
}

/**
 * Send a string to the Firmata host application.
 * @param command Must be STRING_DATA
 * @param string A pointer to the char string
 */
void MyFirmataClass::sendString(byte command, const char *string)
{
  sendSysex(command, (byte)strlen(string), (byte *)string);
}

/**
 * Send a string to the Firmata host application.
 * @param string A pointer to the char string
 */
void MyFirmataClass::sendString(const char *string)
{
	sendString(STRING_DATA, string);
}

/**
 * A wrapper for Stream::available().
 * Write a single byte to the output stream.
 * @param c The byte to be written.
 */
void MyFirmataClass::write(byte c)
{
	MyFirmataStream->write(c);
}

void MyFirmataClass::write(const uint8_t *buf, size_t size)
{
	MyFirmataStream->write(buf, size);
}

/**
 * Attach a generic sysex callback function to a command (options are: ANALOG_MESSAGE,
 * DIGITAL_MESSAGE, REPORT_ANALOG, REPORT DIGITAL, SET_PIN_MODE and SET_DIGITAL_PIN_VALUE).
 * @param command The ID of the command to attach a callback function to.
 * @param newFunction A reference to the callback function to attach.
 */
void MyFirmataClass::attach(byte command, callbackFunction newFunction)
{
  switch (command) {
    case ANALOG_MESSAGE: currentAnalogCallback = newFunction; break;
    case DIGITAL_MESSAGE: currentDigitalCallback = newFunction; break;
    case REPORT_ANALOG: currentReportAnalogCallback = newFunction; break;
    case REPORT_DIGITAL: currentReportDigitalCallback = newFunction; break;
    case SET_PIN_MODE: currentPinModeCallback = newFunction; break;
    case SET_DIGITAL_PIN_VALUE: currentPinValueCallback = newFunction; break;
  }
}

/**
 * Attach a callback function for the SYSTEM_RESET command.
 * @param command Must be set to SYSTEM_RESET or it will be ignored.
 * @param newFunction A reference to the system reset callback function to attach.
 */
void MyFirmataClass::attach(byte command, systemResetCallbackFunction newFunction)
{
  switch (command) {
    case SYSTEM_RESET: currentSystemResetCallback = newFunction; break;
  }
}

/**
 * Attach a callback function for the STRING_DATA command.
 * @param command Must be set to STRING_DATA or it will be ignored.
 * @param newFunction A reference to the string callback function to attach.
 */
void MyFirmataClass::attach(byte command, stringCallbackFunction newFunction)
{
  switch (command) {
    case STRING_DATA: currentStringCallback = newFunction; break;
  }
}

/**
 * Attach a generic sysex callback function to sysex command.
 * @param command The ID of the command to attach a callback function to.
 * @param newFunction A reference to the sysex callback function to attach.
 */
void MyFirmataClass::attach(byte command, sysexCallbackFunction newFunction)
{
  currentSysexCallback = newFunction;
}

/**
 * Detach a callback function for a specified command (such as SYSTEM_RESET, STRING_DATA,
 * ANALOG_MESSAGE, DIGITAL_MESSAGE, etc).
 * @param command The ID of the command to detatch the callback function from.
 */
void MyFirmataClass::detach(byte command)
{
  switch (command) {
    case SYSTEM_RESET: currentSystemResetCallback = NULL; break;
    case STRING_DATA: currentStringCallback = NULL; break;
    case START_SYSEX: currentSysexCallback = NULL; break;
    default:
      attach(command, (callbackFunction)NULL);
  }
}

/**
 * Detach a callback function for a delayed task when using FirmataScheduler
 * @see FirmataScheduler
 * @param newFunction A reference to the delay task callback function to attach.
 */
void MyFirmataClass::attachDelayTask(delayTaskCallbackFunction newFunction)
{
  delayTaskCallback = newFunction;
}

/**
 * Call the delayTask callback function when using FirmataScheduler. Must first attach a callback
 * using attachDelayTask.
 * @see FirmataScheduler
 * @param delay The amount of time to delay in milliseconds.
 */
void MyFirmataClass::delayTask(long delay)
{
  if (delayTaskCallback) {
    (*delayTaskCallback)(delay);
  }
}

/**
 * @param pin The pin to get the configuration of.
 * @return The configuration of the specified pin.
 */
byte MyFirmataClass::getPinMode(byte pin)
{
	return 0; // pinConfig[pin];
}

/**
 * Set the pin mode/configuration. The pin configuration (or mode) in Firmata represents the
 * current function of the pin. Examples are digital input or output, analog input, pwm, i2c,
 * serial (uart), etc.
 * @param pin The pin to configure.
 * @param config The configuration value for the specified pin.
 */
void MyFirmataClass::setPinMode(byte pin, byte config)
{
	MyFirmataStream->write(SET_PIN_MODE);
	MyFirmataStream->write(pin);
	MyFirmataStream->write(config);

	//if (pinConfig[pin] == PIN_MODE_IGNORE)
	//	return;
	//pinState[pin] = 0;
	//pinConfig[pin] = config;
	//if (currentPinModeCallback)
	//	(*currentPinModeCallback)(pin, config);
}

/**
 * @param pin The pin to get the state of.
 * @return The state of the specified pin.
 */
int MyFirmataClass::getPinState(byte pin)
{
	return 0; // pinState[pin];
}

/**
 * Set the pin state. The pin state of an output pin is the pin value. The state of an
 * input pin is 0, unless the pin has it's internal pull up resistor enabled, then the value is 1.
 * @param pin The pin to set the state of
 * @param state Set the state of the specified pin
 */
void MyFirmataClass::setPinState(byte pin, int state)
{
//	pinState[pin] = state;
}


// sysex callbacks
/*
 * this is too complicated for analogReceive, but maybe for Sysex?
 void MyFirmataClass::attachSysex(sysexFunction newFunction)
 {
 byte i;
 byte tmpCount = analogReceiveFunctionCount;
 analogReceiveFunction* tmpArray = analogReceiveFunctionArray;
 analogReceiveFunctionCount++;
 analogReceiveFunctionArray = (analogReceiveFunction*) calloc(analogReceiveFunctionCount, sizeof(analogReceiveFunction));
 for(i = 0; i < tmpCount; i++) {
 analogReceiveFunctionArray[i] = tmpArray[i];
 }
 analogReceiveFunctionArray[tmpCount] = newFunction;
 free(tmpArray);
 }
*/

//******************************************************************************
//* Private Methods
//******************************************************************************

// make one instance for the user to use
MyFirmataClass MyFirmata;

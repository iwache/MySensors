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

#ifndef GPIO_h
#define	GPIO_h

#define INPUT 0
#define OUTPUT 1
#define INPUT_PULLUP 2

#define ANALOG 3

#define LOW 0
#define HIGH 1

/**
 * @brief GPIO class
 */
class GPIOClass
{

public:
	/**
	 * @brief GPIOClass constructor.
	 */
	GPIOClass();
	/**
	 * @brief GPIOClass copy constructor.
	 */
	GPIOClass(const GPIOClass& other);
	/**
	 * @brief GPIOClass destructor.
	 */
	~GPIOClass();
	/**
	 * @brief Configures the specified pin to behave either as an input or an output.
	 *
	 * @param pin The number of the pin.
	 * @param mode INPUT or OUTPUT.
	 */
	void _pinMode(uint8_t pin, uint8_t mode);
	/**
	 * @brief Write a high or a low value for the given pin.
	 *
	 * @param pin number.
	 * @param value HIGH or LOW.
	 */
	void _digitalWrite(uint8_t pin, uint8_t value);
	/**
	 * @brief Reads the value from a specified pin.
	 *
	 * @param pin The number of the pin.
	 * @return HIGH or LOW.
	 */
	uint8_t _digitalRead(uint8_t pin);
	/**
	* @brief Reads the analog value from a specified analog pin.
	*
	* @param pin The number of the pin.
	* @return analog value
	*/
	uint16_t _analogRead(uint8_t pin);
	/**
	* @brief Write the analog value to a specified PWM pin.
	*
	* @param pin The number of the pin.
	* @param value analog
	*/
	void _analogWrite(uint8_t pin, uint16_t value);
	/**
	 * @brief Arduino compatibility function, returns the same given pin.
	 *
	 * @param pin The number of the pin.
	 * @return The same parameter pin number.
	 */
	uint8_t _digitalPinToInterrupt(uint8_t pin);
	/**
	 * @brief Overloaded assign operator.
	 *
	 */
	GPIOClass& operator=(const GPIOClass& other);

private:
#if defined(MY_FIRMATA_CLIENT)
	MyDigitalInputFirmata digitalInputFeature;
	MyDigitalOutputFirmata digitalOutputFeature;
	MyAnalogInputFirmata analogInputFeature;
	MyAnalogOutputFirmata analogOutputFeature;
#endif
};

extern GPIOClass GPIO;

#endif

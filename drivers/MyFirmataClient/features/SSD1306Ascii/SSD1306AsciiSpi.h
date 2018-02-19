/* Arduino SSD1306Ascii Library
 * Copyright (C) 2015 by William Greiman
 *
 * This file is part of the Arduino SSD1306Ascii Library
 *
 * This Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Arduino SSD1306Ascii Library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
/**
 * @file SSD1306AsciiSpi.h
 * @brief Class for hardware SPI displays.
 */
#ifndef SSD1306AsciiSpi_h
#define SSD1306AsciiSpi_h

#include "SSD1306Ascii.h"
#include "MySSD1306Firmata.cpp"

/**
 * @class SSD1306AsciiSpi
 * @brief Class for SPI displays on the hardware SPI bus.
 */
class SSD1306AsciiSpi : public SSD1306Ascii {
public:
	/**
	 * @brief Initialize the display controller.
	 *
	 * @param[in] dev A device initialization structure.
	 * @param[in] cs The display controller chip select pin.
	 * @param[in] dc The display controller cdata/command pin.
	 */
	void begin(const DevType* dev, uint8_t cs, uint8_t dc) {
		begin(dev, cs, dc, 255);

		//pinMode(m_cs, OUTPUT);
		//pinMode(m_dc, OUTPUT);
		//SPI.begin();
		//init(dev);
	}
	/**
	 * @brief Initialize the display controller.
	 *
	 * @param[in] dev A device initialization structure.
	 * @param[in] cs The display controller chip select pin.
	 * @param[in] dc The display controller cdata/command pin.
	 * @param[in] rst The display controller reset pin.
	 */
	void begin(const DevType* dev, uint8_t cs, uint8_t dc, uint8_t rst) {
		m_myFirmata.attachOled(cs, dc, rst);
		if (rst != 255) {
			reset(rst);
		}
		init(dev);
	}
	/**
	* @brief Reset the display controller.
	*
	* @param[in] rst Reset pin number.
	*/
	void reset(uint8_t rst) {
		m_myFirmata.resetOled();
	}

protected:
	void writeDisplay(uint8_t b, uint8_t mode) {
		if (m_bufferCount > 0)
		{
			m_buffer[m_bufferPtr++] = b;
			m_buffer[m_bufferPtr++] = mode;

			byte l = sizeof(m_buffer);

			if (m_bufferPtr >= sizeof(m_buffer))
				sendBuffer();
		}
		else {
			m_myFirmata.writeDisplay(b, mode);
////		return;

			Serial.print(b, HEX);
			Serial.print('-');
			Serial.print(mode, HEX);
			Serial.print(' ');
		}
	}

	void beginBuffer()
	{
		m_bufferCount++;
	}

	void endBuffer()
	{
		m_bufferCount--;
		if (m_bufferCount == 0)
			sendBuffer();
	}

	void sendBuffer()
	{
		m_myFirmata.writeDisplayBuffer(m_buffer, m_bufferPtr);

/*
		for (size_t i = 0; i < m_bufferPtr; i += 2)
		{
			uint8_t b = m_buffer[i];
			uint8_t mode = m_buffer[i + 1];

//			m_myFirmata.writeDisplay(b, mode);

			Serial.print(b, HEX);
			Serial.print('-');
			Serial.print(mode, HEX);
			Serial.print(' ');
		}
*/
		m_bufferPtr = 0;
	}

private:
	MySSD1306Firmata m_myFirmata;
	byte m_bufferCount;
	byte m_buffer[50];
	byte m_bufferPtr;
};

#include "SSD1306Ascii.cpp"

#endif  // SSD1306AsciiSpi_h

// Unknown macro for the Visual C++ compiler
#define __attribute__(x)

// Additional includes for the Visual C++ compiler
#include "drivers/Win32/compatibility.cpp"
#include "drivers/Win32/noniso.cpp"
#include "drivers/Win32/log.c"
#include "drivers/Win32/Print.cpp"
#include "drivers/Win32/Stream.cpp"
#include "drivers/Win32/IPAddress.cpp"
#include "drivers/Win32/Ethernet.cpp"
#include "drivers/Win32/EthernetUdp.cpp"
#include "drivers/Win32/EthernetClient.cpp"
#include "drivers/Win32/EthernetServer.cpp"
#include "drivers/Win32/SerialSimulator.cpp"
#include "drivers/Win32/SoftEeprom.cpp"
#include "drivers/Win32/SPIDEV.cpp"

#if defined(MY_FIRMATA_CLIENT)
#include "drivers/MyFirmataClient/MyFirmataClass.cpp"
#include "drivers/MyFirmataClient/MyEthernetClientStream.h"
#include "drivers/MyFirmataClient/MyFirmataExt.cpp"
#include "drivers/MyFirmataClient/MyEncoder7Bit.cpp"
#include "drivers/MyFirmataClient/MyDigitalInputFirmata.cpp"
#include "drivers/MyFirmataClient/MyDigitalOutputFirmata.cpp"
#include "drivers/MyFirmataClient/MyAnalogInputFirmata.cpp"
#include "drivers/MyFirmataClient/MyAnalogOutputFirmata.cpp"
#endif

#include "drivers/Win32/GPIO.cpp"

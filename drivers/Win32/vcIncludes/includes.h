// Unknown macro for the Visual C++ compiler
#define __attribute__(x)

// Additional includes for the Visual C++ compiler

#include "../Arduino.cpp"
#include "../noniso.cpp"
#include "../log.c"
#include "../Print.cpp"
#include "../Stream.cpp"
#include "../IPAddress.cpp"
#include "../Ethernet.cpp"
#include "../EthernetUdp.cpp"
#include "../EthernetClient.cpp"
#include "../EthernetServer.cpp"
#include "../SerialSimulator.cpp"
#include "../SoftEeprom.cpp"
#include "../SPIDEV.cpp"

#if defined(MY_FIRMATA_CLIENT)
#include "../../MyFirmataClient/myfirmatafeature.h"
#include "../../MyFirmataClient/MyFirmataClass.cpp"
#include "../../MyFirmataClient/MyFirmataExt.cpp"
#include "../../MyFirmataClient/MyEthernetClientStream.h"
#include "../../MyFirmataClient/MyEncoder7Bit.cpp"
#include "../../MyFirmataClient/MyDigitalInputFirmata.cpp"
#include "../../MyFirmataClient/MyDigitalOutputFirmata.cpp"
#include "../../MyFirmataClient/MyAnalogInputFirmata.cpp"
#include "../../MyFirmataClient/MyAnalogOutputFirmata.cpp"
#endif

#include "../GPIO.cpp"

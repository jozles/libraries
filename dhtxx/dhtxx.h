
/*   
    Interrupt-free, small memory vasting dhtxx interface (about 1320 bytes)
    this is working with any pin who accept :
           PULLUP mode, digitalRead and digitalWrite
    Detection of rising and falling edge of data may be delayed by 11uS 
    As the shortest serial bit is about 75uS long, concurrent isr should not be longer 
    than 63uS. This should not happen with usual Arduino core.

    getDht(uint8_t pin) return a status (codes below) and load values into   
    four bytes : dhtxx.temph, dhtxx.templ, dhtxx.humidh, dhtxx.humidl 
    (h for integer value, l for decimal)
    Depending of dht unit, decimal value may be unrelevant. Check datasheet.
    pin is the Arduino pin number used to connect DHT.
*/

#ifndef DHTXX_H_INCLUDED
#define DHTXX_H_INCLUDED

#include "Arduino.h"

#define humidh frame[0]
#define humidl frame[1]
#define temph frame[2]
#define templ frame[3]

class Dhtxx
{
    public:
        Dhtxx();
        float getDhtTemp(uint8_t pin);
        float getDhtHumid(uint8_t pin);
        int8_t getDht(uint8_t pin);
        uint8_t frame[5];
};

#endif // DHTXX_H_INCLUDED







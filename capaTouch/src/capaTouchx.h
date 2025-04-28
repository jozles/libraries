#ifndef CAPAT_H_INCLUDED
#define CAPAT_H_INCLUDED

#include "Arduino.h"

#if defined(__AVR__)
#define PIN_TO_BASEREG(pin)             (portInputRegister(digitalPinToPort(pin)))
#define PIN_TO_BITMASk(pin)             (digitalPinToBitMask(pin))
#define IO_REG_TYPE uint8_t
#define DIRECT_READ(base, mask)         (((*(base)) & (mask)) ? 1 : 0)
#define DIRECT_MODE_INPUT(base, mask)   ((*((base)+1)) &= ~(mask), (*((base)+2)) &= ~(mask))
#define DIRECT_MODE_OUTPUT(base, mask)  ((*((base)+1)) |= (mask))
#define DIRECT_WRITE_LOW(base, mask)    ((*((base)+2)) &= ~(mask))
#define DIRECT_WRITE_HIGH(base, mask)   ((*((base)+2)) |= (mask))
#endif

#if defined(__SAM3X8E__)
#define PIN_TO_BASEREG(pin)             (&(digitalPinToPort(pin)->PIO_PER))
#define PIN_TO_BITMASK(pin)             (digitalPinToBitMask(pin))
#define IO_REG_TYPE uint32_t
#define IO_REG_ASM
#define DIRECT_READ(base, mask)         (((*((base)+15)) & (mask)) ? 1 : 0)
#define DIRECT_MODE_INPUT(base, mask)   ((*((base)+5)) = (mask))
#define DIRECT_MODE_OUTPUT(base, mask)  ((*((base)+4)) = (mask))
#define DIRECT_WRITE_LOW(base, mask)    ((*((base)+13)) = (mask))
#define DIRECT_WRITE_HIGH(base, mask)   ((*((base)+12)) = (mask))
#endif

#if defined(ARDUINO_ARCH_ESP8266)
#define PIN_TO_BASEREG(pin) ((volatile uint32_t*) GPO)
#define PIN_TO_BITMASK(pin) (1 << pin)
#define IO_REG_TYPE uint32_t
#define IO_REG_ASM
#define DIRECT_READ(base, mask) ((GPI & (mask)) ? 1 : 0) //GPIO_IN_ADDRESS
#define DIRECT_MODE_INPUT(base, mask) (GPE &= ~(mask)) //GPIO_ENABLE_W1TC_ADDRESS
#define DIRECT_MODE_OUTPUT(base, mask) (GPE |= (mask)) //GPIO_ENABLE_W1TS_ADDRESS
#define DIRECT_WRITE_LOW(base, mask) (GPOC = (mask)) //GPIO_OUT_W1TC_ADDRESS
#define DIRECT_WRITE_HIGH(base, mask) (GPOS = (mask)) //GPIO_OUT_W1TS_ADDRESS
#endif

#if defined(ARDUINO_ARCH_ESP32)
#include <driver/rtc_io.h>
#define PIN_TO_BASEREG(pin)             (0)
#define PIN_TO_BITMASK(pin)             (pin)
#define IO_REG_TYPE uint32_t
#define IO_REG_BASE_ATTR
#define IO_REG_MASK_ATTR
#endif


#define MAXKEY      2           // greater or equal to cntNb !!
#define NBCNTMEM    0x0fff      // 16     // puissance de 2 !!
#define CALIB       16          // NBCNTMEM  // nbre de count() en calibration
#define DEBOUNCE    60
#define CHGSTROBE   0xffff      // millis() strobe mean's update to protect from near finger
#define MAXCOUNT    0x0fff      // avoid wd tO

class Capat{
    public:
        Capat();
        void init(uint8_t samples,uint8_t common,uint8_t* touch,uint8_t cntNb);
        void calibrate();
        void count();
        void capaKeysCheck();
        uint8_t capaKeysGet(uint8_t keyNum);

        uint16_t highTresh[MAXKEY];                 // key high treshold
        uint16_t lowTresh[MAXKEY];                  // key low treshold
        uint16_t cntk[MAXKEY];                      // key cnt
        uint16_t meank[MAXKEY];                     // off key mean cnt value
        uint32_t totalMem[MAXKEY];                  // key total cnt value
        bool     keyVal[MAXKEY];                    // off/on
        bool     keyChge[MAXKEY];                   // last update key change
        uint32_t keyChSt[MAXKEY];                   // key change time

    private:
        void     meanUpdate();

        uint8_t  samples;                           // samples nbr to perform
        uint8_t  keyNb;                             // nbr of active keys
        //uint16_t cntMem[NBCNTMEM*MAXKEY];           // all keys cnt memorys for mean calculation
        
        uint8_t  prevCntMem[MAXKEY];                // key prev ptr in cntMem
        uint32_t debounce[MAXKEY];                  // key last millis
        
        IO_REG_TYPE	sBit;
        volatile IO_REG_TYPE *sReg; 

        IO_REG_TYPE	rBit[MAXKEY];
        volatile IO_REG_TYPE* rReg[MAXKEY];
        
};

#endif // DS18X20_H_INCLUDED



#ifndef _RADIO_PWRSLP_H_INCLUDED
#define _RADIO_PWRSLP_H_INCLUDED

#include "Arduino.h"

//#ifdef MACHINE_DET328

#ifdef ATMEGA8
#define WDTCSR WDTCR
#define EIFR   GIFR
#define TIMSK1 TIMSK
#endif //


// prescaler WDT
#define NB_PRESCALER_VALUES 10
#define T16   0b00000000
#define T32   0b00000001
#define T64   0b00000010
#define T125  0b00000011
#define T250  0b00000100
#define T500  0b00000101
#define T1000 0b00000110
#define T2000 0b00000111
#define T4000 0b00100000
#define T8000 0b00100001

// effective Time @8MHz (*10uS)
#define EMPTY_LOOP 18
#define RT16    2300
#define RT32    4300
#define RT64    8200
#define RT125   16100
#define RT250   31900
#define RT500   63400
#define RT1000  123500
#define RT2000  253000
#define RT4000  507200
#define RT8000  1010000

// effective time without counter @8MHz (*10uS)
#define LT16    2315
#define LT32    4284
#define LT64    8222
#define LT125   16110
#define LT250   31902
#define LT500   63400
#define LT1000  123474
#define LT2000  252974
#define LT4000  507174
#define LT8000  1009974

/* --------------- @ env 2sec / sleep -------------------- */
#define  AWAKE_OK_VALUE       2     // nbre réveils entre chaque test de temp 
#define  AWAKE_MIN_VALUE      10    // nbre réveils maxi pour message minimum de présence
#define  AWAKE_KO_VALUE       120   // debug 1500 // nbre réveils avant prochain test si com HS
#define  AWAKE_RETRY_VALUE    3     // nbre de retry avant KO

void sleepPwrDown(uint8_t durat);
uint8_t sleepPwrDownV(int32_t durat,int32_t* slpt);
void sleepStdby(int32_t durat);
void lethalSleep();
void checkOn();
void checkOff();
void wd();
void getVolts();
uint16_t adcRead0(uint8_t admuxval,uint8_t dly);
float adcRead(uint8_t admuxval,float factor, uint16_t offset, uint8_t ref,uint8_t dly);

//#endif // MACHINE_DET328

#endif // _RADIO_PWRSLP_H_INCLUDED

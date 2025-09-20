#ifndef _RADIO_PWRSLP_H_INCLUDED
#define _RADIO_PWRSLP_H_INCLUDED

#include "Arduino.h"

//#ifdef MACHINE_DET328

#ifdef ATMEGA8
#define WDTCSR WDTCR
#define EIFR   GIFR
#define TIMSK1 TIMSK
#endif //

// TPL adust (10sec)
#define PER_ADJUST -1

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

// timings indices
#define ST16   NB_PRESCALER_VALUES-1
#define ST32   NB_PRESCALER_VALUES-2
#define ST64   NB_PRESCALER_VALUES-3
#define ST125  NB_PRESCALER_VALUES-4
#define ST250  NB_PRESCALER_VALUES-5
#define ST500  NB_PRESCALER_VALUES-6
#define ST1000 NB_PRESCALER_VALUES-7
#define ST2000 NB_PRESCALER_VALUES-8
#define ST4000 NB_PRESCALER_VALUES-9
#define ST8000 NB_PRESCALER_VALUES-10

// effective Time @8MHz (*10uS)
#define CKRSTDLY 27                // oscilator restart time

#define EMPTY_LOOP 18
#define RT16    1900
#define RT32    3500
#define RT64    6700
#define RT125   12800
#define RT250   25300
#define RT500   50300
#define RT1000  100300
#define RT2000  200300
#define RT4000  500300
#define RT8000  800300

// effective time without counter @8MHz (*10uS)
#define LT16    1600
#define LT32    3200
#define LT64    6400
#define LT125   12500
#define LT250   25000
#define LT500   50000
#define LT1000  100000
#define LT2000  200000
#define LT4000  400000
#define LT8000  800000

/* --------------- @ env 2sec / sleep -------------------- */
#define  AWAKE_OK_VALUE       2     // nbre réveils entre chaque test de temp 
#define  AWAKE_MIN_VALUE      10    // nbre réveils maxi pour message minimum de présence
#define  AWAKE_KO_VALUE       120   // debug 1500 // nbre réveils avant prochain test si com HS
#define  AWAKE_RETRY_VALUE    3     // nbre de retry avant KO

void sleepPwrDown(uint8_t durat);
uint8_t sleepPwrDownV(int32_t durat,int32_t* slpt,bool sleep);
uint8_t sleepPwrDownV(int32_t durat,int32_t* slpt);
uint8_t sleepPwrDownV(int32_t durat);
void sleepStdby(int32_t durat);
void calibratePwrDown();
void sleepStdby(int32_t durat);
void lethalSleep();
void checkOn();
void checkOff();
void wd();
//void getVolts();
//void getVolts(float vfactor,float thfactor);
void getVolts(float vfactor,float thfactor,float* vt,float* th) ;
uint16_t adcRead0(uint8_t admuxval,uint8_t dly);
float adcRead(uint8_t admuxval,float factor, uint16_t offset, uint8_t ref,uint8_t dly);

//#endif // MACHINE_DET328

#endif // _RADIO_PWRSLP_H_INCLUDED

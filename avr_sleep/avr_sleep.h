#ifndef AVRSLEEP_H_INCLUDED
#define AVRSLEEP_H_INCLUDED

#include <Arduino.h>

// prescaler WDT
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

#define  STEP_VALUE 8               // nbre sec par période Sleep
#define  AWAKE_OK_VALUE  5 //15     // 120 sec entre chaque test de temp
#define  AWAKE_MIN_VALUE 15 //111   // environ 15 min pour message minimum de présence
#define  AWAKE_KO_VALUE  450        // 1 heure avant prochain test si com HS
#define  AWAKE_RETRY_VALUE     3    // nbre de retry avant KO

uint16_t sleepPwrDown(uint8_t durat);

#endif // AVRSLEEP_H_INCLUDED

#ifndef _NRF_CONST_INCLUDED
#define _NRF_CONST_INCLUDED

#include "radio_const.h"


#define POWONDLY 100+10   // millis()  - 100 -
#define POWUPDLY 5+1      // millis()
#define LORA_ADDR_LENGTH RADIO_ADDR_LENGTH
#define MAX_PAYLOAD_LENGTH 32

#define TO_REGISTER  20   // millis()

#define ACK     true      // compat nrf
#define NO_ACK  false     // compat nrf

//  #define RF_SPEED   RF_SPD_1MB // vitesse radio  RF_SPD_2MB // RF_SPD_1MB // RF_SPD_250K
//  #define ARD_VALUE  0          // ((0-15)+1) x 250uS delay before repeat
//  #define ARC_VALUE  4          // (0-15) repetitions

#endif // _NRF_CONST_INCLUDED

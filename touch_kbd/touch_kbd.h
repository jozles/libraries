#ifndef _TOUCH_KBD_H_
#define _TOUCH_KBD_H_

#include <Arduino.h>

#define CLEAR_KBD true

class TKbd 
{
    public:
        TKbd();
        uint16_t* kXPos;
        uint16_t* kYPos;
        uint16_t* kWidth;
        uint16_t* kHeight;
        uint8_t*  kTxt;
        uint8_t*  kCTxt;
        uint8_t   kCTl;

        uint8_t keyNb;
        
        void init(uint8_t kn,uint16_t* kxp,uint16_t* kyp,uint16_t* kw,uint16_t* kh,uint8_t* kt,uint8_t* kct,uint8_t kctl);
        void showKbd(uint16_t xk,uint16_t yk,uint16_t wk,uint16_t hk,bool clear);
        char tKbdGet(uint8_t* kbd);
};

#endif //  _TOUCH_KBD_H_
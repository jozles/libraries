#ifndef _TOUCH_KBD_H_
#define _TOUCH_KBD_H_

#include <Arduino.h>

#define NEW_KBD true
#define MAX_KBD_TIME 120000  // 2 minutes
#define MAX_KEY_NB 45

// ******* special keys *******

#define KEY_SHIFT   0x00
#define KEY_BACKSP  0x01
#define KEY_ENTER   0x02
#define KEY_SPACE   0x03
#define KEY_123     0x04
#define KEY_abc     0x05
#define KEY_TIMOUT  0x1f

#define SPACE 0x20

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
        char* kbd;
        
        void init(uint8_t kn,uint16_t* kxp,uint16_t* kyp,uint16_t* kw,uint16_t* kh,uint8_t* kt,uint8_t* kct,uint8_t kctl);
        void showKbd(uint16_t xk,uint16_t yk,uint16_t wk,uint16_t hk,bool clear);
        char getKbd(uint16_t xk,uint16_t yk,uint16_t wk,uint16_t hk,bool kclear);

};

#endif //  _TOUCH_KBD_H_
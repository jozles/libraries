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
#define KEY_Valid   0x00
#define KEY_Modif   0x01
#define KEY_Aband   0x02
#define KEY_TIMOUT  0x1f

#define SPACE 0x20

class TKbd 
{
    public:
        TKbd();

        uint8_t keyNb;
        char* currkbd;
        
        void init(uint8_t keyNb);
        void showKbd(uint16_t xk,uint16_t yk,uint16_t wk,uint16_t hk,bool clear,char* kbd,uint8_t keyLines,uint8_t keyCol,char* ksc,uint8_t skl);
        char getKbd(uint16_t xk,uint16_t yk,uint16_t wk,uint16_t hk,bool kclear);
        char getKbd0(uint16_t xk,uint16_t yk,uint16_t wk,uint16_t hk,bool kclear,char* kbd,uint8_t lines,uint8_t cols,char* ksc,uint8_t skl);
        char getVma();

};

#endif //  _TOUCH_KBD_H_
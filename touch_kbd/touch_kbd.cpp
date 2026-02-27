#include <Arduino.h>
#include <stdio.h>
#include <TFT_eSPI.h>
#include <touch_kbd.h>
#include "tt_const.h"

extern TFT_eSPI my_lcd;



TKbd::TKbd(uint8_t kn,uint16_t* kxp,uint16_t* kyp,uint16_t* kw,uint16_t* kh,uint8_t* kt,uint8_t* kct,uint8_t kctl){
    keyNb=kn;
    kXPos=kxp;
    kYPos=kyp;
    kWidth=kw;
    kHeight=kh;
    kTxt=kt;
    kCTxt=kct;
    kCTl=kctl;
}

void TKbd::showKbd(uint16_t xk,uint16_t yk,uint16_t wk,uint16_t hk,bool kclear){
    if(kclear){my_lcd.fillRect(xk,yk,wk,hk,BLACK);}
    my_lcd.setTextColor(WHITE, BLACK);
    uint8_t r=my_lcd.getRotation();
    my_lcd.setRotation(3);                  // paysage

    for(uint8_t i=0;i<keyNb;i++){
        my_lcd.drawRoundRect(kXPos[i]+xk,kYPos[i]+yk,kWidth[i],kHeight[i],2, WHITE);
    }
    my_lcd.setRotation(r);
}
#include <Arduino.h>
#include <stdio.h>
#include <TFT_eSPI.h>
#include <touch_kbd.h>
#include "tt_const.h"

extern TFT_eSPI my_lcd;

TKbd::TKbd(){
    keyNb=0;
    kXPos=nullptr;
    kYPos=nullptr;
    kWidth=nullptr;
    kHeight=nullptr;
    kTxt=nullptr;
    kCTxt=nullptr;
    kCTl=0;

}

void TKbd::init(uint8_t kn,uint16_t* kxp,uint16_t* kyp,uint16_t* kw,uint16_t* kh,uint8_t* kt,uint8_t* kct,uint8_t kctl){
    keyNb=kn;       
    kXPos=kxp;  // table keys xpos
    kYPos=kyp;  // table keys ypos
    kWidth=kw;  // table keys width
    kHeight=kh; // table keys height
    kTxt=kt;    // table keys content
    kCTxt=kct;  // table keys content if ctl
    kCTl=kctl;
}

// xposition,yposition,width,height,clearscreen before
void TKbd::showKbd(uint16_t xk,uint16_t yk,uint16_t wk,uint16_t hk,bool kclear){
    if(kclear){my_lcd.fillRect(xk,yk,wk,hk,BLACK);}
    my_lcd.setTextColor(WHITE, BLACK);
    uint8_t r=my_lcd.getRotation();
    my_lcd.setRotation(3);                  // paysage

    if(kXPos!=nullptr){                     // descripteur "extensif"
        for(uint8_t i=0;i<keyNb;i++){
            my_lcd.drawRoundRect(kXPos[i]+xk,kYPos[i]+yk,kWidth[i],kHeight[i],2, WHITE);
        }
    } else {
        uint8_t keyLines=5;
        uint8_t keyCol=10;
        uint16_t bkw=wk/keyCol;
        uint16_t bkh=hk/keyLines;
        uint16_t ox=xk;
        uint16_t oy=yk;

        for(uint8_t l=0;l<keyLines;l++){
            uint8_t cx=keyCol;
            switch(l){
                case 0:
                case 1: 
                case 2: ox=xk;
                        for(uint8_t c=0;c<keyCol;c++){my_lcd.drawRoundRect(ox+c*bkw,oy,bkw,bkh,2, WHITE);}
                        oy+=bkh;        
                        break;
                case 3: ox=xk;
                        my_lcd.drawRoundRect(ox,oy,bkw*1.5,bkh,2,WHITE);
                        ox+=bkw*1.5;
                        for(uint8_t c=0;c<keyCol-3;c++){
                            my_lcd.drawRoundRect(ox,oy,bkw,bkh,2, WHITE);
                            ox+=bkw;
                        }
                        my_lcd.drawRoundRect(ox,oy,bkw*1.5,bkh,2,WHITE);
                        oy+=bkh;        
                        break;
                case 4: ox=xk;
                        my_lcd.drawRoundRect(ox,oy,bkw*1.5,bkh,2,WHITE);
                        ox+=bkw*1.5;
                        my_lcd.drawRoundRect(ox,oy,bkw,bkh,2, WHITE);
                        ox+=bkw;
                        my_lcd.drawRoundRect(ox,oy,bkw*5,bkh,2, WHITE);
                        ox+=bkw*5;
                        my_lcd.drawRoundRect(ox,oy,bkw,bkh,2, WHITE);
                        ox+=bkw;
                        my_lcd.drawRoundRect(ox,oy,bkw*1.5,bkh,2,WHITE);
                default: break;                        
            }
            
        }

    }



    my_lcd.setRotation(r);
}


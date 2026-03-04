#include <Arduino.h>
#include <stdio.h>
#include <TFT_eSPI.h>
#include <TFT_Touch.h>
#include <touch_kbd.h>
#include "tt_const.h"

extern TFT_eSPI my_lcd;
extern TFT_Touch my_touch;


char* kU={"0123456789AZERTYUIOPQSDFGHJKLM_WXCVBN?__. !_"};
char* kL={"0123456789azertyuiopqsdfghjklm_wxcvbn?__. !_"};
char* kV={"0123456789@#&()+-/*=,;:'\"%{}[]_$\\^~|?.__< >_"};
char* kS={" Shift\0BackSp\0 Enter\0 Space\0  123 \0  abc \0"};
#define SKEY_LEN 7
//#define LKS SKEY_LEN*6

char kbdU[MAX_KEY_NB];
char kbdL[MAX_KEY_NB];
char kbdV[MAX_KEY_NB];

uint16_t xp[MAX_KEY_NB];
uint16_t yp[MAX_KEY_NB];
uint16_t wp[MAX_KEY_NB];
uint16_t hp[MAX_KEY_NB];

TKbd::TKbd(){
    keyNb=0;
    kXPos=nullptr;
    kYPos=nullptr;
    kWidth=nullptr;
    kHeight=nullptr;
    kTxt=nullptr;
    kCTxt=nullptr;
    kCTl=0;
    kbd=kbdL;
}

void TKbd::init(uint8_t kn,uint16_t* kxp,uint16_t* kyp,uint16_t* kw,uint16_t* kh,uint8_t* kt,uint8_t* kct,uint8_t kctl){
    keyNb=kn;if(keyNb>MAX_KEY_NB){keyNb=MAX_KEY_NB;}      
    kXPos=kxp;  // table keys xpos
    kYPos=kyp;  // table keys ypos
    kWidth=kw;  // table keys width
    kHeight=kh; // table keys height
    kTxt=kt;    // table keys content
    kCTxt=kct;  // table keys content if ctl
    kCTl=kctl;
    memcpy(kbdL,kL,keyNb);kbdL[30]=KEY_SHIFT;kbdL[38]=KEY_BACKSP;kbdL[39]=KEY_123;kbdL[41]=KEY_SPACE;kbdL[43]=KEY_ENTER;
    memcpy(kbdU,kU,keyNb);kbdU[30]=KEY_SHIFT;kbdU[38]=KEY_BACKSP;kbdU[39]=KEY_123;kbdU[41]=KEY_SPACE;kbdU[43]=KEY_ENTER;
    memcpy(kbdV,kV,keyNb);kbdV[30]=KEY_SHIFT;kbdV[38]=KEY_BACKSP;kbdV[39]=KEY_abc;kbdV[41]=KEY_SPACE;kbdV[43]=KEY_ENTER;
}

// xposition,yposition,width,height,clearscreen before
void TKbd::showKbd(uint16_t xk,uint16_t yk,uint16_t wk,uint16_t hk,bool kclear){
    if(!kclear){return;}
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
        uint8_t kptr=0;
        uint8_t dx=10,dy=4,dxsk=2,dysk=8;

        for(uint8_t l=0;l<keyLines;l++){
            uint8_t cx=keyCol;
            switch(l){
                case 0:
                case 1: 
                case 2: ox=xk;
                        for(uint8_t c=0;c<keyCol;c++){
                            my_lcd.drawRoundRect(ox+c*bkw,oy,bkw,bkh,2, WHITE);
                            my_lcd.drawChar(kbd[kptr],ox+c*bkw+dx,oy+dy,2);
                            // store coordinates 
                            xp[kptr]=ox+c*bkw;
                            yp[kptr]=oy;
                            wp[kptr]=bkw;
                            hp[kptr]=bkh;
                            kptr++;
                        }
                        oy+=bkh;        
                        break;
                case 3: ox=xk;
                        my_lcd.drawRoundRect(ox,oy,bkw*1.5,bkh,2,WHITE);
                        if(kbd[kptr]<0x20){
                            my_lcd.drawString(&kS[kbd[kptr]*SKEY_LEN],ox+dxsk,oy+dysk,1); 
                        }
                        else my_lcd.drawChar(kbd[kptr],ox+dx,oy+dy,2);
                        // store coordinates 
                        xp[kptr]=ox;
                        yp[kptr]=oy;
                        wp[kptr]=bkw*1.5;
                        hp[kptr]=bkh;                        
                        kptr++;                        
                        ox+=bkw*1.5;
                        for(uint8_t c=0;c<keyCol-3;c++){
                            my_lcd.drawRoundRect(ox,oy,bkw,bkh,2, WHITE);
                            my_lcd.drawChar(kbd[kptr],ox+dx,oy+dy,2);
                            // store coordinates 
                            xp[kptr]=ox;
                            yp[kptr]=oy;
                            wp[kptr]=bkw;
                            hp[kptr]=bkh;                            
                            kptr++;                            
                            ox+=bkw;
                        }
                        my_lcd.drawRoundRect(ox,oy,bkw*1.5,bkh,2,WHITE);
                        if(kbd[kptr]<0x20){
                            my_lcd.drawString(&kS[kbd[kptr]*SKEY_LEN],ox+dxsk,oy+dysk,1); 
                        }
                        else my_lcd.drawChar(kbd[kptr],ox+dx,oy+dy,2);
                        // store coordinates 
                        xp[kptr]=ox;
                        yp[kptr]=oy;
                        wp[kptr]=bkw*1.5;
                        hp[kptr]=bkh;                        
                        kptr++;                        
                        oy+=bkh;        
                        break;
                case 4: ox=xk;
                        my_lcd.drawRoundRect(ox,oy,bkw*1.5,bkh,2,WHITE);
                        if(kbd[kptr]<0x20){
                            my_lcd.drawString(&kS[kbd[kptr]*SKEY_LEN],ox+dxsk,oy+dysk,1); 
                        }
                        else my_lcd.drawChar(kbd[kptr],ox+dx,oy+dy,2);
                        // store coordinates 
                        xp[kptr]=ox;
                        yp[kptr]=oy;
                        wp[kptr]=bkw*1.5;
                        hp[kptr]=bkh;                        
                        kptr++;                        
                        ox+=bkw*1.5;
                        my_lcd.drawRoundRect(ox,oy,bkw,bkh,2, WHITE);
                        my_lcd.drawChar(kbd[kptr],ox+dx,oy+1,2);
                        // store coordinates 
                        xp[kptr]=ox;
                        yp[kptr]=oy;
                        wp[kptr]=bkw;
                        hp[kptr]=bkh;                        
                        kptr++;                        
                        ox+=bkw;
                        my_lcd.drawRoundRect(ox,oy,bkw*5,bkh,2, WHITE);
                        if(kbd[kptr]<0x20){
                            my_lcd.drawString(&kS[kbd[kptr]*SKEY_LEN],ox+dxsk,oy+dysk,1); 
                        }
                        else my_lcd.drawChar(kbd[kptr],ox+dx,oy+dy,2);
                        // store coordinates 
                        xp[kptr]=ox;
                        yp[kptr]=oy;
                        wp[kptr]=bkw*5;
                        hp[kptr]=bkh;                        
                        kptr++;                        
                        ox+=bkw*5;
                        my_lcd.drawRoundRect(ox,oy,bkw,bkh,2, WHITE);
                        my_lcd.drawChar(kbd[kptr],ox+dx,oy+dy,2);
                        // store coordinates 
                        xp[kptr]=ox;
                        yp[kptr]=oy;
                        wp[kptr]=bkw;
                        hp[kptr]=bkh;
                        kptr++;                        
                        ox+=bkw;
                        my_lcd.drawRoundRect(ox,oy,bkw*1.5,bkh,2,WHITE);
                        if(kbd[kptr]<0x20){
                            my_lcd.drawString(&kS[kbd[kptr]*SKEY_LEN],ox+dxsk,oy+dysk,1); 
                        }
                        else my_lcd.drawChar(kbd[kptr],ox+dx,oy+dy,2);
                        // store coordinates 
                        xp[kptr]=ox;
                        yp[kptr]=oy;
                        wp[kptr]=bkw*1.5;
                        hp[kptr]=bkh;                                                
                default: break;                        
            }   
        }
    }
    //for(uint8_t a=0;a<keyNb;a++){
    //    printf("a:%d x:%d y:%d\n",a,xp[a],yp[a]);
    //}
}

char TKbd::getKbd(uint16_t xk,uint16_t yk,uint16_t wk,uint16_t hk,bool kclear){

    if(kclear){showKbd(xk,yk,wk,hk,kclear);}

    uint32_t kbdTime=millis();
    uint16_t xp0=0;
    uint16_t yp0=0; 
    
    printf("getKbd\n");
    my_touch.setCal(495, 3398, 721, 3448, 320, 240, 1);
    my_touch.setRotation(3);

    while((millis()-kbdTime)<MAX_KBD_TIME){
        
        if(my_touch.Pressed()) {
            
            xp0 = my_touch.X();
            yp0 = my_touch.Y();
            printf("x:%d y:%d\n",xp0,yp0);my_lcd.fillRect(xp0-2,yp0-2,5,5,YELLOW);delay(250);
            if((xp0 < my_lcd.width())&&(yp0 < my_lcd.height())){
                for(uint8_t kn=0;kn<keyNb;kn++){
                    if((xp0>xp[kn]) && (yp0>yp[kn]) && (xp0<(xp[kn]+wp[kn])) && (yp0<(yp[kn]+hp[kn]))){
                        char c=kbd[kn];
                        printf("c:%c\n",c);
                        if(c==KEY_abc){c=KEY_SHIFT;}
                        if(c==KEY_SHIFT){
                            if(kbd==kbdL){kbd=kbdU;}
                            else kbd=kbdL;
                            showKbd(xk,yk,wk,hk,NEW_KBD);
                            kbdTime=millis();
                            break;
                        }
                        else if(c==KEY_SPACE){c=SPACE;}
                        else if(c==KEY_123){
                            kbd=kbdV;
                            showKbd(xk,yk,wk,hk,NEW_KBD);
                            kbdTime=millis();
                            break;                            
                        }
                        
                        return c;
                    }
                }
            }
            else kbdTime=millis();
        }
    }
    return KEY_TIMOUT;
}
#include <Arduino.h>
#include <stdio.h>
#include <TFT_eSPI.h>
#include <TFT_Touch.h>
#include <touch_kbd.h>
#include "tt_const.h"

extern TFT_eSPI my_lcd;
extern TFT_Touch my_touch;

#define KEYNB 44  // key nb

char* kU={"0123456789AZERTYUIOPQSDFGHJKLM_WXCVBN?__. !_"};
char* kL={"0123456789azertyuiopqsdfghjklm_wxcvbn?__. !_"};
char* kV={"0123456789@#&()+-/*=,;:'\"%{}[]_$\\^~|?.__< >_"};
char* kS={" Shift\0BackSp\0 Enter\0 Space\0  123 \0  abc \0"};

#define SKEY_LEN 7
//#define LKS SKEY_LEN*6

char kbdU[MAX_KEY_NB];
char kbdL[MAX_KEY_NB];
char kbdV[MAX_KEY_NB];

#define VMA_KEY_LEN 11
char kbdVma[]={KEY_Valid,KEY_Modif,KEY_Aband};
char* kSvma={"   Valider\0  Modifier\0Abandonner\0"};

#define ON_KEY_LEN 11
char kbdON[]={KEY_OUI,KEY_NON};
char* kSON={"       OUI\0       NON\0"};

uint16_t xp[MAX_KEY_NB];
uint16_t yp[MAX_KEY_NB];
uint16_t wp[MAX_KEY_NB];
uint16_t hp[MAX_KEY_NB];

TKbd::TKbd(){
    keyNb=0;

}

void TKbd::init(uint8_t kn){
    keyNb=kn;     
    currkbd=kbdL;
    
    memcpy(kbdL,kL,keyNb);kbdL[30]=KEY_SHIFT;kbdL[38]=KEY_BACKSP;kbdL[39]=KEY_123;kbdL[41]=KEY_SPACE;kbdL[43]=KEY_ENTER;
    memcpy(kbdU,kU,keyNb);kbdU[30]=KEY_SHIFT;kbdU[38]=KEY_BACKSP;kbdU[39]=KEY_123;kbdU[41]=KEY_SPACE;kbdU[43]=KEY_ENTER;
    memcpy(kbdV,kV,keyNb);kbdV[30]=KEY_SHIFT;kbdV[38]=KEY_BACKSP;kbdV[39]=KEY_abc;kbdV[41]=KEY_SPACE;kbdV[43]=KEY_ENTER;

    printf("nb:%d currk:%s \n",keyNb,currkbd);delay(1000);

}

void TKbd::showKbd(uint16_t xk,uint16_t yk,uint16_t wk,uint16_t hk,bool kclear,char* kbd,uint8_t keyLines,uint8_t keyCol,char* ksc,uint8_t skl){
    if(!kclear){return;}
    if(kclear){my_lcd.fillRect(xk,yk,wk,hk,BLACK);}
    my_lcd.setTextColor(WHITE, BLACK);
    uint8_t r=my_lcd.getRotation();
    my_lcd.setRotation(3);                  // paysage

        uint16_t bkw=wk/keyCol;
        uint16_t bkh=hk/keyLines;
        uint16_t ox=xk;
        uint16_t oy=yk;
        uint8_t kptr=0;
        uint8_t dx=10,dy=4,dxsk=2,dysk=8;

        for(uint8_t l=0;l<keyLines;l++){
            uint8_t cx=keyCol;
            switch(l){
                case 0:         // la ligne 0 DOIT etre une boucle sur les colonnes pour permettre les menus horizontaux
                case 1: 
                case 2: ox=xk;
                        for(uint8_t c=0;c<keyCol;c++){
                            my_lcd.drawRoundRect(ox+c*bkw,oy,bkw,bkh,2, WHITE);
                            if(kbd[kptr]<0x20){
                                my_lcd.drawString(&ksc[kbd[kptr]*skl],ox+c*bkw+dxsk,oy+dysk,2); 
                            }
                            else my_lcd.drawChar(kbd[kptr],ox+c*bkw+dx,oy+dy,2);
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
                            my_lcd.drawString(&ksc[kbd[kptr]*skl],ox+dxsk,oy+dysk,1); 
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
                            my_lcd.drawString(&ksc[kbd[kptr]*skl],ox+dxsk,oy+dysk,1); 
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
                            my_lcd.drawString(&ksc[kbd[kptr]*skl],ox+dxsk,oy+dysk,1); 
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
                            my_lcd.drawString(&ksc[kbd[kptr]*skl],ox+dxsk,oy+dysk,1); 
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
                            my_lcd.drawString(&ksc[kbd[kptr]*skl],ox+dxsk,oy+dysk,1); 
                        }
                        else my_lcd.drawChar(kbd[kptr],ox+dx,oy+dy,2);
                        // store coordinates 
                        xp[kptr]=ox;
                        yp[kptr]=oy;
                        wp[kptr]=bkw*1.5;
                        hp[kptr]=bkh;
                        kptr++;                                                
                default: break;                        
            }   
        }
        keyNb=kptr;

}

char TKbd::getOuiNon(){

    char c=getKbd0(20,150,240,30,NEW_KBD,kbdON,1,2,kSON,ON_KEY_LEN);
    printf("c:%d\n",c);
    switch(c){
        case KEY_OUI:return 'O';
        case KEY_NON:return 'N';
        default:return 'N';
    }
}

char TKbd::getVma(){

    char c=getKbd0(20,150,240,30,NEW_KBD,kbdVma,1,3,kSvma,VMA_KEY_LEN);
    printf("c:%d\n",c);
    switch(c){
        case KEY_Valid:return 'V';
        case KEY_Modif:return 'M';
        case KEY_Aband:return 'A';
        default:return 'A';
    }
}

char TKbd::getKbd(uint16_t xk,uint16_t yk,uint16_t wk,uint16_t hk,bool kclear){
    char c=getKbd0(xk,yk,wk,hk,kclear,currkbd,5,10,kS,SKEY_LEN);
    
    return c;
}

char TKbd::getKbd0(uint16_t xk,uint16_t yk,uint16_t wk,uint16_t hk,bool kclear,char* kbd,uint8_t lines,uint8_t cols,char* ksc,uint8_t skl){

    if(kclear){showKbd(xk,yk,wk,hk,kclear,kbd,lines,cols,ksc,skl);} // calcul keyNb !

    uint32_t kbdTime=millis();
    uint16_t xp0=0;
    uint16_t yp0=0; 
    
    printf("getKbd\n");

    while((millis()-kbdTime)<MAX_KBD_TIME){
        
        if(my_touch.Pressed()) {
            
            xp0 = my_touch.X();
            yp0 = my_touch.Y();
            printf("x:%d y:%d\n",xp0,yp0);my_lcd.fillRect(xp0-2,yp0-2,5,5,YELLOW);delay(250);
            if((xp0 < my_lcd.width())&&(yp0 < my_lcd.height())){
                for(uint8_t kn=0;kn<keyNb;kn++){
                    if((xp0>xp[kn]) && (yp0>yp[kn]) && (xp0<(xp[kn]+wp[kn])) && (yp0<(yp[kn]+hp[kn]))){
                        char c=kbd[kn];
                        
                        // ****** spécifique kbdL,kbdU,kbdV
                        // ****** vvvvvvvvvvvvvvvvvvvvvvvvv
                        if(ksc==kS){
                            printf("c:%c %d currk:%s kbd:%s\n",c,c,currkbd,kbd);
                            if(c==KEY_abc){c=KEY_SHIFT;}
                            if(c==KEY_SHIFT){
                                if(currkbd==kbdL){currkbd=kbdU;}
                                else currkbd=kbdL;
                                kbd=currkbd;
                                showKbd(xk,yk,wk,hk,NEW_KBD,currkbd,lines,cols,ksc,skl);
                                kbdTime=millis();
                                break;
                            }
                            else if(c==KEY_SPACE){c=SPACE;}
                            else if(c==KEY_123){
                                currkbd=kbdV;
                                kbd=currkbd;
                                showKbd(xk,yk,wk,hk,NEW_KBD,currkbd,lines,cols,ksc,skl);
                                kbdTime=millis();
                                break;                            
                            }
                        }
                        // ****** ^^^^^^^^^^^^^^^^^^^^^^^^^^
                        return c;
                    }
                }
            }
            else kbdTime=millis();
        }
    }
    return KEY_TIMOUT;
}
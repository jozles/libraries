#include <Arduino.h>
#include "capaTouch.h"

IO_REG_TYPE	sBit;
volatile IO_REG_TYPE *sReg; 

IO_REG_TYPE	rBit1;
IO_REG_TYPE	rBit2;
volatile IO_REG_TYPE *rReg1;
volatile IO_REG_TYPE *rReg2;

char prev1='O',prev2='O';
unsigned long k1=0, k2=0;
unsigned long debounce1=0, debounce2=0;

Capat::Capat(){}

void Capat::init(uint8_t samp,uint8_t common,uint8_t* touch,uint8_t touchNb)
{
    samples=samp;
    sBit = PIN_TO_BITMASK(common);
    sReg = PIN_TO_BASEREG(common);
    
    rBit1 = PIN_TO_BITMASK(touch[0]);
    rReg1 = PIN_TO_BASEREG(touch[0]);
    
    keyNb = touchNb;
    Serial.print("capaT ");Serial.print(common);Serial.print(" k=");Serial.print(touchNb);Serial.print(" ");delay(1000);
    for(uint8_t i=0;i<keyNb;i++){
        rBit[i] = PIN_TO_BITMASK(touch[i]);
        rReg[i] = PIN_TO_BASEREG(touch[i]);
    Serial.print(touch[i]);if(i<keyNb-1){Serial.print(";");}
    }
//    Serial.println();
}

void Capat::calibrate()
{
    memset(prevCntMem,0x00,sizeof(uint8_t)*MAXKEY);
    memset(debounce,0x00,sizeof(uint32_t)*MAXKEY);
    memset(keyChSt,0x00,sizeof(uint32_t)*MAXKEY);

    uint16_t maxCount[MAXKEY];
    memset(maxCount,0x00,(uint32_t)MAXKEY*sizeof(uint16_t));
//    Serial.print("s=");Serial.println((uint16_t)sizeof(uint16_t));
    for(uint8_t i=0;i<CALIB;i++){
        count();
        Serial.print(i);
        for(uint8_t k=0;k<keyNb;k++){
            if(cntk[k]>maxCount[k]){maxCount[k]=cntk[k];}
            Serial.print("(");Serial.print(k);Serial.print(")");Serial.print(cntk[k]);Serial.print(" ");Serial.print(maxCount[k]);Serial.print(" ");
        }
        Serial.println();
    }

    for(uint8_t i=0;i<keyNb;i++){              // precharge to allow mean calculation start
        //#define MAXMEAN 0x00cf
        //uint32_t m=(uint32_t)0x01ff*(uint32_t)0x3fff;           //0x007fffff;
        //totalMem[i]=(uint32_t)MAXMEAN*(uint32_t)NBCNTMEM;
        totalMem[i]=(uint32_t)maxCount[i]*(uint32_t)NBCNTMEM;
        //meank[i]=MAXMEAN;
        meank[i]=maxCount[i];
        Serial.print(" | ");Serial.print(i);Serial.print(" ");Serial.print(maxCount[i]);Serial.print(" ");Serial.print(lowTresh[i]);Serial.print("-");Serial.print(highTresh[i]);
        //for(uint8_t j=0;j<NBCNTMEM;j++){
        //    cntMem[NBCNTMEM*i+j]=MAXMEAN;
        //}
    }
    Serial.println();
/*
    for(uint8_t h=0;h<10;h++){count();}     // heating

    for(uint8_t i=0;i<CALIB;i++){
        count();delay(100);
        meanUpdate();
        Serial.print(i);
        for(uint8_t j=0;j<keyNb;j++){debounce[j]=0;}
        
        for(uint8_t v=0;v<1;v++){
            Serial.print("(");Serial.print(v);Serial.print(" ");
            Serial.print(cntk[v]);Serial.print(" ");Serial.print(totalMem[v]);
            Serial.print(")");
            //for(uint8_t j=0;j<NBCNTMEM;j++){Serial.print(cntMem[v*NBCNTMEM+j]);Serial.print(",");}
        }
        Serial.println();
    }
*/    
}

void Capat::count()
{ 
    memset(cntk,0x00,sizeof(uint16_t)*MAXKEY);

    bool rn[MAXKEY];
    bool rt;
/*
    DIRECT_MODE_OUTPUT(sReg, sBit);             // receive output
    DIRECT_WRITE_LOW(sReg, sBit);               // common low    
*/
pinMode(13,OUTPUT);
digitalWrite(13,LOW);

    for(uint8_t s=0;s<samples;s++){
//        Serial.print(s);Serial.print(" ");

/*        for(uint8_t i=0;i<keyNb;i++){
            DIRECT_MODE_INPUT(rReg[i], rBit[i]);    // receive input
            DIRECT_MODE_OUTPUT(rReg[i], rBit[i]);   // receive output
            DIRECT_WRITE_LOW(rReg[i], rBit[i]);     // receive low
            rn[i]=false;
        }
*/
        rt=false;
/*
DIRECT_MODE_INPUT(rReg[0], rBit[0]);    // receive input
DIRECT_MODE_OUTPUT(rReg[0], rBit[0]);   // receive output
DIRECT_WRITE_LOW(rReg[0], rBit[0]);     // receive low
*/
pinMode(12,INPUT);
pinMode(12,OUTPUT);
digitalWrite(12,LOW);


        noInterrupts();

/*        for(uint8_t i=0;i<keyNb;i++){
            DIRECT_MODE_INPUT(rReg[i], rBit[i]);    // receive input (capacitor is empty)
        }
*/

//DIRECT_MODE_INPUT(rReg[0], rBit[0]);
pinMode(12,INPUT);

//        DIRECT_WRITE_HIGH(sReg, sBit);              // begin filling common high
digitalWrite(13,HIGH);
        while(!rt){                                 // pooling keys until all high
            rt=true;
/*            for(uint8_t i=0;i<keyNb;i++){
                if(!rn[i]){cntk[i]++;rn[i]=DIRECT_READ(rReg[i], rBit[i]);rt&=rn[i];}
            }
*/            
if(
//    !DIRECT_READ(rReg[0], rBit[0]) 
    
    !digitalRead(12) && cntk[0]<MAXCOUNT){cntk[0]++;rt=false;}
        }
    
        interrupts();
        
/*        for(uint8_t i=0;i<keyNb;i++){
            DIRECT_WRITE_HIGH(rReg[i], rBit[i]);     // receive high    
            DIRECT_MODE_OUTPUT(rReg[i], rBit[i]);    // receive output
            DIRECT_WRITE_HIGH(rReg[i], rBit[i]);     // receive high
            rn[i]=true;
        }
*/        
        rt=true;
/*
DIRECT_WRITE_HIGH(rReg[0], rBit[0]);     // receive high    
DIRECT_MODE_OUTPUT(rReg[0], rBit[0]);    // receive output
DIRECT_WRITE_HIGH(rReg[0], rBit[0]);     // receive high
*/
digitalWrite(12,HIGH);
pinMode(12,OUTPUT);
digitalWrite(12,HIGH);



        noInterrupts();

/*        for(uint8_t i=0;i<keyNb;i++){
            DIRECT_MODE_INPUT(rReg[i], rBit[i]);    // receive input (capacitor is empty)
        }
*/        
//DIRECT_MODE_INPUT(rReg[0], rBit[0]);    // receive input (capacitor is empty)
pinMode(12,INPUT);
//        DIRECT_WRITE_LOW(sReg, sBit);               // common low begin filling    
digitalWrite(13,LOW);
        while(rt){                                  // pooling keys until all low
            rt=false;
/*            
            for(uint8_t i=0;i<keyNb;i++){
                if(rn[i]){cntk[i]++;rn[i]=DIRECT_READ(rReg[i], rBit[i]);rt|=rn[i];}
            }
*/            
if(
    //DIRECT_READ(rReg[0], rBit[0])
    digitalRead(12) && cntk[0]<MAXCOUNT){cntk[0]++;rt=true;}
        }
/*        for(uint8_t p=0;p<keyNb;p++){Serial.print(cntk[p]);Serial.print(":");}
        Serial.println();
*/
        interrupts();
    }
}

void Capat::meanUpdate()
{
    for(uint8_t i=0;i<keyNb;i++){

        if(millis()-debounce[i]>DEBOUNCE){      // no update if debounce running
            //uint8_t*  prev=&prevCntMem[i];
            uint32_t* total=&totalMem[i];
            uint16_t* mean=&meank[i];
            uint16_t* lowT=&lowTresh[i];
            uint16_t* highT=&highTresh[i];
            uint16_t  cnta=cntk[i];
            uint16_t* cnt=&cnta;
            bool*     val=&keyVal[i];
            bool*     chg=&keyChge[i];

            if(*cnt<=*lowT/2+*mean){                  // no update when ON (but mean=0x8fff forcing at start)
                //(*prev)++;
                //(*prev)&=(NBCNTMEM-1);
                //*total-=cntMem[NBCNTMEM*i+*prev];
                if(millis()-keyChSt[i]>CHGSTROBE){    // protect from near untouching finger
                    *total-=*mean;
                    *total+=*cnt;
                }
                //cntMem[NBCNTMEM*i+*prev]=*cnt;
                *mean=*total/NBCNTMEM;
                
                *lowT=*mean/10;                     // low treshold = mean+1/8
                *highT=*lowT+*lowT/4;               // high treshold = mean+1/4
            }
            *chg=false;
            if(*cnt<*mean){*cnt=*mean;}
            if((*cnt-*mean)>highTresh[i]){if(*val!=true){*chg=true;debounce[i]=millis();keyChSt[i]=millis();}*val=true;}
            else if((*cnt-*mean)<lowTresh[i]){if(*val!=false){*chg=true;debounce[i]=millis();keyChSt[i]=millis();}*val=false;}
            // else nomansland ->no change
        }
    }
}    

void Capat::capaKeysCheck()
{
    count();
    meanUpdate();
}

uint8_t Capat::capaKeysGet(uint8_t keyNum)
{
    return  keyVal[keyNum]*2+keyChge[keyNum];       
}

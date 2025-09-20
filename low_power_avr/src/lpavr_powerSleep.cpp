
#include "radio_const.h"

//#ifdef MACHINE_DET328

//#include "radio_powerSleep.h"
//#include "radio_user_peri.h"
//#include "radio_util.h"

/* mécanisme
 *  
 *  la fonction uint16_t sleepPwrDown(durat) gère et effectue la mise en sommeil profond
 *  SLEEP_MODE_PWR_DOWN du MCU, et débranche tout ce qui consomme pour assurer la veille 
 *  la plus sobre possible.
 *  Selon l'argument durat, le réveil provient du watchdog timer ou du pin d'interruption 1 si durat = 0.
 *    WDT :
 *    Les durées valides effectuées par le WDT sont définies dans la fonction wdtSetup()
 *    qui initialise le registre WDTCSR pour déclencher le timer.
 *    Au réveil, la fonction wdtDisable() inhibe le WDT. 
 *    La fonction ISR(WDT_vect) est exécutée lors de l'interruption issue du WDT
 *    INT1 :
 *    La fonction int1_ISR() est exécutée lors de l'interruption,
 *    issue du pin INT1
 *    Comme le WDT est stoppé, la consommation est moindre
 *    
 *  la fonction letalSleep() n'effectue pas wdtSetup ni ne branche d'interruption du pin INT1
 *  rien ne sortira le CPU du sommeil sauf un reset physique. (à utiliser si les batteries 
 *  sont épuisées par exemple).
*/


#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include <SPI.h>

#include "lpavr_powerSleep.h"

#ifdef NRF
#include "nRF24L01.h"
#include "nrf24l01s.h"
extern Nrfp radio;
#endif

#ifdef LORA
#include "LoRa.h"
#include "LoRa_const.h"
extern LoRaClass radio;
#endif

extern bool   diags;
float         volts=0;                           // tension alim (VCC)
extern bool   lowPower;                          
extern float  lowPowerValue;                      
extern float  temp;
uint8_t       cntTest=0;                         // test watchdog
bool          wdIntFlag=false;


extern uint32_t nbS;

#define TT_STEP 6
const char* textTimings=" T16:\0 T32:\0 T64:\0T128:\0T256:\0T512:\0 T1K:\0 T2K:\0 T4K:\0 T8K:\0";
int16_t sleepTimings[]={T8000,T4000,T2000,T1000,T500,T250,T125,T64,T32,T16};
int32_t realSleepTimings[]={RT8000,RT4000,RT2000,RT1000,RT500,RT250,RT125,RT64,RT32,RT16};
int32_t cntSleepTimings[]={LT8000,LT4000,LT2000,LT1000,LT500,LT250,LT125,LT64,LT32,LT16};

void diagT2(char* texte,int duree)
{
  Serial.println(texte);delay(duree*1000);
}

ISR(WDT_vect)                     // ISR interrupt service for MPU INT WDT vector
{
  wdIntFlag=true;
}

ISR(TIMER2_COMPA_vect)            // ISR for Timer2 compare int
{
  
}


void int1_ISR()                   // reed ISR
{
  sleep_disable();
  detachInterrupt(0);
  detachInterrupt(1);
}

void int0_ISR()                   // external timer ISR
{
  sleep_disable();
  detachInterrupt(0);
  detachInterrupt(1);
}


void wdtSetup(uint8_t durat)  // (0-9) durat>9 for external wdt on INT0 (à traiter)
{
// datasheet page 51 and 54, Watchdog Timer.


/*  MCUSR MCU status register (reset sources)(every bit cleared by writing 0 in it)
 *   WDRF reset effectué par WDT
 *   BORF ------------------ brown out detector
 *   EXTRF ----------------- pin reset
 *   PORF ------------------ power ON
 *
 *  WDTCSR watchdog timer control
 *   WDIF watchdog interrupt flag (set when int occurs with wdt configured for) (reset byu writing 1 or executing ISR(WDT_vect))
 *   WDIE watchdog interrupt enable  -> counter ovf make interrupt
 *   WDE  watchdog reset enable      -> counter ovf make reset
 *        WDE  WDIE   Mode
 *         0    0     stop (no watchdog)
 *         0    1     interrupt
 *         1    0     reset
 *         1    1     interrupt then reset (WDIE->0 lors de l'interruption, retour au mode reset)
 *       !!!! fuse WDTON forces reset mode if 0 !!!!
 *   WDCE watchdog change enable (0 write enable to WDE and prescaler update ; auto cleared after 4 cycles)
 *   WDP[3:0] prescaler 2^(0-9)*2048 divide WDT oscillator (f=128KHz p*2048=16mS)
 *
 *   wdr instruction resets timer (wdt_reset();)
 *   
 *   power down supply current for ATMEGA 328P page 594 : typically 4,5 uA at 3,3V 25°C with watchdog enabled (about 6,5 at 5V)
 *   
 *   idle supply current for ATMEGA 328P page 591 : typically labout 0,7mA at 3,3V 8MHz 
 *   
 *   
 */

// WDT prescaler - WDP3-0 bits (msec)
/*
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
*/
    noInterrupts();
    wdt_reset();

    MCUSR &= ~(1<<WDRF);  // pour autoriser WDE=0

#ifdef ATMEGA328
    WDTCSR = (1<<WDCE) | (1<<WDE);    // WDCE ET WDE must be 1
                                      // to write WDP[0-3] and WDE in the following 4 cycles
    WDTCSR = (1<<WDIE) | durat;       // WDCE must be 0 ; WDE=0, WDIE=1 interrupt mode, TXXX 
                                      
#endif //     

    interrupts();
}

void wdtDisable()
{
    noInterrupts();
    wdt_reset();
    MCUSR &= ~(1<<WDRF);              // to allow WDE=0
    
    WDTCSR = (1<<WDCE) | (1<<WDE);    // WDCE ET WDE must be 1 to write WDE in the following 4 cycles
    WDTCSR = 0;                       // WDE and WDIE disabled

    interrupts();                                                    
}

void lethalSleep()
{

    ADCSRA=0;PRR |= (1<<PRADC);             // ADC shutdown
    ACSR &= ~(1<<ACIE);                     // comparator disable 
    ACSR |= (1<<ACD);                       // comparator disable
    TWCR &= ~(1<<TWEN);                     // twi disable
    SPCR &= ~(1<<SPE);                      // spi disable
    PRR=0xff;                               // power_all_disable();

    set_sleep_mode(SLEEP_MODE_PWR_DOWN); 
    noInterrupts();                         // cli();
    sleep_enable();                       
#ifdef ATMEGA328
    sleep_bod_disable();                    // BOD halted if followed by sleep_cpu 
#endif //
    sleep_cpu();                            // noInterrupts -> no awake
}

void clearWd()
{
  
  delayMicroseconds(4);   // some time to saturate reset strobe mosfet (only 2.5V Vgs)
   // external timer : high on done pin ends high pulse -> falling edge generate low pulse on reset
   // VCHECK high shorten reset at VCC -> low pulse masked during volts reading
      bitSet(PORT_DONE,BIT_DONE);           //digitalWrite(DONE,HIGH);
      bitSet(DDR_DONE,BIT_DONE);            //pinMode(DONE,OUTPUT);                 // 100nS minimum pulse
      bitClear(PORT_DONE,BIT_DONE);         ////digitalWrite(DONE,LOW);
      bitClear(DDR_DONE,BIT_DONE);          //pinMode(DONE,INPUT);
  delayMicroseconds(8);   // some time to fill up the reset pulse capacitor low drived by 5111 

}

void checkOn()                              // voltage and temperature reading + reset strobe
{

  bitSet(DDR_VCHK,BIT_VCHK);                //pinMode(VCHECK,OUTPUT); 
  bitSet(PORT_VCHK,BIT_VCHK);               //digitalWrite(VCHECK,VCHECKHL);     
  delayMicroseconds(12);                    // mosfet settling time

}

void checkOff()
{
  bitClear(PORT_VCHK,BIT_VCHK);  
}

void wd()
{
  checkOn();                        // reset strobe on
  clearWd();
  checkOff();                       // reset strobe off
}

uint16_t adcRead0(uint8_t admuxval,uint8_t dly)      // dly=1 if ADC halted
{
    uint16_t a=0;
    
    PRR &= ~(1<<PRADC);    

    ADCSRA |= (1<<ADEN);                    // ADC enable to write ADMUX
    ADMUX   = admuxval;

    delayMicroseconds(10);                  

    ADCSRA  = 0 | (1<<ADEN) | (1<<ADIF) | (1<<ADPS2) | (0<<ADPS1) | (0<<ADPS0);   // ADC enable + start conversion + prescaler /16
    ADCSRA  |= (1<<ADSC);

    //delayMicroseconds(100+dly*50);         // ok with /16 prescaler @8MHz
 unsigned long t=micros();
   while((ADCSRA & (1<<ADSC))!=0 && (micros()-t)<200){}   // wait end of conversion

    a=ADCL;
    a+=ADCH*256;

    ADCSRA &= ~(1<<ADEN);
    PRR |= (1<<PRADC);
//Serial.println(a);
    return a;
}

float adcRead(uint8_t admuxval,float factor, uint16_t offset, uint8_t ref,uint8_t dly)      // dly=1 if ADC halted
{
  return (float)((adcRead0(admuxval,dly)*factor-(offset))+ref);
}

void getVolts(float vfactor,float thfactor,float* vt,float* th)         // get unregulated voltage/temp and reset watchdog for external timer period 
{
  checkOn();

  sleepStdby(1);                                    // mosfet & mcp9700 1mS settling time

  if(vfactor==0){vfactor=VFACTOR;}
  *vt=adcRead(VADMUXVAL,vfactor,0,0,1);
  if(*vt<=lowPowerValue){lowPower=true;}

#ifndef DS18X20
  //Serial.print(thfactor*1000);
  if(thfactor==0){thfactor=TFACTOR;}
  //delayMicroseconds(1000);                        // mcp9700 1mS settling time
  *th=adcRead(TADMUXVAL,thfactor,TOFFSET,TREF,1);
  *th=(float)((int)(temp*10))/10;
#endif 

  checkOff();
}

/*void getVolts(float vfactor,float thfactor)
{
  getVolts(vfactor,thfactor,&volts,&temp);
}*/

/*void getVolts()
{
  getVolts(0,0);
}*/

void disable_pins()
{

  DDRC = B00000000;PORTC = B00000000;

  }

void sleepPwrDown(uint8_t durat)  /* *** WARNING *** no hardware PowerUp()/down included    */
{                                 /*       durat=0 to enable external timer (INT0)          */

    ADCSRA=0;PRR |= (1<<PRADC);             // ADC shutdown
    ACSR &= ~(1<<ACIE);                     // comparator disable 
    ACSR |= (1<<ACD);                       // comparator disable
    TWCR &= ~(1<<TWEN);                     // twi disable
    SPCR &= ~(1<<SPE);                      // spi disable
    
    byte old_prr=PRR;
    PRR=0xff;                               // power_all_disable();
    
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    
    noInterrupts();                         // cli();

    if(durat!=0){wdtSetup(durat);}          // WDTCSR register setup for sleep with WDT int awake

    else{                                   // external timer interrupt awaking
      //noInterrupts();
      wdt_disable();
      attachInterrupt(0,int0_ISR,ISREDGE);  // external timer interrupt enable
      EIFR=bit(INTF0);                      // clr flag
    }
    //attachInterrupt(1,int1_ISR,CHANGE);   // reed interrupt enable
    //EIFR=bit(INTF1);                      // clr flag
        
    sleep_enable();                       
#ifdef ATMEGA328
    sleep_bod_disable();                    // BOD halted if followed by sleep_cpu 
#endif //
    interrupts();                           // sei();
    sleep_cpu();
    sleep_disable();
    if(durat!=0){wdtDisable();}                         

    PRR=old_prr;

    //if(durat==0){wd();}                     // watchdog
}

uint8_t sleepPwrDownV(int32_t durat,int32_t* slpt,bool sleep)  // versatile with variable durat
                            /* *** WARNING *** no hardware PowerUp()/down included    */
{                           /*       durat=0 to enable external timer (INT0)          */
  if(durat!=0){ 

    //Serial.print(durat);Serial.print(',');delay(5);

    durat*=100;
    *slpt*=100;

    uint8_t old_TWCR=TWCR;
    uint8_t old_SPCR=SPCR;

    ADCSRA=0;PRR &= ~(1<<PRADC);            // ADC shutdown
    ACSR &= ~(1<<ACIE);                     // comparator disable
    ACSR |= (1<<ACD);                       // comparator disable
    TWCR &= ~(1<<TWEN);                     // twi disable
    SPCR &= ~(1<<SPE);                      // spi disable
    
    uint8_t old_prr=PRR;
    
    if(sleep){
      power_all_disable();                        // all bits set in PRR register (I/O modules clock halted ; no millis())     
      set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    }

    for(uint8_t k=0;k<NB_PRESCALER_VALUES;k++){     

      while(durat+100>=realSleepTimings[k]){       // !!!!!!!!!!!!!!!!!!!!!!!!!

        if(sleep){
          wdtSetup(sleepTimings[k]);            // WDTCSR register setup for sleep with WDT int awake        
          noInterrupts();                       // cli(); 
          sleep_enable();     
                      
#ifdef ATMEGA328
          sleep_bod_disable();                  // BOD halted if followed by sleep_cpu 
#endif //
          interrupts();                         // sei();
          sleep_cpu();
          sleep_disable();
          wdtDisable();
          *slpt+=cntSleepTimings[k];
        }
        else{                                   // wait for wd interrupt 
          wdtSetup(sleepTimings[k]);            // WDTCSR register setup for sleep with WDT int awake 
          //noInterrupts();
          wdIntFlag=false;     
          sleep_enable();
          //interrupts();
          while (wdIntFlag==false){delayMicroseconds(12);}
          sleep_disable();
          wdtDisable();         
        }
        
        durat-=realSleepTimings[k];
      } 
    }
    if(sleep){
      PRR = old_prr;
    }
    *slpt/=100;
    durat/=100;

    TWCR = old_TWCR;
    SPCR = old_SPCR;
  }
  if(durat>1 && sleep){sleepStdby(durat);durat=0;}

  return durat;                                 // remaining time unsleepable 
}

uint8_t sleepPwrDownV(int32_t durat,int32_t* slpt)
{
  return sleepPwrDownV(durat,slpt,true);
}

void sleepStdby(int32_t durat)                  // extended standby mode with Timer2 - 32mS mùaxi
{                                               // 128uS resolution

  uint8_t old_PRR=PRR;
  uint8_t old_TWCR=TWCR;
  uint8_t old_SPCR=SPCR;  

        if(durat>32){durat=32;}

        uint16_t d=(durat*1000)/128;

        //ADCSRA &= ~(1<<ADEN);                   // ADC shutdown
        ADCSRA=0;
        wdtDisable();
        ACSR &= ~(1<<ACIE);                     // comparator disable
        ACSR |= (1<<ACD);                       // comparator disable
        TWCR &= ~(1<<TWEN);                     // twi disable
        SPCR &= ~(1<<SPE);                      // spi disable

        //power_all_disable();                  // all bits set in PRR register (I/O modules clock halted)
        PRR |= (1<<PRTIM0);
        //PRR &= ~(1<<PRTIM1);                    // millis()
        //PRR &= ~(1<<PRTIM2);                    // wake interrupt
        PRR |= (1<<PRSPI);
        PRR |= (1<<PRUSART0);
        PRR |= (1<<PRADC);
        PRR |= (1<<PRTWI);

        set_sleep_mode(SLEEP_MODE_EXT_STANDBY); 
        //set_sleep_mode(SLEEP_MODE_IDLE); 

        noInterrupts();                         // cli(); 
          
        TCCR2A = 0;
        TCCR2B = 0;
        OCR2A  = (uint8_t)d;                    // set compare match register increments 1024/8000000
        TCCR2A |= (1 << WGM21);                 // turn on CTC mode
  // Set 1024 prescaler
        TCCR2B |= (1 << CS20);
        TCCR2B |= (1 << CS21);
        TCCR2B |= (1 << CS22);   
        TCNT2  = 0;
        TIFR2  |= (1 << OCF2A);
        TIMSK2 |= (1 << OCIE2A);                // enable timer compare interrupt

        sleep_enable();                       
#ifdef ATMEGA328
        sleep_bod_disable();                    // BOD halted if followed by sleep_cpu 
#endif 
        interrupts();                           // sei();

        sleep_cpu();        
        sleep_disable();
        //power_all_enable();                     // all bits clr in PRR register (I/O modules clock running)
        PRR=old_PRR;
        TWCR = old_TWCR;
        SPCR = old_SPCR;
}


void calibratePwrDown()                           // should be done for 3.9V, 3.7V, 3.5V and 30°,20°,10°,0°
{
  Serial.print("calibration ");if(diags){Serial.println();}
  int32_t sl=0;
  int32_t saveRST[NB_PRESCALER_VALUES];

  for(uint8_t i=NB_PRESCALER_VALUES-1;i>3;i--){   // 16->512
    saveRST[i]=realSleepTimings[i];
    unsigned long t1=0,t2=0;
    unsigned long t0=micros()/100,t=t0;
    while(t == t0){t=micros()/100;}

    sleepPwrDownV(realSleepTimings[i]/100,&sl,false);t1=micros()/100;   // no sleep

    sleepPwrDownV(realSleepTimings[i]/100,&sl,true);t2=micros()/100;    // sleep

    if(diags){
      Serial.print(i);Serial.print(' ');delay(1);
      Serial.print(realSleepTimings[i]);Serial.print(';');delay(1);}

    realSleepTimings[i]=(t1-t)*10;
    if(realSleepTimings[i]==0){realSleepTimings[i]=saveRST[i];}
    else realSleepTimings[i]+=CKRSTDLY*10;        // delay restart after sleep
    
    if(diags){
      Serial.print(realSleepTimings[i]);Serial.print('(');delay(1);
      Serial.print(t1);Serial.print('-');delay(1);
      Serial.print(t);Serial.print(") ");delay(1);
      Serial.print(cntSleepTimings[i]);Serial.print(';');delay(1);
     }

    cntSleepTimings[i]=realSleepTimings[i];
    if(diags){Serial.println();}delay(1);
  }
  Serial.println("done");
}

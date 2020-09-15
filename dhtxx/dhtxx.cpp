
/*   
    Interrupt-free, small memory wasting dhtxx interface (about 1320 bytes)
    this is working with any pin who accept :
           PULLUP mode, digitalRead and digitalWrite
    Detection of rising and falling edge of data may be delayed by 11uS 
    As the shortest serial bit is about 75uS long, concurrent isr should not be longer 
    than 53uS (75-2*11). This should not happen with usual Arduino core.

    getDht(uint8_t pin) return a status (codes below) and load values into   
    four bytes : dhtxx.temph, dhtxx.templ, dhtxx.humidh, dhtxx.humidl 
    (h for integer value, l for decimal)
    Depending of dht unit, decimal value may be unrelevant. Check datasheet.
    pin is the Arduino pin number used to connect DHT.
*/

#include "Arduino.h"
#include "Dhtxx.h"

// possible return status for getDHT() function
#define GET_OK 1
#define TO10 -10
#define TO11 -11
#define TO12 -12
#define TO2 -2
#define SUM_ERR -3

// various timings for time-out
#define T_WAIT_START 50
#define T_LOW_START 100
#define T_HIGH_START 100
#define T_LOW 55
#define T_HIGH 75
#define T_BIT_MED 100


Dhtxx::Dhtxx()
{
}

float Dhtxx::getDhtTemp(uint8_t pin)
{
  getDht(pin);
  return (float)(frame[2]+frame[3]/100);  
}

float Dhtxx::getDhtHumid(uint8_t pin)
{
  getDht(pin);
  return (float)(frame[0]+frame[1]/100);  
}

int8_t Dhtxx::getDht(uint8_t pin)
{

uint8_t bitval[]={128,64,32,16,8,4,2,1};  //1,2,4,8,16,32,64,128
uint8_t sum=0;
uint8_t i=0,j=0;
long micr=0,micr_bit=0;

  // output 19mS pulse
  pinMode (pin, OUTPUT);
  digitalWrite (pin,LOW);
  delay(19);
  digitalWrite (pin,HIGH);

  pinMode (pin, INPUT_PULLUP);   // start acquisition

  // skip 1 bit then read 5 bytes

  // skip 1 bit
  micr=micros()+T_WAIT_START;
  // wait for falling edge, return error if time-out occurs
  while(digitalRead(pin)!=LOW) {if(micros()>micr){return TO10;}}
  micr=micros()+T_LOW_START;
  // wait for rising edge, return error if time-out occurs
  while(digitalRead(pin)!=HIGH) {if(micros()>micr){return TO11;}}
  micr=micros()+T_HIGH_START;
  // wait for falling edge, return error if time-out occurs
  while(digitalRead(pin)!=LOW) {if(micros()>micr){return TO12;}}

  //read 5 bytes
  for(i=0;i<5;i++){

  // get 8 bits
    frame[i]=0;
    for(j=0;j<8;j++){

      micr_bit=micros();
      while(digitalRead(pin)!=HIGH) {if(micros()>micr_bit+T_LOW){return TO2;}}
      // 1st pin test about 15uS after start read LOW ; next about 11uS after
      micr=micros()+T_HIGH;
      while(digitalRead(pin)!=LOW) {if(micros()>micr){return TO2;}}
      // 1st pin test less than 13,6uS after read HIGH ; next 11uS after

      // if greater than (about) 100uS bit value is "1" else "0"
      if((micros()-micr_bit)>T_BIT_MED){frame[i]+=bitval[j];}   // 6uS
      // next read less than 17uS after previous
    }
    sum+=frame[i]; // update checksum
  }
  // verify sum
  if(sum!=2*frame[4]){return SUM_ERR;}
  return GET_OK;
}

#include <Arduino.h>
#include <wire.h>

#include "ds3231.h"

#ifndef DUE
#define WIRE Wire
#endif  // DUE
#ifdef DUE
extern TwoWire Wire1;
#define WIRE Wire1
#endif  // DUE

Ds3231::Ds3231()  // constructeur
{
}

byte decToBcd(byte val)
{
  return( (val/10*16) + (val%10) );
}

byte bcdToDec(byte val)
{
  return( (val/16*10) + (val%16) );
}

void Ds3231::setTime(byte second, byte minute, byte hour,
    byte dayOfWeek,byte dayOfMonth, byte month, byte year)
{
  WIRE.beginTransmission(i2cAddr);
  WIRE.write(0); // set next input to start at the seconds register
  WIRE.write(decToBcd(second)); // set seconds
  WIRE.write(decToBcd(minute)); // set minutes
  WIRE.write(decToBcd(hour)); // set hours
  WIRE.write(decToBcd(dayOfWeek)); // set day of week (1=Sunday, 7=Saturday)
  WIRE.write(decToBcd(dayOfMonth)); // set date (1 to 31)
  WIRE.write(decToBcd(month)); // set month
  WIRE.write(decToBcd(year)); // set year (0 to 99)
  WIRE.endTransmission();
}


void Ds3231::readTime(byte *second,byte *minute,byte *hour,
    byte *dow,byte *day,byte *month,byte *year)
{
  WIRE.beginTransmission(i2cAddr);
  WIRE.write(0);
  WIRE.endTransmission();
  WIRE.requestFrom(i2cAddr, 7);

  *second = bcdToDec(WIRE.read() & 0x7f);
  *minute = bcdToDec(WIRE.read());
  *hour = bcdToDec(WIRE.read() & 0x3f);
  *dow = bcdToDec(WIRE.read());
  *day = bcdToDec(WIRE.read());
  *month = bcdToDec(WIRE.read());
  *year = bcdToDec(WIRE.read());
}

void Ds3231::readTemp(float* th)
{
  WIRE.beginTransmission(i2cAddr);
  WIRE.write(17); // set DS3231 register pointer to 11h
  WIRE.endTransmission();
  WIRE.requestFrom(i2cAddr, 2);
  // request two bytes of data from DS3231 starting from register 11h

  byte msb = WIRE.read();
  byte lsb = WIRE.read();
  //Serial.print(*msb);Serial.print(":");Serial.println(*lsb);
  switch(lsb)
  {
    case 0:break;
    case 64: lsb=25;break;
    case 128:lsb=50;break;
    case 192:lsb=75;break;
    default: msb=99,lsb=99;break; // error
  }
  *th=(float)msb+(float)lsb/100;
}

void Ds3231::getDate(uint32_t* hms2,uint32_t* amj2,byte* js,char* strdate)
{
  const char* days={"SunMonTueWedThuFriSat"};
  const char* months={"JanFebMarAprMayJunJulAugSepOctNovDec"};
  byte seconde,minute,heure,jour,mois,annee; // numérique DS3231   // ,joursemaine
  #define LB 8
  char buf[LB];memset(buf,0x00,LB);
  readTime(&seconde,&minute,&heure,js,&jour,&mois,&annee);
  *hms2=(long)(heure)*10000+(long)minute*100+(long)seconde;*amj2=(long)(annee+2000)*10000+(long)mois*100+(long)jour;
  memset(strdate,0x00,LDATEB);
  memcpy(strdate,days+(*js-1)*3,3);strcat(strdate,", ");sprintf(buf,"%u",(byte)jour);strcat(strdate,buf);
  strcat(strdate," ");strncat(strdate,months+(mois-1)*3,3);strcat(strdate," ");sprintf(buf,"%u",annee+2000);strcat(strdate,buf);
  strcat(strdate," ");sprintf(buf,"%.2u",heure);strcat(strdate,buf);strcat(strdate,":");sprintf(buf,"%.2u",minute);
  strcat(strdate,buf);strcat(strdate,":");sprintf(buf,"%02u",seconde);strcat(strdate,buf);
  if(strlen(strdate)<LDATEB-5){strcat(strdate," GMT");}else{strcat(strdate,"ov");}
}

void Ds3231::alphaNow(char* buff)
{

  byte second,minute,hour,day,month,year,dow;
  readTime(&second,&minute,&hour,&dow,&day,&month,&year);

  sprintf(buff,"%.8lu",(long)(year+2000)*10000+(long)month*100+(long)day);
  sprintf((buff+8),"%.2u",hour);sprintf((buff+10),"%.2u",minute);sprintf((buff+12),"%.2u",second);
  buff[14]=dow;
  buff[15]='\0';
  //Serial.print("alphaNow ");Serial.print(buff);Serial.print(" ");Serial.print(ndt.hour);Serial.print(" ");Serial.print(ndt.minute);Serial.print(" ");Serial.println(ndt.second);
  //Serial.println();
}



#include "Arduino.h"
#include <shconst2.h>

#ifndef PERIF
#include <MemoryFree.h>
#include <FreeStack.h>
#endif // PERIF

//#ifdef PERIF
//#include <ESP8266WiFi.h>
//#endif // PERIF

extern char  pass[];
extern char* chexa;

static unsigned long blinktime=0;
int     nbreBlink=0;          // si nbreBlink impair   -> blocage
int     cntBlink=0;

#define TIMEOVFSLOTNB 10
uint32_t timeOvfSlot[TIMEOVFSLOTNB];

extern char* usrnames;
extern char* usrpass;


  /* debug
int   cntdebug[NBDBPTS];
unsigned long  timedebug[NBDBPTS*NBDBOC];
int*  v0debug[NBDBPTS*NBDBOC];
int*  v1debug[NBDBPTS*NBDBOC];
char* v2debug[NBDBPTS*NBDBOC];
char* v3debug[NBDBPTS*NBDBOC];
*/

uint32_t pinLed;

bool wdEnable=false;
bool trigwdOn=false;

int   int00=0;
int*  int0=&int00;


int convIntToString(char* str,int num)
{

  int i=0,t=0,num0=num;
  while(num0!=0){num0/=10;i++;}                 // comptage nbre chiffres partie entière
  t=i;
  for (i=i;i>0;i--){num0=num%10;num/=10;str[i-1]=chexa[num0];}
  str[t]='\0';
  if(str[0]=='\0'){str[0]='0';}
  return t;
}

int convNumToString(char* str,float num)  // retour string terminée par '\0' ; return longueur totale '\0' inclus
{
  int i=0,v=0,t=0;

  t=convIntToString(str,(int)num);

  num=num-(int)num;
  str[t]='.';
  for(i=0;i<2;i++){num=num*10;v=(int)num;num=num-v;str[t+1+i]=chexa[v];}
  t+=3;
  str[t]='\0';

  return t;
}

uint8_t conv_atob(char* ascii,uint16_t* bin)
{
  uint8_t j=0;
  byte c;
  *bin=0;
  for(j=0;j<LENVAL;j++){c=ascii[j];if(c>='0' && c<='9'){*bin=*bin*10+c-48;}else break;}
  return j;
}

uint8_t conv_atobl(char* ascii,uint32_t* bin)
{
  uint8_t j=0;
  byte c;
  *bin=0;
  for(j=0;j<LENVAL;j++){c=ascii[j];if(c>='0' && c<='9'){*bin=*bin*10+c-48;}else break;}
  return j;
}

uint8_t conv_atobl(char* ascii,uint32_t* bin,uint8_t len)
{
  uint8_t j=0;
  byte c;
  *bin=0;
  for(j=0;j<len;j++){c=ascii[j];if(c>='0' && c<='9'){*bin=*bin*10+c-48;}else break;}
  return j;
}

void sp(const char* a,bool ln)
{
    Serial.print(a);if(ln!=0){Serial.println();}
}

void serialPrintIp(uint8_t* ip)
{
  for(int i=0;i<4;i++){Serial.print(ip[i]);if(i<3){Serial.print(".");}}
  //Serial.print(" ");
}

uint8_t textIp(byte* nipadr,byte* buf)
{
  byte* in=nipadr;
  uint16_t uin;
  for(int i=0;i<4;i++){
    in+=conv_atob((char*)in,&uin);
    in++;
    *buf++=(uint8_t)uin;
  }
  return (uint8_t)(in-nipadr);
}

void charIp(byte* nipadr,char* aipadr,char* jsbuf)
{
  char buf[8];
  for(int i=0;i<4;i++){
        sprintf(buf,"%d",nipadr[i]);strcat(aipadr,buf);if(i<3){strcat(aipadr,".");}
  }
  if(jsbuf!=nullptr){strcat(jsbuf,aipadr);strcat(jsbuf,";");}
}

void charIp(byte* nipadr,char* aipadr)
{
  charIp(nipadr,aipadr,nullptr);
}
/*
void charIp(IPAddress* nipadr,char* aipadr,char* jsbuf)   // ne fonctionne pas ... pb avec <ESP8266WiFi.h>
{
  char buf[8];
  for(int i=0;i<4;i++){
        sprintf(buf,"%d",(uint8_t)nipadr[i]);strcat(aipadr,buf);if(i<3){strcat(aipadr,".");}
  }
  if(jsbuf!=nullptr){strcat(jsbuf,aipadr);strcat(jsbuf,";");}
}

void charIp(IPAddress* nipadr,char* aipadr)
{
  charIp(nipadr,aipadr,nullptr);
}
*/
void conv_atoh(char* ascii,byte* h)
{
    uint8_t c=0;
  c = (uint8_t)(strchr(chexa,ascii[0])-chexa)<<4 ;
  c |= (uint8_t)(strchr(chexa,ascii[1])-chexa) ;
  *h=c;
}

void conv_htoa(char* ascii,byte* h)
{
    uint8_t c=*h,d=c>>4,e=c&0x0f;
        ascii[0]=chexa[d];ascii[1]=chexa[e];
}

void conv_htoa(char* ascii,byte* h,uint8_t len)
{
    for(uint8_t i=0;i<len;i++){
      conv_htoa(ascii+2*(len-i-1),(byte*)(h+i));
    }
}

void dumpstr0(char* data,uint8_t len)
{
    char a[]={0x00,0x00,0x00};
    uint8_t c;
    Serial.print("   ");Serial.print((long)data,HEX);Serial.print("   ");
    for(int k=0;k<len;k++){conv_htoa(a,(byte*)&data[k]);Serial.print(a);Serial.print(" ");}
    Serial.print("    ");
    for(int k=0;k<len;k++){
            c=data[k];
            if(c<32 || c>127){c='.';}
            Serial.print((char)c);
    }
    Serial.println();
}

void dumpstr(char* data,uint16_t len)
{
    while(len>=16){len-=16;dumpstr0(data,16);data+=16;}
    if(len!=0){dumpstr0(data,len);}
}

void dumpfield(char* fd,uint8_t ll)
{
    byte a;
    for(int ff=ll-1;ff>=0;ff--){
            a=((fd[ff]&0xF0)>>4)+'0';if(a>'9'){a+=7;}Serial.print((char)a);
            a=(fd[ff]&0x0F)+'0';if(a>'9'){a+=7;}Serial.print((char)a);
            Serial.print(" ");
            //if((fd[ff]&0xF0)==0){Serial.print("0");}
            //Serial.print((byte)fd[ff],HEX);
    }
    Serial.print(" ");
}

#ifdef __arm__
void memDump(char* loc)
{
  void* stackPtr = alloca(4); // This returns a pointer to the current bottom of the stack
  Serial.print("\n====> ");Serial.print(loc);
  Serial.print(" StackPtr ");Serial.print((unsigned long)stackPtr);
  Serial.print(" loc ");Serial.println((unsigned long)loc);
  delay(10);

  dumpstr((char*)loc,16);
  Serial.println()  ;
  dumpstr((char*)stackPtr-64,64);
  Serial.println();
  dumpstr((char*)stackPtr,64);

  delay(10); // serial
}
#endif // __arm__
/*
byte calcBitCrc (byte shiftReg, byte data_bit)
{
  byte fb;

  fb = (shiftReg & 0x01) ^ data_bit;
   // exclusive or least sig bit of current shift reg with the data bit
   shiftReg = shiftReg >> 1;                  // shift one place to the right
   if (fb==1){shiftReg = shiftReg ^ 0x8C;}    // CRC ^ binary 1000 1100
   return(shiftReg);
}
*/
uint8_t calcCrc(char* buf,int len)
{
  uint8_t crc=0,j,k,m;
  int i;

  for(i=0;i<len;i++){
    m=(uint8_t)buf[i];
    for(j=0;j<8;j++){
        k=(crc^m)&0x01;
        crc=crc>>1;
        if(k==1){crc=crc^0x8C;}     // 0x8C is 00011001 rigth rotated polynom
        m=m>>1;
    }
  }
  return crc;
}

byte setcrc(char* buf,int len)
{
  byte c=calcCrc(buf,len);
  conv_htoa(buf+len,&c);buf[len+2]='\0';
  return c;
}


float convStrToNum(char* str,int* sizeRead)
{
  float v=0;
  uint8_t v0=0;
  float pd=1;
  int minu=1;
  int i=0;
#define MAXL 10

  for(i=0;i<MAXL;i++){
    if(i==0 && str[i]=='+'){i++;}
    if(i==0 && str[i]=='-'){i++;minu=-1;}
    if(str[i]=='.'){if(pd==1){pd=10;}i++;}
    *sizeRead=i+1;
    if(str[i]!='_' && str[i]!='\0' && str[i]>='0' && str[i]<='9'){
      v0=*(str+i)-48;
      if(pd==1){v=v*10+v0;}
      else{v+=(float)v0/pd;pd*=10;}
      //Serial.print("str2num=");Serial.print(pd);Serial.print(" ");Serial.print(v,4);Serial.print(" ");Serial.println((char*)&str[i]);
    }
    else {i=MAXL;}
  }
  //Serial.print("s>n str,num=");Serial.print(string);Serial.print(" ");Serial.println(v*minus);
  return v*minu;
}


int32_t convStrToInt(char* str,int* sizeRead)
{
  int32_t v=0;
  int minu=1;

#define MAXLS 12 // max int32 length -4 294 967 296 (+séparator)

  for(int i=0;i<MAXLS;i++){
//Serial.print(i);Serial.print(" ");Serial.print(str);Serial.print(" ");Serial.print((char)str[i]);Serial.print(" ");Serial.println((int)v);

    if(i==0){
        if(str[i]=='+'){i++;}
        else if(str[i]=='-'){i++;minu=-1;}
    }
    *sizeRead=i+1;

    if(str[i]!='_' && str[i]!='\0' && str[i]>='0' && str[i]<='9'){
      v*=10;
      v+=str[i]-'0';
    }
    else {i=MAXLS;}
  }
  //Serial.print("s>n str,num=");Serial.print(string);Serial.print(" ");Serial.println(v*minus);
  return v*minu;
}

boolean compMac(byte* mac1,byte* mac2)
{
  for(int i=0;i<6;i++){if(mac1[i] != mac2[i]){return FAUX;}}
  return VRAI;
}

void packMac(uint8_t* mac,char* ascMac)
{
  for(int i=0;i<6;i++){conv_atoh(ascMac+i*3,mac+i);
  //Serial.print(ascMac+i*3);Serial.print(" ");Serial.println((uint8_t)(mac+i),HEX);
  }
}

void unpackMac(char* buf,byte* mac) //uint8_t* mac)
{
  for(int i=0;i<6;i++){conv_htoa(buf+(i*3),mac+i);if(i<5){buf[i*3+2]='.';}}
    buf[17]='\0';
}

void serialPrintMac(byte* mac,uint8_t nl)
{
  char macBuff[18];
  unpackMac(macBuff,mac);
  Serial.print(macBuff);
  if(nl!=0){Serial.println();}
}


void packDate(char* dateout,char* datein)
{
    for(int i=0;i<6;i++){
        dateout[i]=datein[i*2] << 4 | (datein[i*2+1] & 0x0F);
    }
}

void unpackDate(char* dateout,char* datein)
{
    for(int i=0;i<6;i++){
        dateout[i*2]=(datein[i] >> 4)+48; dateout[i*2+1]=(datein[i] & 0x0F)+48;
    }
}

uint8_t dcb2b(byte val)
{
    return ((val>>4)&0x0f)*10+(val&0x0f);
}

uint32_t cvds(char* d14,uint8_t skip)   // conversion date packée (yyyymmddhhmmss 7 car) en sec
{
    uint32_t secDay=24*3600L;
    uint32_t secYear=365*secDay;
    uint32_t m28=secDay*28L,m30=m28+secDay+secDay,m31=m30+secDay;
    uint32_t monthSec[]={0,m31,monthSec[1]+m28,monthSec[2]+m31,monthSec[3]+m30,monthSec[4]+m31,monthSec[5]+m30,monthSec[6]+m31,monthSec[7]+m31,monthSec[8]+m30,monthSec[9]+m31,monthSec[10]+m30,monthSec[11]+m31};

    uint32_t aa=0;if(skip==0){aa=dcb2b(d14[0])*100L;};aa+=dcb2b(d14[1-skip]);
    uint32_t njb=aa/4L;       // nbre années bisextiles depuis année 0
    uint8_t  mm=dcb2b(d14[2-skip]);
                        if(mm>2 && aa%4==0){njb++;}
    uint32_t bisext=njb*secDay;

    return bisext+aa*secYear+monthSec[(mm-1)]+(dcb2b(d14[3-skip])-1)*secDay
            +dcb2b(d14[4-skip])*3600L+dcb2b(d14[5-skip])*60L+dcb2b(d14[6-skip]);
}

int  dateCmp(char* olddate,char* newdate,uint32_t offset,uint8_t skip1,uint8_t skip2)
{

    uint32_t oldds=cvds(olddate,skip1),newds=cvds(newdate,skip2);

    if((oldds+offset)<newds){return -1;}
    if((oldds+offset)>newds){return 1;}
    return 0;
}

void serialPrintDate(char* datein)
{
    for(int i=0;i<6;i++){
        Serial.print((char)((datein[i] >> 4)+48));Serial.print ((char)((datein[i] & 0x0F)+48));}
        Serial.println();
}

void forceWd()
{
    digitalWrite(pinLed,LOW);
    while(1){};
}

void trigwd(uint32_t durWd)
{
  if(wdEnable){
    int ledState=digitalRead(pinLed);
            
    digitalWrite(pinLed,LEDOFF);    // pour provoquer un flanc
    digitalWrite(pinLed,LEDON);
    wdEnable=false;
    delayMicroseconds(durWd);
    wdEnable=true;
    digitalWrite(pinLed,ledState);
    /*if(ledState==HIGH){digitalWrite(pinLed,HIGH);}
    else digitalWrite(pinLed,LOW);*/
  }
}

void trigwd()
{
  trigwd(2);
}

void lb0()
{
  if(nbreBlink>0){blinktime=millis()-1;cntBlink=nbreBlink+1;digitalWrite(pinLed,LEDOFF);}   // nbreBlink>0 start new cycle
  
  if(millis()>blinktime){
    //Serial.print("led ");
      blinktime=millis();
      if(digitalRead(pinLed)==LEDON){
        //Serial.println("OFF ");
          digitalWrite(pinLed,LEDOFF);                            // extinction
          if(cntBlink>1){blinktime+=FASTBLINK;}
          else {blinktime+=SLOWBLINK;cntBlink=abs(nbreBlink)+1;}  // dernière extinction
          if(cntBlink>MAXBLK){cntBlink-=MAXBLK;}                  // code erreur figé
          cntBlink--;                                         
      }
      else {
        //Serial.println("ON ");
        digitalWrite(pinLed,LEDON);                               // allumage
        blinktime+=PULSEBLINK;
      }
  }  
}

void ledblink(int nbBlk)
{
  if(nbBlk!=0){
    if(nbBlk!=BCODEONBLINK){              // force allumage sans modif cycle en cours
      if(!(nbreBlink>MAXBLK)){nbreBlink=nbBlk;} // si nbre courant > MAXBLK, reste sur le code d'erreur 
      if(nbreBlink%2!=0 && nbreBlink>0){  // si nbBlk impair positif blocage
        nbreBlink=abs(nbreBlink);         // force nbreBlink >0
        lb0();                            // start new cycle
        nbreBlink=-abs(nbreBlink);        // force nbreBlink <0
        while(1){lb0();trigwd();delay(1);}          // infinite loop
      }
    }
    else {                                // force allumage statique jusqu'au pronchain ledblink
      digitalWrite(pinLed,LEDON);
      blinktime=millis()+2;
    }    
  }
  lb0();      // cycle en cours (cntblink - nbreBlink allumages(PULSEBLINK) séparés par extinctions(FASTBLINK) puis 1 extinction(SLOWBLINK) )
              // si nbreBlink >0       déclenchement immédiat 1 cycle (nbreBlink)
              // si nbreBlink >MAXBLK  blocage sur nbreBlink-MAXBLK
              // si nbreBlink 0        rien ne change
              // si nbreBlink <0       déclenchement cycle(nbreBlink) à la fin de SLOWBLINK
}

void blink(uint8_t nb)
{
  for(nb=nb;nb>0;nb--){
    digitalWrite(pinLed,LEDON);delay(5);
    digitalWrite(pinLed,LEDOFF);delay(100);
  }
}

void initLed()
{
  pinLed=PINLED;
  pinMode(pinLed,OUTPUT);
  //wdEnable=true;                  // start trigwd ; trigwd() est dans yield() et ne doit pas être 
                                  // activé avant que PINLED soit mis en OUTPUT
  //trigwd(10000);
  
  nbreBlink=0;
  cntBlink=0;
  blinktime=millis();
}

void timeOvfSet(uint8_t slot)
{
    if(slot<=TIMEOVFSLOTNB){timeOvfSlot[slot-1]=micros();}
}

void timeOvfCtl(uint8_t slot)
{
#ifdef FLAGTIMEOVF
    if(slot<=TIMEOVFSLOTNB){
    Serial.print("tOvf[");Serial.print(slot);
    Serial.print("]=");Serial.println((micros()-timeOvfSlot[slot-1]));
    if((micros()-timeOvfSlot[slot-1])>2000){Serial.print("<<<<<<<<<<<<");}
    }
#endif
}


/* debug

void debug(int cas){
  if(cntdebug[cas]==0 || cas==1){
    Serial.print("debug");Serial.println(cas);
  }
   cntdebug[cas]++;
}

void setdebug(int cas,int* v0,int* v1,char* v2, char* v3)
{
    cntdebug[cas]++;if(cntdebug[cas]<NBDBOC){
    timedebug[cas*NBDBOC+cntdebug[cas]]=micros();
    v0debug[cas*NBDBOC+cntdebug[cas]]=v0;
    v1debug[cas*NBDBOC+cntdebug[cas]]=v1;
    v2debug[cas*NBDBOC+cntdebug[cas]]=v2;
    v3debug[cas*NBDBOC+cntdebug[cas]]=v3;
  }
}

void showdebug()
{
  for(int deb=0;deb<NBDBPTS;deb++){
    Serial.print("point=");Serial.println(deb);
    for(int occ=0;occ<cntdebug[deb];occ++){
      Serial.print("     occ=");Serial.print(occ);
      Serial.print(" ");Serial.print(timedebug[deb*NBDBOC+occ]);
      Serial.print(" ");Serial.print((int)v0debug[deb*NBDBOC+occ]);
      Serial.print(" ");Serial.print((int)v1debug[deb*NBDBOC+occ]);
      Serial.print(" ");Serial.print((char*)v2debug[deb*NBDBOC+occ]);
      Serial.print(" ");Serial.println((char*)v3debug[deb*NBDBOC+occ]);
    }
  }
}

void initdebug()
{
  Serial.print("   *** init debug ***");
  for(int dg=0;dg<NBDBPTS;dg++){
    cntdebug[dg]=0;for(int dp=0;dp<NBDBOC;dp++){timedebug[dg*NBDBOC+dp]=0;}
  }
}
*/


bool ctlpass(char* data,char* model)
{
  return !memcmp(model,data,strlen(model));
}

bool ctlto(unsigned long time,uint16_t to)
{
    //Serial.print("ctlto=");Serial.print(time);Serial.print(" to=");Serial.println(to);
 return (millis()-time)>((uint32_t)to*1000);
}

void startto(unsigned long* time,uint16_t* to,uint16_t valto)
{
  *to=valto;
  *time=millis();
        //Serial.print("startto=");Serial.print(*time);Serial.print(" to=");Serial.print(*to);Serial.print(" valto=");Serial.println(valto);
}

int searchusr(char* usrname)
{
    int nbu=-1,k,pt;
    bool ok=FAUX;
//    Serial.print(usrname);Serial.print(" usernames=");Serial.println(usrnames);

    for (nbu=NBUSR-1;nbu>=0;nbu--){
//        Serial.print("nbu=");Serial.println(nbu);
        for(k=0;k<LENUSRNAME;k++){
//                Serial.print(" ");Serial.print(k);Serial.print(" ");Serial.print(usrname[k]);Serial.print(" ");Serial.println(usrnames[k+nbu*(LENUSRNAME+1)]);
            pt=nbu*(LENUSRNAME+1)+k;
            if(usrnames[pt]=='\0' && k==0){break;}
            else if(usrnames[pt]=='\0'){ok=VRAI;}
            else if(usrnames[pt]!=usrname[k]){break;}
        }
        if(k==LENUSRNAME || ok==VRAI){return nbu;}
    }
    return -1;
}

int serDataAvailable(uint8_t serialNb)
{
  switch(serialNb){
    case 0:return Serial.available();
    case 1:return Serial1.available();
    default: return '\0';
  }
}

char serDataRead(uint8_t serialNb)
{
  switch(serialNb){
    case 0:return Serial.read();
    case 1:return Serial1.read();
    default: return '\0';
  }
}

uint16_t serialRcv(char* rcv,uint16_t maxl,uint8_t serialNb)
/*
  Pour que Serial.available() ne réponde pas vide alors qu'il reste des caractères à recevoir,
  Il faut laisser le temps à ceux-ci d'arriver (!)
  Donc le buffer de réception doit être au moins aussi grand que le message attendu ;
  et il peut être utile de le vider avant usage.
    utile : Serial.getRxBufferSize() Serial.setRxBufferSize(nnnn) par défaut 256
*/
{  
  char inch=RCVSYNCHAR;
  uint8_t lfcnt=0;
  uint16_t lrcv=0;

    while(serDataAvailable(serialNb) && (lfcnt<RSCNB || inch==RCVSYNCHAR)){
      if(lfcnt==0){delay(2);}   // acquisition 
      
      inch=serDataRead(serialNb);
      if(inch==RCVSYNCHAR){
        lfcnt++;}
      else if (lfcnt<RSCNB){lfcnt=0;}
    }
    
    if(lfcnt>=RSCNB){
      *rcv=inch;lrcv++;
      while(serDataAvailable(serialNb) && lrcv<(maxl-1)){
        *(rcv+lrcv)=serDataRead(serialNb);lrcv++;
      }
      *(rcv+lrcv)='\0';
    }
    if(lrcv>0){
      Serial.print("\nreçu=");Serial.print(lrcv);Serial.println(" char");
    }
    return lrcv;
}


#include "Arduino.h"
#include <shconst2.h>

#ifndef PERIF
#ifndef DETS
#include <MemoryFree.h>
#include <FreeStack.h>
#endif // DETS
#endif // PERIF

extern char* chexa;

static unsigned long blinktime=0;
int     nbreBlink=0;          // si nbreBlink impair   -> blocage
int     cntBlink=0;

//#define TIMEOVFSLOTNB 10
//uint32_t timeOvfSlot[TIMEOVFSLOTNB];
/*
extern char  pass[];
extern char* usrnames;
extern char* usrpass;
*/
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

const char* table64="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

void to64(char* srce,char* dest,uint32_t len)   // len dest >= len srce*(1,33)+1
{
  uint8_t a;
  uint32_t i=0;
  uint32_t j=0;
  uint8_t k=len-((len/3)*3);

  for(i=0;i<len-k;i+=3){
    a=srce[i]>>2;dest[j]=table64[a];
    a=(srce[i]&0x03)<<4;a+=srce[i+1]>>4;dest[j+1]=table64[a];
    a=(srce[i+1]&0x0F)<<2;a+=srce[i+2]>>6;dest[j+2]=table64[a];
    a=srce[i+2]&0x3F;dest[j+3]=table64[a];
    j+=4;}
  
  if(k==1){dest[j]=table64[srce[i]>>2];dest[j+1]=table64[(srce[i]&0x03)<<4];}
  if(k==2){
    a=srce[i]>>2;dest[j]=table64[a];
    a=(srce[i]&0x03)<<4;a+=srce[i+1]>>4;dest[j+1]=table64[a];
    a=(srce[i+1]&0x0F)<<2;dest[j+2]=table64[a];}

    //Serial.print("to64 len ");Serial.print(len);Serial.print(" k ");Serial.print(k);Serial.print(" i ");Serial.print(i);
    //Serial.print(" dest :");Serial.println((char*)&dest[j]);
}

int convIntToString(char* str,int num,uint8_t len)
{
  int i=0,t=0,num0=num;
  if(num<0){i=1;str[0]='-';}
  while(num0!=0){num0/=10;i++;}             // comptage nbre chiffres partie entière
  if(len!=0){i=len;}                        // len!=0 complète avec des 0 ou troncate
  t=i;
  for (i=i;i>0;i--){num0=num%10;num/=10;str[i-1]=chexa[num0];}
  str[t]='\0';
  if(str[0]=='\0'){str[0]='0';}
  return t;
}

int convIntToString(char* str,int num)
{
  return convIntToString(str,num,0);
}

int convNumToString(char* str,float num)    // retour string terminée par '\0' ; return longueur totale '\0' inclus
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
  char buf[TEXTIPADDRLENGTH+1];
  memset(buf,0x00,TEXTIPADDRLENGTH+1);
  for(int i=0;i<4;i++){
    sprintf(buf+strlen(buf),"%d",nipadr[i]);if(i<3){strcat(buf,".");}
  }
  memcpy(aipadr,buf,TEXTIPADDRLENGTH);
  if(jsbuf!=nullptr){strcat(jsbuf,buf);strcat(jsbuf,";");}
}

void charIp(byte* nipadr,char* aipadr)
{
  charIp(nipadr,aipadr,nullptr);
}

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

/*bool compMac(byte* mac1,byte* mac2)
{
  for(int i=0;i<6;i++){if(mac1[i] != mac2[i]){return FAUX;}}
  return VRAI;
}*/

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
    }
    Serial.print(" ");
}

#ifdef __arm__
void memDump(char* loc)
{
  /*
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
  */
}
#endif // __arm__

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

int checkData(char* data,uint16_t* lenData)    // controle la taille et le crc d'un message (longueur,crc)
{                            // renvoie MESSOK (1) OK ; MESSCRC (-2) CRC ; MESSLEN (-3)  le message d'erreur est valorisé
  int mess;
  int sizeReadLength;
  uint8_t c=0;
  uint16_t ld;
  if(lenData==nullptr){lenData=&ld;}

  *lenData=(uint16_t)convStrToNum(data,&sizeReadLength);
  conv_atoh(data+*lenData,&c);
  if(*lenData!=(uint16_t)strlen(data)-2){
    mess=MESSLEN;
    
    //dumpstr(data,strlen(data));
    Serial.print(" sizeRead=");Serial.print(sizeReadLength);
    Serial.print(" lenData=");Serial.print(*lenData);Serial.print(" strlen(data)=");Serial.print(strlen(data));
    Serial.print(" CRC,c,len= ");Serial.print(calcCrc(data,*lenData),HEX);Serial.print(", ");
    if(c<16){Serial.print("0");}
    Serial.print(c,HEX);Serial.print(" , ");Serial.println(*lenData);
  }
  else if(calcCrc(data,*lenData)!=c){mess=MESSCRC;}
  else mess=MESSOK;

  return mess;
}

int checkData(char* data)
{
  return checkData(data,nullptr);
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

/*
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
*/

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
/*
bool ctlpass(char* data,char* model)
{
  return !memcmp(model,data,strlen(model));
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
*/

#ifndef NOCONFSER

int serDataAvailable(uint8_t serialNb)
{
  switch(serialNb){
    case 0:return Serial.available();
//#ifndef DETS
    case 1:return SERIALX.available();
//#endif
    default: return Serial.available();;
  }
}

char serDataRead(uint8_t serialNb)
{
  switch(serialNb){
    case 0:return Serial.read();
//#ifndef DETS
    case 1:return SERIALX.read();
//#endif
    default: return Serial.read();;
  }
}

void serPurge(uint8_t serialNb)
{
  unsigned long t=millis();
  uint16_t l=0;
  while((millis()-t)<100){
      if(serDataAvailable(serialNb)){Serial.print(serDataRead(serialNb));l++;t=millis();}
  }
  if(l!=0){Serial.println("endPurge");}
}

uint16_t serialRcv(char* rcv,uint16_t maxl,uint8_t serialNb)
/*
  Tout message commence par des caractères de synchro (RCVSYNCHAR). 
  Il faut en envoyer TSCNB avant un message et en recevoir au moins RSCNB pour débuter une réception
  (les caractères exédentaires sont ignorés)
  Entrée : Si des caractères sont disponibles, le premier caractère utile doit être reçu en TBEGSER mS
           et les suivants en moins de TENDSER par caractère (un délai de TENDSER termine l'attente)
  Retour : le nombre de caractères utiles reçus + le message ;
  
  Le buffer de réception est petit (Serial.getRxBufferSize() - Serial.setRxBufferSize(nnnn) par défaut 256)
  Pour éviter des écrasements, l'émission doit être ralentie (genre 1mS par caractère).
  Il est utile de purger le buffer (serPurge()) avant de demander un envoi.
*/
{  
#define TBEGSER 2000 // délai maxi entre 1er caractère reçu et premier car utile
#define TENDSER 100  // délai maxi pour la réception du caractère utile suivant 

if(!serDataAvailable(serialNb)){return 0;}

  char inch=RCVSYNCHAR;
  uint8_t lfcnt=0;
  uint16_t lrcv=0;
  unsigned long t=millis();
  uint16_t n=0;

    // recevoir au moins RSCNB #
    t=millis();
    while((millis()-t)<TBEGSER && (lfcnt<RSCNB || inch==RCVSYNCHAR)){
      trigwd();
      switch(serialNb){
        case 0:if((n=Serial.available())){inch=Serial.read();}break;
#ifndef DETS
        case 1:if((n=SERIALX.available())){inch=SERIALX.read();}break;
#endif
        default:if((n=Serial.available())){inch=Serial.read();}break;
      }
      if(n!=0){
        if(inch==RCVSYNCHAR){lfcnt++;}
        else if (lfcnt<RSCNB){lfcnt=0;}
      }
    }
    // réception message 
    if((millis()-t)<TBEGSER){                       // pas sortie en TO
      *rcv=inch;lrcv=1;
      t=millis();
      while((millis()-t)<TENDSER && lrcv<MAXSER){
        trigwd();
        switch(serialNb){
          case 0:if(Serial.available()){*(rcv+lrcv)=Serial.read();lrcv++;t=millis();}break;
#ifndef DETS
          case 1:if(SERIALX.available()){*(rcv+lrcv)=SERIALX.read();lrcv++;t=millis();}break;
#endif
          default:if(Serial.available()){*(rcv+lrcv)=Serial.read();lrcv++;t=millis();}break;
        }  
      }
      *(rcv+lrcv)='\0';
    }
    return lrcv;
}


uint16_t setExpEnd(char* bec)     // ajoute longueur au début du message et crc à la fin (pour config série)
{
  uint16_t ll=(uint16_t)strlen(bec);
  sprintf(bec,"%04u",ll);
  bec[4]=';';
  setcrc(bec,ll);
  
  *(bec+ll+2)='\0';
  return (uint16_t)(ll+2);
}
#endif // NOCONFSER
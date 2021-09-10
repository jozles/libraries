#ifndef _UTIL_H_
#define _UTIL_H_

//#ifdef PERIF
//#include <ESP8266WiFi.h>
//#endif // PERIF

uint8_t calcCrc(char* bufCrc,int len);
byte setcrc(char* buf,int len);
void conv_atoh(char* ascii,byte* hex);
void conv_htoa(char* ascii,byte* hex);
void conv_htoa(char* ascii,byte* hex,uint8_t len);
uint8_t conv_atob(char* ascii,uint16_t* bin);
uint8_t conv_atobl(char* ascii,uint32_t* bin);
uint8_t conv_atobl(char* ascii,uint32_t* bin,uint8_t len);
float convStrToNum(char* str,int* sizeRead);
int32_t convStrToInt(char* str,int* sizeRead);
int convIntToString(char* str,int num);
int convNumToString(char* str,float num);  // retour string terminée par '\0' ; return longueur totale '\0' inclus
boolean compMac(byte* mac1,byte* mac2);       // FAUX si != ; VRAI si ==
void packMac(byte* mac,char* ascMac);
void unpackMac(char* buf,byte* mac);
void serialPrintMac(byte* mac,uint8_t nl);
uint8_t textIp(byte* nipadr,byte* buf);
void charIp(byte* nipadr,char* aipadr,char* jsbuf);
void charIp(byte* nipadr,char* aipadr);
//void charIp(IPAddress* nipadr,char* aipadr,char* jsbuf);
//void charIp(IPAddress* nipadr,char* aipadr);
void sp(const char* a,bool ln);
void serialPrintIp(uint8_t* ip);
void packDate(char* dateout,char* datein);
void unpackDate(char* dateout,char* datein);
int  dateCmp(char* olddate,char* newdate,uint32_t offset,uint8_t skip1,uint8_t skip2);
void serialPrintDate(char* datein);
void forceWd();
void trigwd();
void trigwd(uint32_t dur);
void ledblink(int nbBlk);
void blink(uint8_t nb);
//void ledblink(int nb,int mode);
//void ledblink();
//void initBlink();
void initLed();
void dumpstr(char* data,uint16_t len);
void dumpfield(char* fd,uint8_t ll);

bool ctlpass(char* data,char* model);
int  searchusr(char* usrname);
bool ctlto(unsigned long time,uint16_t to);
void startto(unsigned long* time,uint16_t* to,uint16_t valto);

void timeOvfSet(uint8_t slot);
void timeOvfCtl(uint8_t slot);

uint16_t  serialRcv(char* rcv,uint16_t maxl,uint8_t serialNb);
void      serPurge(uint8_t serialNb);
uint16_t  setExpEnd(char* bec);

#ifdef __arm__
void memDump(char* loc);
#endif // __arm__

/*
void initdebug();
void showdebug();
void debug(int cas);
void setdebug(int cas,int* v0,int* v1,char* v2, char* v3);
*/
#endif // _UTIL_H_

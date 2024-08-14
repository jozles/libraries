#include <Arduino.h>

#ifndef _UTIL_H_
#define _UTIL_H_

uint8_t calcCrc(char* bufCrc,int len);
byte setcrc(char* buf,int len);
int checkData(char* data);
int checkData(char* data,uint16_t* lenData);
void conv_atoh(char* ascii,byte* hex);
void conv_htoa(char* ascii,byte* hex);
void conv_htoa(char* ascii,byte* hex,uint8_t len);
uint8_t conv_atob(const char* ascii,uint16_t* bin);
uint8_t conv_atob(const char* ascii,uint16_t* bin,uint8_t len);
uint8_t conv_atobl(const char* ascii,uint32_t* bin);
uint8_t conv_atobl(const char* ascii,uint32_t* bin,uint8_t len);
uint32_t convStrToHex(char* str,uint8_t len);
float convStrToNum(char* str,int* sizeRead);
int32_t convStrToInt(char* str,int* sizeRead);
int convIntToString(char* str,int num);
int convIntToString(char* str,int num,uint8_t len);
int convNumToString(char* str,float num);
//bool compMac(byte* mac1,byte* mac2);
void packMac(byte* mac,char* ascMac);
void unpackMac(char* buf,byte* mac);
void serialPrintMac(byte* mac,uint8_t nl);
uint8_t textIp(byte* nipadr,byte* buf);
void charIp(char* aipadr,char* nipadr,char* jsbuf);
void charIp(char* aipadr,char* nipadr);
void sp(const char* a,bool ln);
void serialPrintIp(uint8_t* ip);
void packDate(char* dateout,char* datein);
void unpackDate(char* dateout,char* datein);
void unpack(char* out,char* in,uint8_t len);
void pack(char* out,char* in,uint8_t inputLen,bool rev);
uint16_t packHexa(const char* out,uint8_t len);
void unpackHexa(uint16_t out,char* in,uint8_t len);
int  dateCmp(char* olddate,char* newdate,uint32_t offset,uint8_t skip1,uint8_t skip2);
void serialPrintDate(char* datein);
void forceWd();
void trigwd();
void trigwd(uint32_t dur);
void ledblink(int nbBlink,uint8_t pulseBlink);
void blink(uint8_t nb);
void initLed(uint32_t led,bool ledlow,bool ledhigh);
void dumpstr(char* data,uint16_t len,bool cr);
void dumpstr(char* data,uint16_t len);
void dumpfield(char* fd,uint8_t ll);
uint16_t ato64(char* srce,char* dest,uint32_t len);
bool diagSetup(unsigned long t_on);

//bool ctlpass(char* data,char* model);
//int  searchusr(char* usrname);
bool ctlto(unsigned long time,uint16_t to);
void startto(unsigned long* time,uint16_t* to,uint16_t valto);

#ifndef NOCONFSER
uint16_t  serialRcv(char* rcv,uint16_t maxl,uint8_t serialNb);
void      serPurge(uint8_t serialNb);
uint16_t  setExpEnd(char* bec);
#endif // NOCONFSER

#ifdef __arm__
void memDump(char* loc);
#endif // __arm__

/*
void timeOvfSet(uint8_t slot);
void timeOvfCtl(uint8_t slot);
*/
/*
void initdebug();
void showdebug();
void debug(int cas);
void setdebug(int cas,int* v0,int* v1,char* v2, char* v3);
*/
#endif // _UTIL_H_

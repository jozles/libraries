#ifndef MESSAGES_H_INCLUDED
#define MESSAGES_H_INCLUDED

#include "Arduino.h"
#include "shconst2.h"


int buildMess(const char* fonction,char* data,const char* sep);
int buildMess(const char* fonction,char* data,const char* sep,bool diags);
int checkData(char* data);
//int checkData(char* data,int* len);
int checkHttpData(char* data,uint8_t* fonction);
const char* periDiag(int8_t diag);

#ifndef PERIF
#include <Ethernet.h>

void purgeServer(EthernetClient* cli);
void purgeServer(EthernetClient* cli,bool diags);
int waitRefCli(EthernetClient* cli,const char* ref,int lref,char* buf,int lbuf);           // attente d'un chaine spécifique dans le flot
int waitRefCli(EthernetClient* cli,const char* ref,int lref,char* buf,int lbuf,bool diags);
int messToServer(EthernetClient* cli,const char* host,int port,char* data);    // connecte au serveur et transfère la data
int getHttpResponse(EthernetClient* cli,char* data,int lmax,uint8_t* fonction);
int getHttpResponse(EthernetClient* cli,char* data,int lmax,uint8_t* fonction,bool diags);
#endif // PERIF

#ifdef PERIF
#include <ESP8266WiFi.h>

void purgeServer(WiFiClient* cli);
void purgeServer(WiFiClient* cli,bool diags);
int waitRefCli(WiFiClient* cli,const char* ref,int lref,char* buf,int lbuf);
int waitRefCli(WiFiClient* cli,const char* ref,int lref,char* buf,int lbuf,bool diags);            // attente d'un chaine spécifique dans le flot
int messToServer(WiFiClient* cli,const char* host,int port,char* data);    // connecte au serveur et transfère la data
int getHttpResponse(WiFiClient* cli,char* data,int lmax,uint8_t* fonction);
int getHttpResponse(WiFiClient* cli,char* data,int lmax,uint8_t* fonction,bool diags);
#endif // PERIF

#endif // MESSAGES_H_INCLUDED

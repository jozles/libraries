#ifndef MESSAGES_H_INCLUDED
#define MESSAGES_H_INCLUDED

#include "Arduino.h"
#include "shconst2.h"

#define NODIAGS false
#define DIAGS   true

int buildMess(const char* fonction,const char* data,const char* sep);
int buildMess(const char* fonction,const char* data,const char* sep,bool diags);
int buildMess(const char* fonction,const char* data,const char* sep,bool diags,bool diagMask);
/*
int checkData(char* data);
int checkData(char* data,uint16_t* lenData);
*/
int checkHttpData(char* data,uint8_t* fonction);
const char* periDiag(int8_t diag);

#if MACHINE_FRONTAL || MACHINE_CONCENTRATEUR

#if MACHINE_FRONTAL
#include <Ethernet.h>
#endif // MACHINE==F

#if MACHINE_CONCENTRATEUR
#include <Ethernet2.h>
#endif // MACHINE==C


void purgeCli(EthernetClient* cli);
void purgeCli(EthernetClient* cli,bool diags);
int waitRefCli(EthernetClient* cli,const char* ref,int lref,char* buf,int lbuf);           // attente d'un chaine spécifique dans le flot
int waitRefCli(EthernetClient* cli,const char* ref,int lref,char* buf,int lbuf,bool diags);
int messToServer(EthernetClient* cli,const char* host,int port,char* data,EthernetServer* server);    // connecte au serveur et transfère la data
int messToServer(EthernetClient* cli,const char* host,int port,char* data);   
int getHttpResponse(EthernetClient* cli,char* data,int lmax,uint8_t* fonction);
int getHttpResponse(EthernetClient* cli,char* data,int lmax,uint8_t* fonction,bool diags);
#endif // MACHINE!='E'

#if MACHINE_ESP
#ifndef ESP32 
#include <ESP8266WiFi.h>
#endif
#ifdef ESP32
#include <WiFi.h>
#endif


void purgeCli(WiFiClient* cli);
void purgeCli(WiFiClient* cli,bool diags);
int waitRefCli(WiFiClient* cli,const char* ref,int lref,char* buf,int lbuf);
int waitRefCli(WiFiClient* cli,const char* ref,int lref,char* buf,int lbuf,bool diags);            // attente d'un chaine spécifique dans le flot
int messToServer(WiFiClient* cli,const char* host,int port,char* data);    // connecte au serveur et transfère la data
int messToServer(WiFiClient* cli,const char* host,int port,char* data,WiFiServer* server,WiFiClient* cliext);    // connecte au serveur et transfère la data
int getHttpResponse(WiFiClient* cli,char* data,int lmax,uint8_t* fonction);
int getHttpResponse(WiFiClient* cli,char* data,int lmax,uint8_t* fonction,bool diags);
#endif // MACHINE==E

#endif // MESSAGES_H_INCLUDED


#include "Arduino.h"
#include <shconst2.h>
#include <shutil2.h>
#include <shmess2.h>


#ifdef PERIF
#include <ESP8266WiFi.h>

extern WiFiClient cli;                 // client local du serveur externe
extern WiFiClient cliext;              // client externe du serveur local

#if CONSTANT==RTCSAVED
extern int cstlen;
#endif //
#endif // // PERIF

#ifndef PERIF
#include <Ethernet.h> //bibliothèque W5100/5500 Ethernet

//extern EthernetClient cli;
/*
extern int16_t*  periNum;                      // ptr ds buffer : Numéro du périphérique courant
extern int32_t*  periPerRefr;                  // ptr ds buffer : période maximale accès serveur
extern uint16_t* periPerTemp;                  // ptr ds buffer : période de lecture tempèrature
extern float*    periPitch;                    // ptr ds buffer : variation minimale de température pour datasave
extern float*    periLastVal;                  // ptr ds buffer : dernière valeur de température
extern float*    periAlim;                     // ptr ds buffer : dernière tension d'alimentation
extern char*     periLastDateIn;               // ptr ds buffer : date/heure de dernière réception
extern char*     periLastDateOut;              // ptr ds buffer : date/heure de dernier envoi
extern char*     periLastDateErr;              // ptr ds buffer : date/heure de derniere anomalie com
extern int8_t*   periErr;                      // ptr ds buffer : code diag anomalie com (voir MESSxxx shconst.h)
extern char*     periNamer;                    // ptr ds buffer : description périphérique
extern char*     periVers;                     // ptr ds buffer : version logiciel du périphérique
extern char*     periModel;                    // ptr ds buffer : model du périphérique
extern byte*     periMacr;                     // ptr ds buffer : mac address
extern byte*     periIpAddr;                   // ptr ds buffer : Ip address
extern uint16_t* periPort;                     // ptr ds buffer : port periph server
extern byte*     periSwNb;                     // ptr ds buffer : Nbre d'interrupteurs (0 aucun ; maxi 4(MAXSW)
extern byte*     periSwVal;                    // ptr ds buffer : état/cde des inter
extern byte*     periInput;                    // ptr ds buffer : Mode fonctionnement inters (MAXTAC=4 par switch)
extern uint32_t* periSwPulseOne;               // ptr ds buffer : durée pulses sec ON (0 pas de pulse)
extern uint32_t* periSwPulseTwo;               // ptr ds buffer : durée pulses sec OFF(mode astable)
extern uint32_t* periSwPulseCurrOne;           // ptr ds buffer : temps courant pulses ON
extern uint32_t* periSwPulseCurrTwo;           // ptr ds buffer : temps courant pulses OFF
extern byte*     periSwPulseCtl;               // ptr ds buffer : mode pulses
extern byte*     periSwPulseSta;               // ptr ds buffer : état clock pulses
extern uint8_t*  periSondeNb;                  // ptr ds buffer : nbre sonde
extern boolean*  periProg;                     // ptr ds buffer : flag "programmable"
extern byte*     periDetNb;                    // ptr ds buffer : Nbre de détecteurs maxi 4 (MAXDET)
extern byte*     periDetVal;                   // ptr ds buffer : flag "ON/OFF" si détecteur (2 bits par détec))
extern float*    periThOffset;                 // ptr ds buffer : offset correctif sur mesure température
extern float*    periThmin;                    // ptr ds buffer : alarme mini th
extern float*    periThmax;                    // ptr ds buffer : alarme maxi th
extern float*    periVmin;                     // ptr ds buffer : alarme mini volts
extern float*    periVmax;                     // ptr ds buffer : alarme maxi volts
extern byte*     periDetServEn;                // ptr ds buffer ; 1 byte 8*enable detecteurs serveur

extern byte      periMacBuf[6];
extern int8_t    periMess;                     // code diag réception message (voir MESSxxx shconst.h)
*/
#endif // // ndef PERIF

extern char*  chexa;

extern char*  fonctions;
extern int    nbfonct;
extern char   bufServer[LBUFSERVER];

extern byte   mac[6];

extern byte memDetServ;  // image mémoire NBDSRV détecteurs (8)

const char*   periText={TEXTMESS};

#ifndef PERIF
void purgeServer(EthernetClient* cli){purgeServer(cli,true);}
void purgeServer(EthernetClient* cli,bool diags)
#endif // // PERIF
#ifdef PERIF
void purgeServer(WiFiClient* cli){purgeServer(cli,true);}
void purgeServer(WiFiClient* cli,bool diags)
#endif // // PERIF
{
    if(cli->connected()){
        //Serial.print(" purge ");
        while (cli->available()){char c=cli->read();if(diags){Serial.print(c);}}
        if(diags){Serial.println();}
    } 
    cli->stop();
}

int buildMess(const char* fonction,const char* data,const char* sep)
    {return buildMess(fonction,data,sep,true);}

int buildMess(const char* fonction,const char* data,const char* sep,bool diags)   // concatène un message dans bufServer
{                                                                     // retourne la longueur totale dans bufServer ou 0 si ovf
      if((strlen(bufServer)+strlen(data)+11+5+2+1)>LBUFSERVER)
        {return 0;}
      strcat(bufServer,fonction);
      strcat(bufServer,"=");
      int sb=strlen(bufServer);
      int d=strlen(data)+5;    //+strlen(sep);
      sprintf(bufServer+sb,"%04d",d);
      strcat(bufServer+sb+4,"_");
      strcat(bufServer+sb+5,data);
      setcrc(bufServer+sb,d);
      strcat(bufServer,sep);
      if(diags){Serial.print(" bS=");Serial.println(bufServer);//Serial.print(" bm=");Serial.println(millis());
      }
      return strlen(bufServer);
}

#ifndef PERIF
int messToServer(EthernetClient* cli,const char* host,int port,char* data)    // connecte au serveur et transfère la data
#endif //
#ifdef PERIF
int messToServer(WiFiClient* cli,const char* host,const int port,char* data)    // connecte au serveur et transfère la data
#endif //
{
  int x=0,v=MESSOK,repeat=0;

#ifndef PERIF
    //purgeServer(cli);
#endif // // PERIF

  Serial.print("cx serveur (");Serial.print(x);Serial.print(") ");
  Serial.print(host);Serial.print(":");Serial.print(port);
  Serial.print("...");

  cli->stop();                        // assure la disponibilité de l'instance avant usage
  x=cli->connect(host,port);
  while(!x && repeat<7){

    #ifndef PERIF
    trigwd();
    #endif

    v=MESSOK;
    repeat++;
    
    x=cli->connected();
    //Serial.print("(");Serial.print(x);Serial.print(")");
    if(!x){
        switch(x){
            case -1:Serial.print("time out ");break;
            case -2:Serial.print("invalid server ");break;
            case -3:Serial.print("truncated ");break;
            case -4:Serial.print("invalid response ");break;
            default:break;
        }
        #ifndef PERIF
        Serial.print("status_ap=");Serial.print(cli->status_ap);Serial.print(" ");
        Serial.print("sockx=");Serial.print(cli->sockx_ap);Serial.print(" ");
        #endif // PERIF
        delay(500);Serial.print(repeat);Serial.println(" échouée");v=MESSCX;
    }
  }
  if(v==MESSOK){
        Serial.println(" ok");
        //cli->print(data);
        cli->write(data);
        //cli->write("\r\n HTTP/1.1\r\n Connection:close\r\n\r\n"); // inutile pour serveur sh
  }
  return v;
}

#ifndef PERIF
int waitRefCli(EthernetClient* cli,const char* ref,int lref,char* buf,int lbuf)   // attente d'un chaine spécifique dans le flot
    {return waitRefCli(cli,ref,lref,buf,lbuf,true);}
int waitRefCli(EthernetClient* cli,const char* ref,int lref,char* buf,int lbuf,bool diags)
#endif // PERIF
#ifdef PERIF
int waitRefCli(WiFiClient* cli,const char* ref,int lref,char* buf,int lbuf)       // attente d'un chaine spécifique dans le flot
    {return waitRefCli(cli,ref,lref,buf,lbuf,true);}
int waitRefCli(WiFiClient* cli,const char* ref,int lref,char* buf,int lbuf,bool diags)
#endif // PERIF
// wait for ref,lref    si lbuf<>0 accumule le flot dans buf (ref incluse)
// sortie MESSTO (0) time out   MESSDEC (-1) décap     MESSOK (1) OK
{
  boolean termine=FAUX;
  int ptref=0,ptbuf=0;
  char inch;
  unsigned long timerTo=millis()+TOFINCHCLI;


      if(diags){
            Serial.print(" attente =");Serial.print(ref);
            Serial.print(" ");}                                 //Serial.print(lref);Serial.print(" ");}
      while(!termine){
        
        #ifndef PERIF
        trigwd();
        #endif

        if(cli->available()>0){
          timerTo=millis()+TOINCHCLI;
          inch=cli->read();if(diags){Serial.print(inch);}
          if(lbuf!=0){
            if(ptbuf<lbuf){buf[ptbuf]=inch;ptbuf++;}else {return MESSDEC;}}
          if(inch==ref[ptref]){
            ptref++;if(ptref>=lref){termine=VRAI;}}
          else{ptref=0;}
        }
        else if(millis()>=timerTo){return MESSTO;}
      }
      if(lbuf!=0){buf[ptbuf-lref]='\0';}
      //if(diags){Serial.println();}
      return MESSOK;
}

/*int checkData(char* data)
{
    int l;
    checkData(data,&l);
}
*/

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

int checkHttpData(char* data,uint8_t* fonction)   // checkData et extraction de la fonction
{
    char noms[LENNOM+1];
    int mess=0;

    memcpy(noms,data,LENNOM);noms[LENNOM]='\0';
    mess=checkData(data+LENNOM+1);                // +1 skip '='
    if(mess==MESSOK){
        *fonction=(strstr(fonctions,noms)-fonctions)/LENNOM;
        //Serial.print("\nnbfonct=");Serial.print(nbfonct);Serial.print(" fonction=");Serial.print((strstr(fonctions,noms)-fonctions));Serial.print("/");Serial.print(*fonction);
        if(*fonction>=nbfonct || *fonction<0){mess=MESSFON;}
    }

    return mess;
}

void waitRefCliDiag(bool diags,int pM)
{
  if(diags){Serial.print(" waitRefCli pM=");Serial.print(pM);Serial.print("-");Serial.print(millis());}
}

#ifndef PERIF
int getHttpResponse(EthernetClient* cli, char* data,int lmax,uint8_t* fonction)
    {return getHttpResponse(cli,data,lmax,fonction,true);}
int getHttpResponse(EthernetClient* cli, char* data,int lmax,uint8_t* fonction,bool diags)  // attend un message d'un serveur ; ctle longueur et crc
#endif //  PERIF
#ifdef PERIF
int getHttpResponse(WiFiClient* cli, char* data,int lmax,uint8_t* fonction)
    {return getHttpResponse(cli,data,lmax,fonction,true);}
int getHttpResponse(WiFiClient* cli, char* data,int lmax,uint8_t* fonction,bool diags)      // attend un message d'un serveur ; ctle longueur et crc
#endif //  PERIF
// format <body>contenu...</body></html>\n\r                        la fonction est décodée et les données sont chargées
// contenu fonction__=nnnn_datacrc                                  renvoie les codes "MESSxxx"
{
  const char* body="<body>\0";
  const char* bodyend="</body>\0";
  int q=0;

  q=waitRefCli(cli,body,strlen(body),bufServer,0,diags);
  waitRefCliDiag(diags,q);
  if(q==MESSOK){
        q=waitRefCli(cli,bodyend,strlen(bodyend),data,lmax-strlen(bodyend),diags);
        waitRefCliDiag(diags,q);
  }
  if(q==MESSOK){
        q=checkHttpData(data,fonction);
        if(diags){Serial.print(" checkHttpData pM=");Serial.println(q);}
  }
  return q;
}

const char* periDiag(int8_t diag)
{
  int v=0;
  if(diag>NBMESS){diag=MESSSYS;}
  //if(diag!=MESSOK){
        v=LPERIMESS*(-(diag-1));//} // diag -n -> 1 devient 0->n-1
  return periText+v;
}

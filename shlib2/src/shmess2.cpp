#ifndef DETS

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

//extern uint8_t sock;

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

const char*   periText={TEXTMESS};

#ifndef PERIF
void purgeCli(EthernetClient* cli){purgeCli(cli,true);}
void purgeCli(EthernetClient* cli,bool diags)
#endif // // PERIF
#ifdef PERIF
void purgeCli(WiFiClient* cli){purgeCli(cli,true);}
void purgeCli(WiFiClient* cli,bool diags)
#endif // // PERIF
{
    if(cli->connected()){
        if(diags){Serial.print(" purge :");}
        while (cli->available()){char c=cli->read();if(diags){Serial.print(c);}}
        if(diags){Serial.println();}
    } 
    //cli->stop(); // ne doit pas être effectué de façon générique... ici purge seule
}

int buildMess(const char* fonction,const char* data,const char* sep)
    {return buildMess(fonction,data,sep,false);}

int buildMess(const char* fonction,const char* data,const char* sep,bool diags)
    {return buildMess(fonction,data,sep,diags,false);}

int buildMess(const char* fonction,const char* data,const char* sep,bool diags,bool diagMask)   // concatène un message dans bufServer
{                                                                     // retourne la longueur totale dans bufServer ou 0 si ovf
      int v=0;
      if((strlen(bufServer)+strlen(data)+11+5+2+1)<=LBUFSERVER){
        strcat(bufServer,fonction);
        strcat(bufServer,"=");
        int sb=strlen(bufServer);
        int d=strlen(data)+5;    //+strlen(sep);
        sprintf(bufServer+sb,"%04d",d);
        strcat(bufServer+sb+4,"_");
        strcat(bufServer+sb+5,data);
        setcrc(bufServer+sb,d);
        strcat(bufServer,sep);
        if(diags && !diagMask){Serial.print(" bS=");Serial.println(bufServer);//Serial.print(" bm=");Serial.println(millis());
        }
        v=strlen(bufServer);
      }  

      Serial.print(" bM:");Serial.print(v);Serial.print(' ');delay(1);
      return v;
}

#ifndef PERIF
int messToServer(EthernetClient* cli,const char* host,int port,char* data,EthernetServer* server,EthernetClient* cliext)    // connecte au serveur et transfère la data
#endif //
#ifdef PERIF
int messToServer(WiFiClient* cli,const char* host,const int port,char* data,WiFiServer* server,WiFiClient* cliext)    // connecte au serveur et transfère la data
#endif //
{
  Serial.print("mTS ");

  int x=0,v=MESSOK,repeat=0;

  #ifdef ANALYZE
  MESTOS 
  #endif // ANALYZE

/*   noServerCall : pour serveur vérifier l'utilité 
*    (probablement pour débug des pb de cx aux périfs-server)
*    concerne uniquement les périphériques : server=nullptr si frontal
*    vérifie l'absence de demande pendante
*/
bool noServerCall=true;

#ifndef PERIF
  if(server!=nullptr){
    *cliext=server->available();
    if(*cliext){noServerCall=false;v=MESSSRV;}} // message reçu non traité ---> anomalie

  if(noServerCall){
#endif    

    Serial.print(" cx to ");Serial.print(host);Serial.print(":");Serial.print(port);Serial.print("...");
    cli->stop();                          // assure la disponibilité de l'instance avant usage 
                                          // (en principe inutile, messToServer utilisé lors de débuts de négociation)
#ifndef PERIF
    Serial.print("avant cli->connect cli_socket:");Serial.print(cli->sockindex);
#endif
    x=cli->connect(host,port);
#ifndef PERIF
    Serial.print("après cli->connect cli_socket:");Serial.print(cli->sockindex);Serial.print("(");Serial.print(cli->waitTime_ap);Serial.print(") status_ap:");Serial.print(cli->status_ap);Serial.print(" sockx_ap:");Serial.println(cli->sockx_ap);
#endif
    #define MAXCXTRY 4

    while(!x && repeat<MAXCXTRY && noServerCall){          // (en principe fonctionne du premier coup ou pas)
      #ifndef PERIF
      trigwd();
      #endif

        repeat++;
    
        trigwd();
        x=cli->connected();        
        if(!x){
          /*switch(x){
            case -1:Serial.print("time out ");break;
            case -2:Serial.print("invalid server ");break;
            case -3:Serial.print("truncated ");break;
            case -4:Serial.print("invalid response ");break;
            default:break;
          }*/
          #ifndef PERIF
          Serial.println();
          Serial.print(repeat);
          Serial.print(" (");Serial.print(cli->waitTime_ap);
          Serial.print(") status_ap=");Serial.print(cli->status_ap);Serial.print(" ");
          Serial.print("sockx=");Serial.print(cli->sockx_ap);Serial.print(" ");
          #endif // PERIF
          
          v=MESSCX;
          unsigned long tto=millis();
          while((millis()-tto)<100 && repeat<MAXCXTRY){        // delay 100mS avant retry
            trigwd();
            yield();
            if(server!=nullptr){            // voir plus haut
              *cliext=server->available();
              if(*cliext){noServerCall=false;v=MESSSRV2;break;}
            }
          }
        } // !x
        else v=MESSOK;
    }     // while not connected
    #ifndef PERIF
    Serial.print("sockx=");Serial.print(cli->sockx_ap);
    #endif
    if(v==MESSOK){
      trigwd();
      Serial.print(" ok ");
      cli->write(data);      //cli->write("\r\n HTTP/1.1\r\n Connection:close\r\n\r\n"); // inutile pour serveur sh
    }
    else {
      trigwd();
      cli->stop();     // libération socket
      Serial.print(" ko ");
    }

#ifndef PERIF
  }   // noServerCall before first connection attempt
#endif  
#ifdef ANALYZE
  STOPALL
#endif // ANALYZE
  if(v==MESSSRV){Serial.println("server call");}
  //Serial.print("outofMTS v=");Serial.println(v);
  return v;
}

#ifndef PERIF
int messToServer(EthernetClient* cli,const char* host,int port,char* data)    // connecte au serveur et transfère la data
#endif //
#ifdef PERIF
int messToServer(WiFiClient* cli,const char* host,const int port,char* data)    // connecte au serveur et transfère la data
#endif //
{
  return messToServer(cli,host,port,data,nullptr,nullptr);
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
  bool termine=FAUX;
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
      if(lbuf!=0){buf[ptbuf-lref]='\0';}    // ptbuf>lref sinon MESSTO
      //if(diags){Serial.println();}
      return MESSOK;
}

int checkHttpData(char* data,uint8_t* fonction)   // checkData et extraction de la fonction
{
    char noms[LENNOM+1];
    int mess=MESSOK;

    memcpy(noms,data,LENNOM);noms[LENNOM]='\0';
    mess=checkData(data+LENNOM+1);                // +1 skip '='
    if(mess==MESSOK){
        char* wf=strstr(fonctions,noms);
        unsigned long pf=(wf-fonctions)/LENNOM;
        if(wf==nullptr || (int)pf>=nbfonct){mess=MESSFON;}
        else *fonction=(uint8_t)pf;
        //Serial.print("\nnbfonct=");Serial.print(nbfonct);Serial.print(" fonction=");Serial.print((strstr(fonctions,noms)-fonctions));Serial.print("/");Serial.print(*fonction);
    }

    return mess;
}

void waitRefCliDiag(bool diags,int pM)
{
  //if(diags){Serial.print(" waitRefCli pM=");Serial.print(pM);Serial.print("-");Serial.print(millis());}
}

#ifndef PERIF
int getHttpResponse(EthernetClient* cli, char* data,int lmax,uint8_t* fonction)
    {return getHttpResponse(cli,data,lmax,fonction,false);}
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

#ifdef ANALYZE
  GHTTPR
#endif // ANALYZE

  const char* body="<body>\0";
  const char* bodyend="</body>\0";
  int q=0;

  q=waitRefCli(cli,body,strlen(body),bufServer,0,diags);
  if(q==MESSOK){
  #ifdef ANALYZE
    STOPALL
    GHTTPR
  #endif // ANALYZE  
        q=waitRefCli(cli,bodyend,strlen(bodyend),data,lmax-1,diags);
  }
  if(q==MESSOK){
        q=checkHttpData(data,fonction);
        if(diags){Serial.print(" checkHttpData pM=");Serial.print(q);Serial.print(" f=");Serial.println(*fonction);}
  }
#ifdef ANALYZE
  STOPALL
#endif // ANALYZE
    
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

#endif // ndef DETS
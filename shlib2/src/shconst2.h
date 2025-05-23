#ifndef _SHCONST_H_
#define _SHCONST_H_

// params provenant de platformio.ini // 
//    REDV0 // REDV1      //

#define LENVERSION  4
#define LENMODEL    6
//#define SIZEPULSE   4  // uint32_t
#define LENPERIDATE 6           // LDATEASCII packé
#define LDATEASCII 12           // YYMMDDHHMMSS
#define TEXTIPADDRLENGTH 15


/* /////////// format des adresses IP ///////// 
    
    Les périphériques WiFi les stockent en format IPAddress 
    le serveur en byte[4]
    anciennement messToServer utilisait une adresse texte sur 16 caractères ; 
    acuellement au format IPAddress ce qui permet d'éviter la demande de DNS 
    ci-après les différentes conversions (noter le format petit indien pour uint32_t)

  //IPAddress IpHost;
  //IpHost.fromString(host);                // convert 15 bytes ascii address (aaa.aaa.aaa.aaa) to IPAdress format
  //uint8_t ip[4]={192,168,0,52};
  //IPAddress IpTest;IpTest=ip;Serial.print("test ip ");Serial.println(IpTest);  // convert uint8_t[4] to IPAddress format
  //byte ip[4]={192,168,0,52};
  //IPAddress IpTest;IpTest=ip;Serial.print("test ip ");Serial.println(IpTest);    // convert byte[4] to IPAddress format
  //uint32_t ip;ip=192+168*256+52*256*256*256;                                   // petit indien
  //IPAddress IpTest;IpTest=ip;Serial.print("test ip ");Serial.println(IpTest);  // convert uint32_t to IPAddress format
*/

//#define _MODE_DEVT  // _MODE_RUN _MODE_DEVT // force et change l'adresse Mac de la carte red, l'adresse IP (via DHCP de la box) et le port (en accord avec redir de port de la box)
#define _MODE_RUN  // _MODE_RUN _MODE_DEVT // force et change l'adresse Mac de la carte red, l'adresse IP (via DHCP de la box) et le port (en accord avec redir de port de la box)

#ifdef _MODE_DEVT  
  #define IP_FRONTAL IPAddress(192,168,0,35)      // server DEVT
  #define PORT_FRONTAL 1790                       // server DEVT
#endif // _MODE_DEVT
#ifdef _MODE_RUN
  #define IP_FRONTAL IPAddress(192,168,0,36)      // server RUN
  #define PORT_FRONTAL 1786                       // server RUN !!! n° du port pour périfs, +1 browser, +2 remotes
#endif // _MODE_RUN

#if MACHINE_ESP
 //#define PINLED  0                    //  0 = ESP-12  ; 2 = ESP-01 ;
 //#define LEDON LOW
 //#define LEDOFF HIGH
  #define SERIALX Serial

#endif // MACHINE=='E'

#if !MACHINE_ESP
 
    #ifdef REDV0
        #define STOPREQ 3          // push button pour stop request
        #define PINLED  2          // 2 sur redshieldV0 // 13 sur proto
        #define PULSEBLINK 4       // durée blink
        #define LEDON HIGH
        #define LEDOFF LOW
        #define SDCARD 4           // toujours 4 pour compatibilité lib.
    #endif // RED1.0

    #ifdef REDV1
        #define AP2112             // red shield 1.2 and later
        #define STOPREQ 2          // push button pour stop request
        #define PINLED  3          // 2 sur redV0 // 13 sur proto
        #define PULSEBLINK 4       // durée blink
        #define LEDON HIGH
        #define LEDOFF LOW
        #define SDCARD 4           // toujours 4 pour compatibilité lib.
        #define POWCD 5            // power command W5500/SD/RADIO 
        #ifndef AP2112             
            #define POWON  LOW
            #define POWOFF HIGH
        #endif // AP2112
        #ifdef AP2112
            #define POWON  HIGH
            #define POWOFF LOW
        #endif // AP2112

        #ifdef DUE
            #define WIRE Wire1
            #define SERIALX Serial1
            #define SERNB 1
        #endif  // DUE
        #ifndef DUE                // NUCLEO
            #define WIRE Wire
            #define SERIALX Serial2
            #define SERENB 1
        #endif // DUE
    #endif // RED1.1

    #ifdef DETS
        #define SERIALX Serial
        #define PINLED  4          // 2 sur redshieldV0 // 13 sur proto
        #define PULSEBLINK 4       // durée blink
        #define LEDON HIGH
        #define LEDOFF LOW
        #define STOPREQ 14 // 5         // PD5
        #define SERIALX Serial
        #define SERNB 0
    #endif // DETS

#endif // MACHINE!='E'


#ifdef ANALYZE
// IN CASE OF MESS WITH ANALYZE FLAG, JUST COMMENT IT in platformio.ini
#ifdef DUE
    #define ANPIN0 A5
    #define ANPIN1 A6
    #define ANPIN2 A7
#endif // DUE
#if MACHINE_ESP
    #define ANPIN0 13
    #define ANPIN1 16
    #define ANPIN2 10
#endif // MACHINE=='E'
#define AN0 ANPIN0  // A5
#define AN1 ANPIN1  // A6
#define AN2 ANPIN2  // A7
// toujours un LOW en premier ppour eviter les glitchs à 1 sur la sortie de la porte 
#if !MACHINE_ESP
#define PERIQ   digitalWrite(AN2,LOW);digitalWrite(AN1,HIGH);digitalWrite(AN0,HIGH);  // 110 periReq
#endif // MACHINE!='E'
#define MESTOS  digitalWrite(AN0,LOW);digitalWrite(AN1,LOW);digitalWrite(AN2,HIGH);    // 001 messToServer
#define GHTTPR  digitalWrite(AN0,LOW);digitalWrite(AN1,LOW);digitalWrite(AN2,LOW);    // 000 getHttpResponse
#define STOPALL digitalWrite(AN0,HIGH);digitalWrite(AN1,HIGH);digitalWrite(AN2,HIGH); // end
#endif // ANALYZE



    // protocoles

#define PROTOCHAR " TU"
#define PROTOCSTR "   \0TCP\0UDP\0"
#define LENPROSTR 4
#define NBPROTOC  2

#define UNIXDATELEN 4                   // uint32_t

#define MACADDRLENGTH 6

    // messages

#define LENMESS     692+1               // longueur buffer message v1.6
#define MPOSFONC    0                   // position fonction
#define MPOSLEN     11                  // position longueur (longueur fonction excluse)
#define MPOSNUMPER  MPOSLEN+5           // position num périphérique
#define MPOSMAC     MPOSNUMPER+3        // position adr
#define MPOSDH      (MPOSMAC+MACADDRLENGTH*3) // Date/Heure       // (MACCADDRLENGTH*2)+(MACADDRLENGTH-1 les points) +1 '_')
#define MPOSPERREFR MPOSDH+15           // période refr
#define MPOSTEMPPER MPOSPERREFR+6       // période check température
#define MPOSPITCH   MPOSTEMPPER+6       // pitch
#define MPOSSWCDE   MPOSPITCH+5         // MAXSW commandes sw 0/1 + 1sep (periSwCde)
#define MPOSANALH   MPOSSWCDE+MAXSW+1   // analogLow,analogHigh 2*(4digits) + 1 sep
#define MPOSPULSONE MPOSANALH+8+1       // Timers tOne (4bytes)*4
#define MPOSPULSTWO MPOSPULSONE+NBPULSE*(LENVALPULSE+1)       // Timers tTwo (4bytes+1sep)*4
#define MPOSPULSCTL MPOSPULSTWO+NBPULSE*(LENVALPULSE+1)       // paramètres timers (4*3 bits=2 bytes)
#define MPOSPERRULES MPOSPULSCTL+2*PCTLLEN+1                  // rules
#define MPOSMDETSRV MPOSPERRULES+NBPERRULES*PERINPLEN*2+1     // detecteurs serveur
#define MPOSPORTSRV MPOSMDETSRV+MDSLEN*2+1                    // port perif serveur
#define MPOSMDIAG   MPOSPORTSRV+5                             // texte diag
#define MLMSET      MPOSMDIAG+5          // longueur message fonction incluse

    // fonctions

/*      message GET envoyé par periTable peut envoyer 251 fonctions (maxi version 1.1k)
        le buffer strSD de l'enregistrement d'historique doit pouvoir le contenir + entete et pied
        même pb pour valeurs, nvalf et libfonctions
        valeurs et strSD n'ont pas une longueurs fixe pour chaque fonction donc contrôle de décap

*/
#define LENNOM  10    // nbre car nom de fonction
#define NBVAL  256    // nbre maxi fonct/valeurs pour getnv() --- les noms de fonctions font 10 caractères + 2 séparateurs + la donnée associée = LENVAL
                      // taille macxi pour GET (IE)=2048  ; pour POST plusieurs mega
                      // avec un arduino Atmel la limitation vient de la taille de la ram
#define LENVAL 256    // nbre car maxi valeur (vérifier datasave)
#define MEANVAL  6    // pour donner un param de taille à valeurs[]
#define LENVALEURS NBVAL*MEANVAL+1   // la taille effective de valeurs[]
//#define LENPSW  16    // nbre car maxi pswd

/* erreurs getnv */
#define GETNV_NO_FN       -1    // pas de fonction
#define GETNV_INVALID_FN1 -2    // nom fonction inconnu
#define GETNV_INVALID_FN2 -3    // nom fonction (caractère invalide)
#define GETNV_FNNB_OVF    -4    // trop de fonctions
#define GETNV_RCD_OVF     -5    // valeurs trop longues
#define GETNV_INVALID_VAL -6    // valeur invalide (caractère interdit)
#define GETNV_INV_SEP     -7    // séparateur invalide

#define RECHEAD 28                           // en-tete strSD date/heure/xx/yy + <br> + crlf
#define RECCHAR NBVAL*(MEANVAL+3)+RECHEAD+8  // longueur maxi record histo
#define UDPBUFLEN NBVAL*(MEANVAL+12)+6+1 //LENCDEHTTP // longueur maxi buffer paquet UDP

#define LBUFSERVER LENMESS+LENNOM+1+4+1+2+1 // longueur bufserver (messages in/out periphériques)
                                            // + nom fonct+1+longueur+1+crc+1
//#define LBUFUDP LBUFSERVER 
/* positions fixées dans strSD (fhisto) */

#define HISTOPOSLEN    7   // position longueur ligne / ';'
#define HISTOPOSNUMPER 9   // position numéro périphérique / ';'
#define HISTOPOSMAC    12  // position addr mac    / ';'
#define HISTOPOSTEMP   30  // position température / ';'

/* >>>> mots de passe <<<< */

#define SRVPASS     "17515A" // pwd accès serveur pour péri
#define PERIPASS    SRVPASS
//#define USRPASS     "17515A" // pwd accès serveur via browser
//#define MODPASS     "17515A" // pwd modif serveur via browser
#define LPWD        8
#define LENUSRPASS  8
#define NBUSR       8
#define LENUSRNAME  16
#define LMAILPWD    16
#define LMAILADD    32
#define LMAILMESS   128      // max len mess

/* >>>> config série <<<< */

  #define RCVSYNCHAR '#'
  #define RSCNB 3                   // min Rx config char
  #define TSCNB RSCNB*2             // Tx config char
  #define WIFICFG   "srvconf___"
  #define CONCCFG   "concconf__"
  #define PERICFG   "periconf__"
  #define MAXSER  200

/* >>>> WIFI <<<<< */

#define MAXSSID   4
#define LENSSID   16
#define LPWSSID   48
#define SSID2     "pinks"
#define PWDSSID2  "cain ne dormant pas songeait au pied des monts"
#define SSID1     "devolo-5d3"
#define PWDSSID1  "JNCJTRONJMGZEEQL"

// concentrateurs

#define MAXCONC   4            // nombre concentrateurs/serveur

#define CC_UDP0   55556        // def conc Udp port 
#define CC_UDP1   55557
#define CC_UDP2   55558
#define CC_UDP3   55559

#define CHANNEL0    120         // def radio channel devrait être dans nrf24l01s.h (nécessaire pour frontal2/periph.cpp)
#define CHANNEL1    110         // def radio channel devrait être dans nrf24l01s.h (nécessaire pour frontal2/periph.cpp)
#define CHANNEL2    100         // def radio channel devrait être dans nrf24l01s.h (nécessaire pour frontal2/periph.cpp)
#define CHANNEL3    90          // def radio channel devrait être dans nrf24l01s.h (nécessaire pour frontal2/periph.cpp)

#define CC_SPEED    0           // RF_SPEED_1Mb (voir nrf24l01s.h) devrait être dans nrf24l01s.h (nécessaire pour frontal2/periph.cpp)

#define CC_ADDRX   "rashc\0rbshc\0rcshc\0rdshc\0" // adresses mac par défaut des concentrateurs                                                 
                                                // le premier car doit avoir le bit 0 à 0 pour que le dhcp fonctionne (?)


//#define RADIO_ADDR_LENGTH 5     // doit être == NRF_ADDR_LENGTH // commun pour sketch radio et frontal 
// modif de la valeur compliquée à mettre en oeuvre
//#define CC_NRF_ADDR "SHCO0"     // def conc addr (0)            // commun pour sketch radio et frontal 
/*
#define MAX_PAYLOAD_LENGTH 32   // doit être == NRF_MAX_PAYLOAD_LENGTH
*/

#define TOINCHCLI   100         // msec max attente car server
#define TOFINCHCLI  1000        // msec max attente 1er car server ... devrait etre réduit

#define SLOWBLINK 3000
#define FASTBLINK 300         // 200    // 350
//#define PULSEBLINK 10         // plutôt dépendant de la période de la loop sur
//                              // périf NO_MODE

/* codes blink 
*  valeurs impaires bloquantes
*  valeurs paires < MAXBLK jouées une fois puis le prochain ledblink reprogramme
*  valeurs paires > MAXBLK jouées à perpétuité sans blocage
*  valeurs négatives normales 
*  BCODEONBLINK allume inconditionnellement jusqu'au prochain ledblink */

#define MAXBLK            20  // > MAXBLK codes erreur non bloquants fixes
#define BCODEONBLINK      98  // allume jusqu'au prochain blink
#define BCODEPERIRECLEN   3   // PERIRECLEN ou LENMESS ou LBUFSERVER (ou autre) trop petit -> blocage
#define BCODEPBNTP        (2+MAXBLK)   // pas de service NTP
#define BCODEORDREXT      4   // un ordreExt() a été reçu
#define BCODEWAITWIFI     (4+MAXBLK)   // attente WIFI
#define BCODESDCARDKO     5   // pas de SDCARD ; config KO ; file HS
#define BCODEFHISTO       (6+MAXBLK)   // write 0 car (sdstore_textdh0)
#define BCODELENVAL       7   // LENVAL trop petit
#define BCODECONFIGRECLEN 9   // CONFIGRECLEN ou MLMSET/LENMESS faux -> blocage
#define BCODEPERICACHEKO 11   // periSave et cache invalide
#define BCODESYSERR      13   // system error (fdatasave>99 ; STEPDATASAVE!=talkStep ; MDSLEN invalide)
#define BCODENUMPER      15   // tentative de periLoad d'un perif invalide
#define BCODESHOWLINE    17   // buffer showline overflow / ordreExt overflow

enum {FAUX,VRAI};
enum {OFF,ON};
enum {NOPRINT,PRINT};
//const char*  chexa="0123456789ABCDEFabcdef\0";

/* description périphériques */

#define MAXSW   4       // nbre maxi switchs par périphérique
#define MAXDET  4       // nbre maxi détecteurs physiques par périphérique
#define NBANST  5       // nbre d'états pour l'entrée analogique
//#define MAXDSP  4       // nbre maxi détecteurs spéciaux
//#define MAXDEX  8       // nbre maxi détecteurs externes
#define MAXSDE  4         // nbre maxi sondes par périphérique
#define MAXTAC  4         // 4 types actions sur pulses (start/stop/mask/force)
#define NBPULSE 4

/* bits periCfg */

#define PERI_SERV   0x01  // periCfg flag serveur  
#define PERI_ANAL   0x02  // periCfg flag analogique
#define PERI_RAD    0x04  // commande radiateur
#define PERI_STA    0x08  // thermostat

// messages diag

#define MESSOK    1
#define MESSTO    0
#define MESSDEC  -1
#define MESSCRC  -2
#define MESSLEN  -3
#define MESSFON  -4
#define MESSFULL -5
#define MESSSYS  -6
#define MESSCX   -7
#define MESSNUMP -8
#define MESSMAC  -9
#define MESSSRV  -10
#define MESSSRV2 -11

#define NBMESS   11  // OK ne compte pas

#define TEXTOK      "*OK_"
#define TEXTTO      "*TO_"
#define TEXTDEC     "*OVF"
#define TEXTCRC     "*CRC"
#define TEXTLEN     "*LEN"
#define TEXTFON     "*FON"
#define TEXTFULL    "*FUL"
#define TEXTSYS     "*SYS"
#define TEXTCX      "*CX_"
#define TEXTNUMP    "*NUP"
#define TEXTMAC     "*MAC"
#define TEXTSRV     "*SRV"

//#define TEXTMESS TEXTOK,TEXTTO,TEXTDEC,TEXTCRC,TEXTLEN,TEXTFON,TEXTFULL,TEXTSYS,TEXTCX,TEXTNUMP,TEXTMAC
#define TEXTMESS " OK \0*TO_\0*OVF\0*CRC\0*LEN\0*FON\0*FUL\0*SYS\0*CX_\0*NUP\0*MAC\0"

#define LPERIMESS  5   // len texte diag en réception de message

/* bits structures definitions */

/* SWITCHS CONTROL */

// PULSE MODE = PM
// time one = TO ; time two = TT ; current = C ; duration = D
// Server = SR ; free run = FR ; Phase H/L-LH = PH
// enable = E ; UP/DOWN = U ; H/L = H
// number = N ; detector on = DI ; off = DO ; ON = I ; OFF = O
// most significant = MS ; least = LS
//

/* nombre détecteurs externes */

#define NBDSRV 64               // nombre de det serveur (supporté par périf v1.y ; pour serveur 1.5a MDSRV 32, MDSLEN 4 )
#define MDSLEN 8                // (NBDSRV/8) DOIT ETRE MULTIPLE DE 4 ; len memDetServ

/* bits memDetec */

#define  DETBITLH_VB  0x01 // 1 bit état HIGH LOW
#define  DETBITLH_PB  0
#define  DETBITUD_VB  0x02 // 1 bit flanc/état du déclenchement
#define  DETBITUD_PB  1
#define  DETBITST_VB  0x0C // 2 bits état (déclenché(TRIG)/attente(WAIT)/disable(DIS)) si DIS le bit d'état est invalide
#define  DETBITST_PB  2
#define  DETTRIG      0x03 // valeur DETBIST si déclenché (LH=L falling =H rising)
#define  DETWAIT      0x02 // valeur         si armé      (LH=H falling =L rising)
#define  DETIDLE      0x01 // valeur         si pas d'interruption (LH valide)
#define  DETDIS       0x00 // valeur         si disable (LH invalide)

// codage car différenciation de fonction

#define PMFNCVAL    0X30    // car d'offset ('0') de la valeur de fonction (*valf)

#define PMFNCHAR    0x40    // NE PAS MODIFIER !! le codage des shifts
// de periSwPulseCtl est critique : utilisation des lettres maj et min seules
// car d'offset ('@') dans nom de fonction


// debug
#define NBDBPTS 4
#define NBDBOC  5

/* description pulses */

#define LENVALPULSE 8



/* codes staPulse */

#define LENTSP 4

#define PM_DISABLE  0x06
#define PM6 'DISA'
#define PM_RUN2     0x05
#define PM5 'RUN2'
#define PM_RUN1     0x04
#define PM4 'RUN1'
#define PM_END2     0x03
#define PM3 'END2'
#define PM_END1     0x02
#define PM2 'END1'
#define PM_IDLE     0x01
#define PM1 'IDLE'

/* pulseMode */

#define PCTLLEN (((NBPULSE*PCTLBIT)/8)+1) //  4*3 bits =12 -> 2 bytes/perif
#define PCTLBIT 3                         // 3 bits/pulse

#define PMTOE_VB 0x04           // time one enable valeur du bit
#define PMTOE_PB 0x02           //                 position du bit (nombre shifts)
#define PMTTE_VB 0x02           // time two enable
#define PMTTE_PB 0x01
#define PMFRO_VB 0x01           // freeRun/OneShoot
#define PMFRO_PB 0x00

/* bits controle compteurs */

#define PM_FREERUN  0
#define PM_ONESHOOT 1
#define PM_ACTIF    1


/* définition table règles switchs (ex inputs) */

#define NBPERRULES 48               // Maxi possible pour Eprom 512 bytes des ESP8266
//#define NBPERRULES 24               // pour serveur 1.5a et perif 1.y
#define PERINPLEN  4

// byte 0 & byte 3 -- num détecteur + type (byte 0 source / byte 3 dest ; 6 bits numéro + 2 bits type dsrv/mem/phys-swi/pulse)

#define PERINPV_MS    0xFC          // mask n° det (6 bits - 64 possibles)
#define PERINPNVMS_VB 0x80          // détec MSb
#define PERINPNVMS_PB 0x07
#define PERINPNVLS_VB 0x04          // détec LSb
#define PERINPNVLS_PB 0x02
#define PERINPNT_MS   0x03          // mask type (2 bits - 4 possibles)
#define PERINPNTMS_VB 0x02          // type MSb
#define PERINPNTMS_PB 0x01
#define PERINPNTLS_VB 0x01          // type LSb
#define PERINPNTLS_PB 0x00

// byte 1 -- règles --- inutilisé

#define PERINPRULESLS_VB 0x01       // rules LSb
#define PERINPRULESLS_PB 0x00

// byte 2 -- act/usage/level/enable

#define PERINPACT_MS    0xF0        // mask action - 4 bits (16 possibles)
#define PERINPACTMS_VB  0x80        // action MSb (4 bits)
#define PERINPACTMS_PB  0x07
#define PERINPACTLS_VB  0x10        // action LSb
#define PERINPACTLS_PB  0x04
#define PERINPDETES_VB  0x08        // usage detec level (0=edge 1=static)
#define PERINPDETES_PB  0x03
#define PERINPVALID_VB  0x04        // actif level (0 rise/high 1 fall/low )
#define PERINPVALID_PB  0x02
#define PERINPOLDLEV_VB 0x02        // prev detec level
#define PERINPOLDLEV_PB 0x01
#define PERINPEN_VB     0x01        // enable
#define PERINPEN_PB     0x00


/* types détecteurs */

#define NBDTYP 4

#define DETYEXT 1
#define DETYPHY 2
#define DETYPUL 3
#define DETYSW  2
#define DETYMEM 0


/* codes et libellés actions */

#define LENTACT 5            // len libellé
#define NBACT   16           // nbre actions différentes

#define PMDCA_VIDE  0x00     //
#define ACT0 '_____'
#define PMDCA_RAZ   0x01     // raz
#define ACT1 'R','A','Z',' ',' '
#define PMDCA_STOP  0x02     // stop  clk
#define ACT2 'STOP '
#define PMDCA_START 0x03     // start clk
#define ACT3 'START'
#define PMDCA_SHORT 0x04     // short pulse
#define ACT4 'SHORT'
#define PMDCA_END   0x05     // end pulse
#define ACT5 'END  '
#define PMDCA_IMP   0x06     // start/stop impulsionnel
#define ACT6 'IMP  '
#define PMDCA_RESET 0x07     // reset
#define ACT7 'RESET'
#define PMDCA_LXOR  0x08     // logical xor
#define ACT8 'XOR  '
#define PMDCA_LOR   0x09     // logical or
#define ACT9 'OR   '
#define PMDCA_LAND  0x0A     // logical and
#define ACT10 'AND  '
#define PMDCA_LNOR  0x0B     // logical nor
#define ACT11 'NOR  '
#define PMDCA_LNAND 0x0C     // logical nand
#define ACT12 'NAND '
#define PMDCA_0     0x0D     // force value 0
#define ACT13 '-0-  '
#define PMDCA_1     0x0E     // force value 1
#define ACT14 '-1-  '
#define PMDCA_FORCE 0x0F     // force valeur srce dans dest
#define ACT15 'FORCE'

#define MAXACT 15

#endif //  _SHCONST_H_

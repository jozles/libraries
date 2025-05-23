#ifndef NRF24L01P_H_INCLUDED
#define NRF24L01P_H_INCLUDED

#include <Arduino.h>

#include "nRF24L01.h"       // mnemonics
#include "nrf24l01s_const.h"

/* AP NFR24L01+ node single */

/*
 * Fonctionnement du NRF24L01+ :
 *
 *  au démarrage, le bit PWR_UP et CE_LOW
 *  place le chip en mode standby en maxi 4,5mS
 *
 *  pour transmettre :
 *
 *    (a)charger le FIFO, bit PRIM_RX_low
 *    (b)charger TX_ADDR (et RX_ADDR_P0 si auto ACK)
 *    (c)CE high lance la transmission aprés un delais de 130uS
 *    (d)attendre la fin de la transmission (TX_DS set ou MAX_RT set
 *                                        effacer TX_DS et MAX_RT)
 *    (e)retour en mode standby avec CE low.
 *
 *    fonctions :
 *    Write()     (a)(b)(c)
 *                sort du mode prx
 *                charge le FIFO, les adresses P0 et TX (si 'C'), fait setTx() et flushTx()
 *                et termine avec CE_HIGH
 *    transmitting() (d)(e)
 *                teste la fin de transmission, et passe en mode prx via CE_LOW
 *
 *  pour recevoir :
 *
 *    (f)bit PRIM_RX_high, CE_HIGH commence l'écoute aprés un delais de 130uS
 *    (g)attendre un paquet (RX_DR set ou fifo RX_EMPTY clr ; effacer RX_DR)
 *    (h)controler n° pipe et longueur reçue
 *    (i)retour en standby avec CE low
 *    (j)charger le message reçu
 *
 *    fonctions :
 *    available() (f)(g)(h)
 *                recharge RX_ADDR_P0 si nécessaire (vidage FIFO dans ce cas)
 *                passe en mode prx (pwrUpTx+CE_HIGH) si nécessaire au premier test
 *                contrôle la longueur maxi, transf�re le message
 *                et récupère la longueur reçue et le n° de pipe
 *                termine avec CE_LOW si true (fin du mode prx)
 *    Read()      (f)(g)(h)(i)
 *                effectue available() à la demande
 *
 *
 *             ***** pas d'utilisation des pipes *****
 * RX_P1 du concentrateur reçoit tous les périphériques et emmet avec TX et RX_P0
 * RX_P2 du concentrateur reçoit les requêtes d'adresse de concentrateur des périphériques qui s'apparient
 * RX_P1 des périphériques reçoit les messages du concentrateurs ; TX/RX_P0 fixes sur concentrateur
 * 
 *
 *  périphériques :
 *
 *      macAddr unique allouée au pipe1 (pour mode PRX)
 *      pRegister() envoie la macAddr et reçoit le numT 
 *      n° d'entrée de la table où la macAddr du périphérique est stockée
 *      
 *  concentrateur :
 *
 *      cRegister() reoit la macAddr et renvoie le numT
 *
 *
 *
 *
 */

#define POWONDLY 100+10   // millis()  - 100 -
#define POWUPDLY 5+1      // millis()

#define ACK     true
#define NO_ACK  false

#define NB_PIPE 2         // nombre pipes utilisées
#define NRF_MAX_PAYLOAD_LENGTH 32
#define NRF_ADDR_LENGTH 5

/*
#define CHANNEL0    120         // def radio channel
#define CHANNEL1    110        
#define CHANNEL2    100        
#define CHANNEL3    90         

#define CC_SPEED    0           // RF_SPEED_1Mb (voir nrf24l01s.h)
*/

#define TO_AVAILABLE 20   // millis()
#define TO_REGISTER  20   // millis()

#define RF_SPD_2MB RF_DR_HIGH_BIT
#define RF_SPD_1MB 0
#define RF_SPD_250K RF_DR_LOW_BIT



/*** logic analyzer debug pulse ***/

#ifdef  DETS        
  #define PP4       bitClear(PORT_PP,BIT_PP);bitSet(PORT_PP,BIT_PP);
  #define PP4_INIT  bitSet(PORT_PP,BIT_PP);bitSet(DDR_PP,BIT_PP);
  #define PP4_OFF   bitClear(DDR_PP,BIT_PP);
  #define PP4_HIGH  bitSet(DDR_PP,BIT_PP);bitSet(PORT_PP,BIT_PP);
  #define PP4_LOW   bitSet(DDR_PP,BIT_PP);bitClear(PORT_PP,BIT_PP);  
#endif // DETS
#ifndef DETS
  #define PP4       digitalWrite(PORT_PP,LOW);digitalWrite(PORT_PP,HIGH);
  #define PP4_INIT  pinMode(PORT_PP,OUTPUT);
#endif // ndef DETS


#if MACHINE_CONCENTRATEUR

#endif // MACHINE == 'C'


class Nrfp
{
  public:
    Nrfp();

    void setup(uint8_t channel,uint8_t speed,uint8_t nbperif,byte* cbAddr);

    int  available(uint8_t* pipe,uint8_t* length);
    int  read(byte* data,uint8_t* pipe,uint8_t* length,int numP);
    void readStop();
    void write(byte* data,bool ack,uint8_t len,byte* macTableAddr);
    int  transmitting(bool ack);

    //void powerOn(uint8_t channel);
    void powerOn(uint8_t channel,uint8_t speed,uint8_t nbperif,byte*cbAddr);
    void powerOff();
    void powerUp();
    void powerDown();
    void regRead(uint8_t reg,byte* data);
    void addrWrite(uint8_t reg,byte* data);
    void rxError();

    void printAddr(char* addr,char n);

    bool    powerD=true;      // etat power (true=down)
    uint8_t lastSta=0;
    byte*   locAddr;          // local Addr
    byte*   cbAddr;        

#if MACHINE_DET328
    byte*   ccAddr; 
    int     pRegister(byte* message,uint8_t* pldLength);
    int     txRx(byte* message,uint8_t* pldLength);
#endif // MACHINE == 'P'

  private:

    void regWrite(uint8_t reg,byte* data);
    void addrRead(uint8_t reg,byte* data);
//    void addrWrite(uint8_t reg,byte* data);
    void letsPrx();

    void flushRx();
    void flushTx();
    void setRx();
    void setTx();

    //int tableCLoad();
    //int tableCSave();

    
    bool prxMode=false;       // true=circuit en PRX (pwrUpRx(), CE_HIGH)

    uint8_t nbPerif=0;

    uint8_t regw,statu,fstatu,conf;

#if MACHINE_DET328
    void allPinsLow();
#endif // MACHINE == 'P'
};

#endif // NRF24L01P INCLUDED

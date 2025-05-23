
#include <SPI.h>
#include "nrf24l01s.h"

#include "nrf24l01s_const.h"
//#include "radio_const.h"
//#include "radio_user_conc.h"

/* AP NFR24L01+ node single */

#ifdef DETS        // idem for PRO MINI
#define CSN_HIGH  bitSet(PORT_CSN,BIT_CSN);
#define CSN_LOW   bitClear(PORT_CSN,BIT_CSN);
#define CE_HIGH   bitSet(PORT_CE,BIT_CE);delayMicroseconds(10);
// 10uS is minimal pulse ; 4uS to CSN low
#define CE_LOW    bitClear(PORT_CE,BIT_CE);
#define CSN_INIT  bitSet(DDR_CSN,BIT_CSN);
#define CSN_OFF   bitClear(DDR_CSN,BIT_CSN);
#define CE_INIT   bitSet(DDR_CE,BIT_CE);
#define CE_OFF    bitClear(DDR_CE,BIT_CE);
#endif // DETS

/*#ifdef DUE
#define CSN_HIGH  bitSet(PORTC,29);
#define CSN_LOW   bitClear(PORTC,29);
#define CE_HIGH   bitSet(PORTC,21);delayMicroseconds(10);
#define CE_LOW    bitClear(PORTC,21);
#endif // DUE */

#ifndef CSN_HIGH
#define CSN_HIGH  digitalWrite(CSN_PIN,HIGH);
#define CSN_LOW   digitalWrite(CSN_PIN,LOW);
#define CE_HIGH   digitalWrite(CE_PIN,HIGH);delayMicroseconds(8);
#define CE_LOW    digitalWrite(CE_PIN,LOW);
#define CE_INIT   pinMode(CE_PIN,OUTPUT);
#define CE_OFF    pinMode(CE_PIN,INPUT);
#define CSN_INIT  pinMode(CSN_PIN,OUTPUT);
#define CSN_OFF   pinMode(CSN_PIN,INPUT);
#endif // CSN_HIGH

#ifdef SPI_MODE      
#if MACHINE_DET328
#define SPI_INIT    SPI.beginTransaction(SPISettings(4000000,MSBFIRST,SPI_MODE0));
#endif //
#if MACHINE_CONCENTRATEUR
#define SPI_INIT    SPI.beginTransaction(SPISettings(8000000,MSBFIRST,SPI_MODE0));
#endif //
#define SPI_START   SPI.begin();
#define SPI_OFF     SPI.end();//pinMode(MOSI_PIN,INPUT);pinMode(CLK_PIN,INPUT);
#endif // SPI_MODE

#ifdef DETS
#define GET_STA     CSN_LOW;statu=SPI.transfer(NOP);CSN_HIGH        
#endif // DETS
#ifndef DETS
#define GET_STA     digitalWrite(CSN_PIN,LOW);statu=SPI.transfer(NOP);CSN_HIGH //digitalWrite(CSN_PIN,HIGH);
#endif // DETS

#define CLR_TXDS_MAXRT  regw=TX_DS_BIT|MAX_RT_BIT;regWrite(STATUS,&regw);
#define CLR_RXDR        regw=RX_DR_BIT;regWrite(STATUS,&regw);


#define CONFREG (0x00 & (~MASK_RX_DR_BIT) & (~MASK_TX_DS_BIT) & (~MASK_MAX_RT_BIT)) | ((EN_CRC_BIT) & (~CRCO_BIT)) | (PWR_UP_BIT) | (PRIM_RX_BIT)

#define RFREG   RF_PWR_BITS

/***** config *****/

Nrfp::Nrfp()    // constructeur
{
}

void Nrfp::setup(uint8_t channel,uint8_t speed,uint8_t nbperif,byte* cbAddr)
{ 
    /* registers */

    regw=EN_DYN_ACK_BIT | EN_DPL_BIT;  // ack enable ; dyn pld length enable
    regWrite(FEATURE,&regw);

//    regw=(NRF_ADDR_LENGTH-2)<<AW;       // addresses width
//    regWrite(SETUP_AW,&regw);

//    regw=9;//NRF_MAX_PAYLOAD_LENGTH;        // set payload length
//    regWrite(RX_PW_P0,&regw);

//    regw=9;//NRF_MAX_PAYLOAD_LENGTH;        // set payload length
//    regWrite(RX_PW_P1,&regw);

//    regw=(ERX_P1_BIT|ERX_P0_BIT);   // R0,R1 seuls (ERX_P5_BIT|ERX_P4_BIT|ERX_P3_BIT|ERX_P2_BIT|ERX_P1_BIT|ERX_P0_BIT);
//    regWrite(EN_RXADDR,&regw);
//    regw=(ENAA_P1_BIT|ENAA_P0_BIT); // ACK //(ENAA_P5_BIT|ENAA_P4_BIT|ENAA_P3_BIT|ENAA_P2_BIT|ENAA_P1_BIT|ENAA_P0_BIT);
//    regWrite(EN_AA,&regw);          // ENAA nécessaire pour DPL
    regw=(DPL_P1_BIT|DPL_P0_BIT);   // (DPL_P5_BIT|DPL_P4_BIT|DPL_P3_BIT|DPL_P2_BIT|DPL_P1_BIT|DPL_P0_BIT);
    regWrite(DYNPD,&regw);          // dynamic payload length

    addrWrite(RX_ADDR_P1,locAddr);  // RXP1 = macAddr du circuit pour réception messages dans pipe 1
    addrWrite(RX_ADDR_P2,cbAddr);  // RXP2 = pipe 2 pour recevoir les demandes d'adresse de concentrateur
                                    // (conc only ; chargée en EEPROM sur périf)
    regw=channel;
    regWrite(RF_CH,&regw);

    regw=RFREG | speed;
  
    regWrite(RF_SETUP,&regw);

    regw=(ARD_VALUE<<ARD)+(ARC_VALUE<<ARC);
    regWrite(SETUP_RETR,&regw);

    flushRx();
    flushTx();

    regw=TX_DS_BIT | MAX_RT_BIT | RX_DR_BIT; // clear bits TX_DS , MAX_RT , RX_DR
    regWrite(STATUS,&regw);

    prxMode=false;

    nbPerif=nbperif;
}

/************ utilitary ***************/

void Nrfp::regRead(uint8_t reg,byte* data)
{
    CSN_LOW
    SPI.transfer((R_REGISTER | (REGISTER_MASK & reg)));
    *data=SPI.transfer(*data);
    CSN_HIGH
}

void Nrfp::regWrite(uint8_t reg,byte* data)
{
    CSN_LOW
    SPI.transfer((W_REGISTER | (REGISTER_MASK & reg)));
    SPI.transfer(*data);
    CSN_HIGH
}

void Nrfp::addrRead(uint8_t reg,byte* data)
{
    CSN_LOW
    SPI.transfer((R_REGISTER | (REGISTER_MASK & reg)));
    SPI.transfer(data,NRF_ADDR_LENGTH);
    CSN_HIGH
}

void Nrfp::addrWrite(uint8_t reg,byte* data)
{
    CSN_LOW
    SPI.transfer((W_REGISTER | (REGISTER_MASK & reg)));
    for(uint8_t i=0;i<NRF_ADDR_LENGTH;i++){SPI.transfer(data[i]);}
    CSN_HIGH
}

#if MACHINE_DET328
void Nrfp::allPinsLow()                     /* all radio/SPI pins low */
{
  bitClear(PORT_CE,BIT_CE);       //digitalWrite(CE_PIN,LOW);
  bitSet(DDR_CE,BIT_CE);          //pinMode(CE_PIN,OUTPUT);
  
  bitClear(PORT_CLK,BIT_CLK);     //digitalWrite(CLK_PIN,LOW);
  bitSet(DDR_CLK,BIT_CLK);        //pinMode(CLK_PIN,OUTPUT);
      
  bitClear(PORT_CSN,BIT_CSN);     //digitalWrite(CSN_PIN,LOW);
  bitSet(DDR_CSN,BIT_CSN);        //pinMode(CSN_PIN,OUTPUT);
  
  bitClear(PORT_MOSI,BIT_MOSI);   //digitalWrite(MOSI_PIN,LOW);
  bitSet(DDR_MOSI,BIT_MOSI);      //pinMode(MOSI_PIN,OUTPUT);  
}
#endif // MACHINE_DET328 

void Nrfp::powerOn(uint8_t channel,uint8_t speed,uint8_t nbperif,byte* cbAddr)
{
#if MACHINE_DET328
#if PER_PO == 'P'

  allPinsLow();                   // in order to minimize power during POWONDLY
      
  digitalWrite(RPOW_PIN,LOW);     // power on
  pinMode(RPOW_PIN,OUTPUT);

  delay(POWONDLY);                // powerOn delay ******************** mettre en sleep *********************

  bitSet(PORT_CSN,BIT_CSN);       //digitalWrite(CSN_PIN,HIGH);

#endif // PER_PO
#endif // MACHINE_DET328
#if ((MACHINE_CONCENTRATEUR) || (PER_PO == 'N'))

  digitalWrite(CSN_PIN,HIGH);
  pinMode(CSN_PIN,OUTPUT);

  digitalWrite(CE_PIN,LOW);
  pinMode(CE_PIN,OUTPUT);

#ifdef SPI_MODE  
  digitalWrite(CLK_PIN,LOW);
  pinMode(CLK_PIN,OUTPUT);
  digitalWrite(MOSI_PIN,LOW);
  pinMode(MOSI_PIN,OUTPUT);
  digitalWrite(MOSI_PIN,HIGH);  
#endif //
  
#endif // (MACHINE_CONCENTRATEUR) || (PER_PO == 'N')

  powerUp();
  setup(channel,speed,nbperif,cbAddr);                        // registry inits 
}

/*void Nrfp::powerOn(uint8_t channel)
{
  return powerOn(channel,RF_SPD_1MB);
}*/

void Nrfp::powerOff()
{
#if MACHINE_DET328
  
  allPinsLow();

  digitalWrite(RPOW_PIN,HIGH);    // power off
  pinMode(RPOW_PIN,OUTPUT);

#endif // MACHINE='P'  
}

void Nrfp::powerUp()
{   
#ifdef SPI_MODE
    SPI_INIT;
    SPI_START;
#endif // SPI_MODE

    conf=CONFREG;                         // powerUP/CRC 1 byte/PRX
    regWrite(CONFIG,&conf);
    regWrite(CONFIG,&conf);               // twice !!!

    flushRx();
    flushTx();
    CLR_RXDR

    powerD=false;

    delay(POWUPDLY);       // powerUp delay
}

void Nrfp::powerDown()
{
    if(!powerD){            // si power down, ça bloque de refaire...        
        powerD=true;
     
        CE_LOW

        conf=(CONFREG) & (~PWR_UP_BIT);          // powerDown
        regWrite(CONFIG,&conf);

#ifdef SPI_MODE
        SPI_OFF
#endif // SPI_MODE              
    }
}

void Nrfp::setTx()
{
    conf=(CONFREG) & (~PRIM_RX_BIT);
    regWrite(CONFIG,&conf);
}

void Nrfp::setRx()
{
    conf=(CONFREG) | (PRIM_RX_BIT);
    regWrite(CONFIG,&conf);}


void Nrfp::flushTx()
{
    CSN_LOW
    SPI.transfer(FLUSH_TX);
    CSN_HIGH
}

void Nrfp::flushRx()
{
    CSN_LOW
    SPI.transfer(FLUSH_RX);
    CSN_HIGH
}

void Nrfp::letsPrx()      // goto PRX mode
{
    if(!prxMode){
      CE_LOW              // to change from TX to RX
      //flushRx();
      //CLR_RXDR
      setRx();
      prxMode=true;
    }
    CE_HIGH
//    PP4_LOW
}

void Nrfp::rxError()
{
        CE_LOW
        flushRx();                         // if error flush all
        CLR_RXDR
        prxMode=false;
}

/********** public *************/

void Nrfp::write(byte* data,bool ack,uint8_t len,byte* macTableAddr)  // write data,len to numP if 'C' mode or to CCADDR if 'P' mode
{
    uint8_t llen=len; // NRF_MAX_PAYLOAD_LENGTH;

    prxMode=false;
    CE_LOW

#if MACHINE_CONCENTRATEUR
    addrWrite(TX_ADDR,macTableAddr);    // PER_ADDR);       
    addrWrite(RX_ADDR_P0,macTableAddr); // PER_ADDR);
#endif // MACHINE == 'C'

#if MACHINE_DET328
    addrWrite(TX_ADDR,ccAddr);
    addrWrite(RX_ADDR_P0,ccAddr);
#endif // MACHINE == 'P'

    setTx();
    flushTx();
    CLR_TXDS_MAXRT

    CSN_LOW
    if(ack){SPI.transfer(W_TX_PAYLOAD);}          // with ACK
    else   {SPI.transfer(W_TX_PAYLOAD_NA);}       // without ACK

    for(uint8_t i=0;i<llen;i++){SPI.transfer(data[i]);}
    CSN_HIGH

    CE_HIGH                                 // transmit (CE high->TX_DS 235uS @1MbpS)
    
// transmitting() should be checked now to wait for end of paquet transmission
// or ACK reception before turning CE low
}

int Nrfp::transmitting(bool ack)         // busy -> 1 ; sent -> 0 -> Rx ; MAX_RT -> -1
{     // should be added : TO in case of out of order chip (trst=-2)
      // when sent or max retry, output in PRX mode with CE high

      int trst=1;
      
      GET_STA

      if((statu & (TX_DS_BIT | MAX_RT_BIT))!=0){

        trst=0;
        if(statu & MAX_RT_BIT){
          if(!ack){Serial.print("\nsyst err maxrt without ack ");Serial.println(statu,HEX);delay(2);}
          trst=-1;} 

        CLR_TXDS_MAXRT
        letsPrx();
      }
      
      return trst;
}

int Nrfp::available(uint8_t* pipe,uint8_t* pldLength)
{
/* returns ( 0 full (valid length) ; <0 empty, pipe err or length error) 
   only receiving in pipe 1 ...
*/

    uint8_t maxLength=*pldLength; // NRF_MAX_PAYLOAD_LENGTH; //
    int err=0;

    if(!prxMode){letsPrx();}

    GET_STA
    lastSta=statu;

    if((statu & RX_DR_BIT)==0 && (statu & RX_P_NO_BIT)==RX_P_NO_BIT){                   // RX && FIFO empty ?
      return AV_EMPTY;}

    // RX full : either (statu & RX_DR_BIT)!=0
    //           either (fstatu & RX_EMPTY_BIT)==0) && ((statu & RX_P_NO_BIT)>>RX_P_NO)==1

    *pipe=(statu & RX_P_NO_BIT)>>RX_P_NO;         // get pipe nb (1 or 2) ... if 2 conc name req ; if >2 ... trouble
   
    if(*pipe==1 || *pipe==2){
        CSN_LOW
        SPI.transfer(R_RX_PL_WID);
        *pldLength=SPI.transfer(0xff);            // get pldLength (dynamic length)
        CSN_HIGH      
    }
    else {err=AV_EMPTY;} //AV_NBPIP;}             // ---------------- pipe nb error
    
    if(((*pldLength>maxLength) || (*pldLength<=0)) && err==0){
      err=AV_LMERR;}                              // ---------------- pldLength error

    if(err!=0){rxError();}
    return err;                                   // =0 not empty ; <0 error : invalid pipe nb or length
}

int Nrfp::read(byte* data,uint8_t* pipe,uint8_t* pldLength,int numP)
{
   
    /* mode 'C' returns  0  registration to do 
                        >0  valid data table entry nb 
                        <0  empty, pipe err, length err, mac addr table error
       mode 'P' returns =0  full
                        <0  empty, pipe err or length error
    */
    // numP<NBPERIF means "available() allready done with result ok && *pipe set")
    // PRX mode still true if no error

    if(numP>=nbPerif){
        numP=available(pipe,pldLength);                     // returns 0:pld_ok <0:err
    }

    if(numP>=0){                                            // pipe 1 or 2 pld available

        CSN_LOW
        SPI.transfer(R_RX_PAYLOAD);
        SPI.transfer(data,*pldLength);                      // get pld
        CSN_HIGH
    }

    if(numP!=AV_EMPTY){CLR_RXDR }    // ne pas faire CLR_RXDR si numP==empty : un message a pu arriver depuis la lecture du status
    return numP;    // CE stay HIGH in case of subsequent packets 
                    // (so read() caller has to put CE low if rx completed)
}

void Nrfp::readStop()
{
  CE_LOW
  prxMode=false;
}

#if MACHINE_DET328
int Nrfp::pRegister(byte* message,uint8_t* pldLength)  // peripheral registration to get pipeAddr
{                      // ER_MAXRT ; AV_errors codes ; >=0 numP ok
    /*      message doit être chargé avec adresseMac et version au minimum pour que le concentrateur renvoie le bon format
    memset(message,0x00,NRF_MAX_PAYLOAD_LENGTH+1);
    memcpy(message,locAddr,NRF_ADDR_LENGTH);
    message[NRF_ADDR_LENGTH]='0';
    write(message,NO_ACK,NRF_ADDR_LENGTH+1,0);     // send macAddr + numP=0 to ccAddr ; no ACK
    */
    write(message,NO_ACK,*pldLength,0);     // send macAddr + numP=0 to ccAddr ; no ACK
 
#ifndef DETS
    int trst=1;
    while(trst==1){trst=transmitting(NO_ACK);}
    if(trst<0){return ER_MAXRT;}              // MAX_RT error should not happen (no ACK mode)
                                              // radio card HS or missing
#endif // ndef DETS
      
#ifdef DETS
// version accélérée pour minimiser le délai entre TX_DS et setRx() 
// (jusqu'à 40uS en compil release ; moins de 20uS accéléré)
    GET_STA
    conf=(CONFREG) | (PRIM_RX_BIT);           // ready pour setRx()
    while((statu & (TX_DS_BIT | MAX_RT_BIT))==0){GET_STA}

    if(statu & MAX_RT_BIT){
      Serial.print("\nsyst err maxrt without ack ");Serial.println(statu,HEX);delay(2);
      return ER_MAXRT;} 

    CE_LOW              // to change from TX to RX
    regWrite(CONFIG,&conf);                   // setRx()
    CE_HIGH
    prxMode=true;

// fin version accélérée
#endif // def DETS

    unsigned long time_beg = millis();
    long readTo=0;
    uint8_t pipe=99;
    *pldLength=NRF_MAX_PAYLOAD_LENGTH;
    int numP=AV_EMPTY;    
    while(numP==AV_EMPTY && (readTo>=0)){   // waiting for concentrator answer
        readTo=TO_REGISTER-millis()+time_beg;
        numP=read(message,&pipe,pldLength,nbPerif);}

    PP4_HIGH
    CE_LOW
    if(numP>=0 && (readTo>=0)){             // no TO && pld ok
        numP=message[NRF_ADDR_LENGTH]-'0';  // numP
        PP4
        return numP;}                       // PRX mode still true

    if(numP>=0){
//        CE_LOW
        return ER_RDYTO;}                   // else TO error
    
    CE_LOW
    return numP;                            // or AV error 
}

int Nrfp::txRx(byte* message,uint8_t* pldLength)
{                      // ER_MAXRT ; AV_errors codes ; >=0 numP ok

    //memset(message,0x00,NRF_MAX_PAYLOAD_LENGTH+1);
    message[NRF_MAX_PAYLOAD_LENGTH]=0x00;
    memcpy(message,locAddr,NRF_ADDR_LENGTH);
    
    write(message,NO_ACK,NRF_MAX_PAYLOAD_LENGTH,0);     // send macAddr + numP=0 to ccAddr ; no ACK


    //Serial.print("\n___1___");

#ifndef DETS
    int trst=1;
    while(trst==1){trst=transmitting(NO_ACK);}
    if(trst<0){return ER_MAXRT;}              // MAX_RT error should not happen (no ACK mode)
                                              // radio card HS or missing
#endif // ndef DETS
      
#ifdef DETS
// version accélérée pour minimiser le délai entre TX_DS et setRx()
// (jusqu'à 40uS en compil release ; moins de 20uS accéléré)
    GET_STA
    conf=(CONFREG) | (PRIM_RX_BIT);           // ready pour setRx()
    while((statu & (TX_DS_BIT | MAX_RT_BIT))==0){GET_STA}

    

    //Serial.print("\n___2___");

    if(statu & MAX_RT_BIT){
      Serial.print("\nsyst err maxrt without ack ");Serial.println(statu,HEX);delay(2);
      return ER_MAXRT;} 

    CE_LOW              // to change from TX to RX
    regWrite(CONFIG,&conf);                   // setRx()
    CE_HIGH
    prxMode=true;
// fin version accélérée
#endif // def DETS

    unsigned long time_beg = millis();
    long readTo=0;
    uint8_t pipe=99;
    *pldLength=NRF_MAX_PAYLOAD_LENGTH;
    int numP=AV_EMPTY;    
    while(numP==AV_EMPTY && (readTo>=0)){     // waiting for concentrator answer
        readTo=TO_REGISTER-millis()+time_beg;
        numP=read(message,&pipe,pldLength,nbPerif);}

    //Serial.print("\n___3___");

    PP4_HIGH
    CE_LOW
    if(numP>=0 && (readTo>=0)){               // no TO && pld ok
        numP=message[NRF_ADDR_LENGTH]-'0';        // numP
        PP4
        return numP;}                         // PRX mode still true

    if(numP>=0){
        return ER_RDYTO;}                     // else TO error
    
    return numP;                              // or AV error 
}
#endif // MACHINE == 'P'

void Nrfp::printAddr(char* addr,char n)
{
  for(int j=0;j<NRF_ADDR_LENGTH;j++){Serial.print((char)addr[j]);}
  if(n=='n'){Serial.println();}
}

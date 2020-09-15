
#include "dhtxx.h"

#define PIN 2

Dhtxx dhtxx;

void setup() {

Serial.begin(38400);

uint8_t chk=(uint8_t)dhtxx.getDht(PIN);if(chk<0){Serial.print("err ");}
Serial.println(chk);

}

void loop() {

  delay(3000);
  Serial.print((int)dhtxx.humidh);Serial.print(" ");Serial.print((int)dhtxx.humidl);Serial.print(" | ");
  Serial.print((int)dhtxx.temph);Serial.print(" ");Serial.println((int)dhtxx.templ);
}

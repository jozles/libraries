
#include "avr_sleep.h"

#include <avr/sleep.h>
#include <avr/power.h>

#include <Arduino.h>

uint16_t wdtTime[]={16,32,64,125,250,500,1000,2000,4000,8000};   // durées WDT millis


void hardwarePowerDown()
{
  pinMode(LED,INPUT);
}

void wdtSetup(uint8_t durat)  // (0-9) durat>9 for external wdt on INT0 (à traiter)
{
// datasheet page 54, Watchdog Timer.

/*  MCUSR MCU status register (reset sources)(every bit cleared by writing 0 in it)
 *   WDRF reset effectué par WDT
 *   BORF ------------------ brown out detector
 *   EXTRF ----------------- pin reset
 *   PORF ------------------ power ON
*/

  noInterrupts();

  MCUSR &= ~(1<<WDRF);  // pour autoriser WDE=0

/*  WDTCSR watchdog timer control
 *   WDIF watchdog interrupt flag (set when int occurs with wdt configured for) (reset byu writing 1 or executing ISR(WDT_vect))
 *   WDIE watchdog interrupt enable
 *   WDE  watchdog reset enable
 *        WDE  WDIE   Mode
 *         0    0     stop
 *         0    1     interrupt
 *         1    0     reset
 *         1    1     interrupt then reset (WDIE->0 lors de l'interruption, retour au mode reset)
 *       !!!! le fuse WDTON force le mode reset si 0 !!!!
 *   WDCE watchdog change enable (enable writing 0 to WDE and modif prescaler) (auto cleared after 4 cycles)
 *   WDP[3:0] prescaler 2^(0-9)*2048 divisions de l'oscillateur WDT (f=128KHz p*2048=16mS)
 *
 *   l'instruction wdr reset le timer (wdt_reset();)
 */


    WDTCSR = (1<<WDCE) | (1<<WDE);    // WDCE ET WDE doivent être 1
                                      // pour écrire WDP[0-3] et WDE dans les 4 cycles suivants
    WDTCSR = (1<<WDIE) | durat;       // WDCE doit être 0 ; WDE=0 ; WDIE=1 mode interruption, 8s

  interrupts();

}

uint16_t sleepPwrDown(uint8_t durat)
{

/* to reduce ATMEGA power in sleep mode
 *
 * disable ADC
 *    in ADCSRA register clr ADEN bit ->  ADCSRA = 0;
 *    in DIDR0 register set ADC5D...ADC0D bits
 *    ADC must be disabled before PRR !
 * disable AC (analog comparator)
 *    in ACSR register set ACD bit
 *    in DIDR1 register set AIN1D, AIN0D bits
 * disable BOD -> sleep_bod_disable() before sleep_cpu()
 *    in MCUCR register set BODS and BODSE bits then set BODS and clr BODSE ; 3 clk cycles to sleep or BODS cleared
 * Voltage Reference is connected when either
 *    BOD is enable or bandgap ref connected to AC or ADC enable
 * disable modules clock in PRR -> power_all_disable()
 *    for ACTIVE and IDLE mode only (automatic in other modes)
 * put every I/O pin in input mode
 *
 * Power Reduction Register (PRR)
 *
 * Bit 7 - PRTWI    : Two Wire
 * Bit 6 - PRTIM2   : Timer/Counter2
 * Bit 5 - PRTIM0   : Timer/Counter0
 * Bit 4 - Res:
 * Bit 3 - PRTIM1   : Timer/Counter1
 * Bit 2 - PRSPI    : SPI
 * Bit 1 - PRUSART0 : USART0
 * Bit 0 - PRADC    : ADC
 *
 * Enabling :
 *
 * power_adc_enable();    // ADC converter
 * power_spi_enable();    // SPI
 * power_usart0_enable(); // Serial (USART)
 * power_timer0_enable(); // Timer 0
 * power_timer1_enable(); // Timer 1
 * power_timer2_enable(); // Timer 2
 * power_twi_enable();    // TWI (I2C)
 *
 * Disabling:
 *
 * power_all_disable ();   // turn off all modules
 *
 * power_adc_disable();   // ADC converter
 * power_spi_disable();   // SPI
 * power_usart0_disable();// Serial (USART)
 * power_timer0_disable();// Timer 0
 * power_timer1_disable();// Timer 1
 * power_timer2_disable();// Timer 2
 * power_twi_disable();   // TWI (I2C)
*/
    //nbS++;

    hardwarePowerDown();                  // user hardware

    ADCSRA = 0;                           // ADC shutdown
    power_all_disable();                  // set all bits in PRR register (modules clk halted)

    wdtSetup(durat);
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);

    noInterrupts();                       // cli();
    sleep_enable();
    sleep_bod_disable();                  // BOD halted if sleep_cpu follow
    interrupts();                         // sei();
    sleep_cpu();
    sleep_disable();

    power_all_enable();}

    return wdtTime[durat]/10;
}

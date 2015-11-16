#include "gpio.h"

/* SPI uses pins A2 to A5 */
/*
int triggerOutputPinA = D0;
int triggerOutputPinB = D1;
int triggerInputPinA  = D2;
int triggerInputPinB  = D3;
int cvOutputPinA      = A6;
int cvOutputPinB      = A7;
int cvInputPinA       = A0;
int cvInputPinB       = A1;
int ledPin            = D7;
int DAC_CS_PIN        = A2;
int DAC_SCK_PIN       = A3;
int DAC_SDI_PIN       = A5;
*/


#define OPENSOUND_TRIG_OUT_A_PORT GPIOB
#define OPENSOUND_TRIG_OUT_A_PIN  GPIO_Pin_7 // D0
#define OPENSOUND_TRIG_OUT_B_PORT GPIOB
#define OPENSOUND_TRIG_OUT_B_PIN  GPIO_Pin_6 // D1
#define OPENSOUND_TRIG_IN_A_PORT GPIOB
#define OPENSOUND_TRIG_IN_A_PIN  GPIO_Pin_5 // D2
#define OPENSOUND_TRIG_IN_B_PORT GPIOB
#define OPENSOUND_TRIG_IN_B_PIN  GPIO_Pin_4 // D3
#define OPENSOUND_DAC_CS_PORT GPIOC
#define OPENSOUND_DAC_CS_PIN  GPIO_Pin_6 // A2
#define OPENSOUND_DAC_SCK_PORT GPIOA
#define OPENSOUND_DAC_SCK_PIN  GPIO_Pin_5 // A3
#define OPENSOUND_DAC_SDI_PORT GPIOA
#define OPENSOUND_DAC_SDI_PIN  GPIO_Pin_7 // A5

/* D1 / GPIO_3 / PB6 */
/* D0 / GPIO_4 / PB7 */
/* A0 / GPIO_2 / ?? */
/* A1 / GPIO_7 / PC3 */
/* A2 / GPIO_6 / PC2 */
/* A3 / SPI_SCK / PA5 */
/* A4 / SPI_MISO / PA6 */
/* A5 / SPI_MOSI / PA7 */
/* D2 / GPIO_5 / PB5 */
/* D3 / JTAG_TRST / PB4 */
/* D4 / JTAG_TDO / PB3 */

#define DAC_A_B_BIT   (1<<7) // 0=DAC A, 1=DAC b
#define DAC_BUF_BIT   (1<<6)
#define DAC_GA_BIT    (1<<5) // 0=2x, 1=1x gain
#define DAC_SHDN_BIT  (1<<4) // 0=shutdown, 1=normal
uint8_t TRANSFER_BITS = (DAC_SHDN_BIT | DAC_GA_BIT | DAC_BUF_BIT);

static void spi_transfer(uint8_t working) {
  // function to actually bit shift the data byte out
  for(int i = 1; i <= 8; i++) {
    if(working > 127)
      setPin(OPENSOUND_DAC_SDI_PORT, OPENSOUND_DAC_SDI_PIN); // if it is a 1 (ie. B1XXXXXXX), set the master out pin high
    else
      clearPin(OPENSOUND_DAC_SDI_PORT, OPENSOUND_DAC_SDI_PIN); // if it is not 1 (ie. B0XXXXXXX), set the master out pin low
    setPin(OPENSOUND_DAC_SCK_PORT, OPENSOUND_DAC_SCK_PIN); // set clock high, the pot IC will read the bit into its register

    /* if(working > 127) */
    /*   digitalWrite (DAC_SDI_PIN, HIGH); // if it is a 1 (ie. B1XXXXXXX), set the master out pin high */
    /* else */
    /*   digitalWrite (DAC_SDI_PIN, LOW); // if it is not 1 (ie. B0XXXXXXX), set the master out pin low */
    /* digitalWrite (DAC_SCK_PIN, HIGH); // set clock high, the pot IC will read the bit into its register */
    working <<= 1;
    // set clock low, the pot IC will stop reading and prepare for the next iteration (next significant bit
    clearPin(OPENSOUND_DAC_SCK_PORT, OPENSOUND_DAC_SCK_PIN);
  }
}

static void spi_out(uint8_t high, uint8_t low) {
  /* digitalWrite(DAC_CS_PIN, LOW); */
  clearPin(OPENSOUND_DAC_CS_PORT, OPENSOUND_DAC_CS_PIN);
  spi_transfer(high);
  spi_transfer(low);
  setPin(OPENSOUND_DAC_CS_PORT, OPENSOUND_DAC_CS_PIN);
  /* digitalWrite(DAC_CS_PIN, HIGH); */
}

static void spi_init() {
  configureDigitalOutput(OPENSOUND_DAC_CS_PORT, OPENSOUND_DAC_CS_PIN);
  configureDigitalOutput(OPENSOUND_DAC_SCK_PORT, OPENSOUND_DAC_SCK_PIN);
  configureDigitalOutput(OPENSOUND_DAC_SDI_PORT, OPENSOUND_DAC_SDI_PIN);
  // hold slave select 1 pin high, so that chip is not selected to begin with
  setPin(OPENSOUND_DAC_CS_PORT, OPENSOUND_DAC_CS_PIN);

  /* pinMode(DAC_CS_PIN, OUTPUT); // set CS pin to output */
  /* pinMode(DAC_SCK_PIN, OUTPUT); // set SCK pin to output */
  /* pinMode(DAC_SDI_PIN, OUTPUT); // set MOSI pin to output */
  /* digitalWrite(DAC_CS_PIN, HIGH); // hold slave select 1 pin high, so that chip is not selected to begin with */
}

void dac_setup(){
  // pinMode(DAC_CS_PIN, OUTPUT); // not done by SPI.begin()?
  // digitalWrite(DAC_CS_PIN, HIGH);
  // SPI.begin();
  // SPI.setBitOrder(MSBFIRST);
  // // SPI.setClockDivider(SPI_CLOCK_DIV64);
  // SPI.setClockDivider(SPI_CLOCK_DIV256);
  // // SPI.setDataMode(SPI_MODE1); /* for mode 0,0 */
  // SPI.setDataMode(SPI_MODE1);
  spi_init();
}

void dac_set_a(int value){
  spi_out(TRANSFER_BITS | (value >> 8 & 0xf), value & 0xff);
}

void dac_set_b(int value){
  spi_out(TRANSFER_BITS | DAC_A_B_BIT | (value >> 8 & 0xf), value & 0xff);
}

/* void dac_set(int ch, int value){ */
/*   if(ch == cvOutputPinA){ */
/*     spi_out(TRANSFER_BITS | (value >> 8 & 0xf), value & 0xff); */
/*   }else{ */
/*     spi_out(TRANSFER_BITS | DAC_A_B_BIT | (value >> 8 & 0xf), value & 0xff); */
/*   } */
  // digitalWrite(DAC_CS_PIN, LOW);
  // if(ch == cvOutputPinA){
  //   SPI.transfer(TRANSFER_BITS | ((value && 0x0f00) >> 8));
  // }else{
  //   SPI.transfer(TRANSFER_BITS | DAC_A_B_BIT | ((value & 0x0f00) >> 8));
  // }
  // SPI.transfer(value & 0x00ff);
  // digitalWrite(DAC_CS_PIN, HIGH);
  /* Serial_print("DAC transfer "); */
  /* Serial_print(ch); */
  /* Serial_print(": "); */
  /* Serial_println(value); */
/* } */


void dac_toggle_gain(){
  // toggles 2x gain setting
  if(TRANSFER_BITS & DAC_GA_BIT)
    TRANSFER_BITS &= ~DAC_GA_BIT;
  else
    TRANSFER_BITS |= DAC_GA_BIT;
  /* Serial_println("toggle gain " + String(TRANSFER_BITS & DAC_GA_BIT)); */
}

void dac_set_gain(bool gain){
  if(gain)
    TRANSFER_BITS &= ~DAC_GA_BIT;
  else
    TRANSFER_BITS |= DAC_GA_BIT;
  /* Serial_println("set gain " + String((TRANSFER_BITS & DAC_GA_BIT) ? "1x" : "2x")); */
}

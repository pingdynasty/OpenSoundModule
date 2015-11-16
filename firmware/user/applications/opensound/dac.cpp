#include "dac.h"
#include "application.h"

// #define PHOTON_DAC

void dac_init() {
#ifdef PHOTON_DAC
  pinMode(DAC1, OUTPUT);
  pinMode(DAC2, OUTPUT);
#else
  // DAC pins are PA4 and PA5
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_5;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  DAC_InitTypeDef DAC_InitStructure;
  DAC_StructInit(&DAC_InitStructure);
  /* DAC Periph clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
  DAC_DeInit();
  /* DAC channel1 & channel2 Configuration */
  DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;
  DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
  DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
  DAC_Init(DAC_Channel_1, &DAC_InitStructure);
  DAC_Init(DAC_Channel_2, &DAC_InitStructure);

  /* Enable DAC Channel1 */
  DAC_Cmd(DAC_Channel_1, ENABLE);
  /* Enable DAC Channel2 */
  DAC_Cmd(DAC_Channel_2, ENABLE);

  /* Enable DAC conversion by software */
  DAC_DualSoftwareTriggerCmd(ENABLE);
  DAC_SetDualChannelData(DAC_Align_12b_R, 0x800, 0x800);
#endif
}

void dac_set_a(uint16_t value){
  value = 4095 - max(0, min(4095, value));
  //  pinMode(DAC1, OUTPUT);
#ifdef PHOTON_DAC
  analogWrite(DAC1, value); 
#else
  /* Set the DAC Channel1 data */
  DAC_SetChannel2Data(DAC_Align_12b_R, value);
#endif
}

void dac_set_b(uint16_t value){
  value = 4095 - max(0, min(4095, value));
  //  pinMode(DAC2, OUTPUT);
#ifdef PHOTON_DAC
  analogWrite(DAC2, value); 
#else
  /* Set the DAC Channel2 data */
  DAC_SetChannel1Data(DAC_Align_12b_R, value);
#endif
}

void dac_set_ab(uint16_t a, uint16_t b){
  a = 4095 - max(0, min(4095, a));
  b = 4095 - max(0, min(4095, b));
#ifdef PHOTON_DAC
  analogWrite(DAC1, a); 
  analogWrite(DAC2, b); 
#else
  // reversed channels??
  DAC_SetDualChannelData(DAC_Align_12b_R, b, a);
#endif
}

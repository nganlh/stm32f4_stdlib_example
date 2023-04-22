#include "stm32f4xx.h"
#include "stdio.h"

void v_RCC_Init();
void v_GPIO_Init();
void v_USART1_Init();
void v_ADC1_Init();
void v_USART_Send_Text(volatile char *s);

char ac_str_buf[100];
uint16_t u16_adc_val = 0;
float flt_adc_volt = 0;

int main(void)
{
  // // Enable the FPU
  // SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));
  
  // // Configure the compiler to use the FPU
  // #ifdef __GNUC__
  // __asm volatile ("VCMP.F32  S0, S1");
  // #endif


  v_RCC_Init();
  v_GPIO_Init();
  v_USART1_Init();
  v_ADC1_Init();

  v_USART_Send_Text("ADC Example\r\n");
  GPIO_SetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);

  while (1)
  {
    /* Wait until end of conversion */
    while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
    u16_adc_val = ADC_GetConversionValue(ADC1);
    flt_adc_volt = (float)u16_adc_val * 3.3f / (1 << 12);

    sprintf(ac_str_buf, "ADC value = %d, Voltage = %fV\r\n", u16_adc_val, flt_adc_volt);
    v_USART_Send_Text(ac_str_buf);

    // u16_adc_val++;
    // sprintf(ac_str_buf, "hello %d\r\n", u16_adc_val);
    // v_USART_Send_Text(ac_str_buf);
    for (int i = 0; i < SystemCoreClock/30; i++) { __NOP(); }
  }
}

void v_RCC_Init()
{
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA |
                         RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOD, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_ADC1, ENABLE);
}

void v_GPIO_Init()
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* Init LEDs on GPIOD */
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOD, &GPIO_InitStruct);

  /* Init GPIO pins for USART */
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_Init(GPIOB, &GPIO_InitStruct);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);

  /* Init GPIO pin for ADC */
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void v_USART1_Init()
{
  USART_InitTypeDef USART_InitStruct;

  USART_InitStruct.USART_BaudRate = 115200;
  USART_InitStruct.USART_WordLength = USART_WordLength_8b;
  USART_InitStruct.USART_StopBits = USART_StopBits_1;
  USART_InitStruct.USART_Parity = USART_Parity_No;
  USART_InitStruct.USART_Mode = USART_Mode_Tx;
  USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_Init(USART1, &USART_InitStruct);
  USART_Cmd(USART1, ENABLE);
}

void v_ADC1_Init()
{
  ADC_CommonInitTypeDef ADC_ComInitStruct;
  ADC_InitTypeDef ADC_InitStruct;

  ADC_ComInitStruct.ADC_Mode = ADC_Mode_Independent;
  ADC_ComInitStruct.ADC_Prescaler = ADC_Prescaler_Div2;
  ADC_ComInitStruct.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  ADC_ComInitStruct.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
  ADC_CommonInit(&ADC_ComInitStruct);

  ADC_DeInit();
  ADC_InitStruct.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStruct.ADC_ScanConvMode = DISABLE;
  ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStruct.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
  ADC_InitStruct.ADC_NbrOfConversion = 1;
  ADC_Init(ADC1, &ADC_InitStruct);

  ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_144Cycles);
  ADC_Cmd(ADC1, ENABLE);
  ADC_SoftwareStartConv(ADC1);
}

void v_USART_Send_Text(volatile char *s)
{
  while (*s)
  {
    /* Wait until data register is empty */
    while (!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
    USART_SendData(USART1, *s++);
  }
}

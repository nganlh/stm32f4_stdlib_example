#include "stm32f4xx.h"
#include "stdio.h"

#define TEMP_SENSOR_VREF        ( 0.76f   )
#define TEMP_SENSOR_VSLOPE      ( 0.0025f )    /* Volt / C degree */
#define TEMP_REF                ( 25.0f   )

void v_RCC_Init();
void v_GPIO_Init();
void v_USART1_Init();
void v_ADC1_Init();
void v_DMA2_Init();
void v_USART_Send_Text(volatile char *s);

char ac_str_buf[100];
uint16_t au16_adc_buf[2];
float flt_temp = 0;

int main(void)
{
  v_RCC_Init();
  v_GPIO_Init();
  v_USART1_Init();
  v_ADC1_Init();
  v_DMA2_Init();

  v_USART_Send_Text("ADC Example\r\n");
  GPIO_SetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);

  while (1)
  {
    flt_temp = (float)au16_adc_buf[1] * 3.3 / (1 << 12);
    flt_temp -= TEMP_SENSOR_VREF;
    flt_temp /= TEMP_SENSOR_VSLOPE;
    flt_temp += TEMP_REF;

    sprintf(ac_str_buf, "ADC0 value = %d, Temp ADC value: %d, Temp: %f *C\r\n",
            au16_adc_buf[0], au16_adc_buf[1], flt_temp);

    v_USART_Send_Text(ac_str_buf);
    for (int i = 0; i < SystemCoreClock/30; i++) { __NOP(); }
  }
}

void v_RCC_Init()
{
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA |
                         RCC_AHB1Periph_GPIOB |
                         RCC_AHB1Periph_GPIOD |
                         RCC_AHB1Periph_DMA2, ENABLE);

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

void v_USART_Send_Text(volatile char *s)
{
  while (*s)
  {
    /* Wait until data register is empty */
    while (!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
    USART_SendData(USART1, *s++);
  }
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
  ADC_InitStruct.ADC_ScanConvMode = ENABLE;
  ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStruct.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
  ADC_InitStruct.ADC_NbrOfConversion = 2;
  ADC_Init(ADC1, &ADC_InitStruct);

  ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_144Cycles);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_TempSensor, 2, ADC_SampleTime_144Cycles);

  ADC_TempSensorVrefintCmd(ENABLE);

  ADC_DMACmd(ADC1, ENABLE);
  ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);

  ADC_Cmd(ADC1, ENABLE);
  ADC_SoftwareStartConv(ADC1);
}

void v_DMA2_Init()
{
  DMA_InitTypeDef DMA_InitStruct;
  
  DMA_InitStruct.DMA_Channel = 0;
  DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
  DMA_InitStruct.DMA_Memory0BaseAddr = (uint32_t)au16_adc_buf;
  DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralToMemory;
  DMA_InitStruct.DMA_BufferSize = 2;
  DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStruct.DMA_Priority = DMA_Priority_Medium;
  DMA_InitStruct.DMA_FIFOMode = DMA_FIFOMode_Disable;
  DMA_InitStruct.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
  DMA_InitStruct.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStruct.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

  DMA_Init(DMA2_Stream0, &DMA_InitStruct);
  DMA_Cmd(DMA2_Stream0, ENABLE);
}

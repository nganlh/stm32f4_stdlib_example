#include "stm32f4xx.h"

int main(void)
{
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOD, ENABLE);

  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOD, &GPIO_InitStruct);

  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_Init(GPIOA, &GPIO_InitStruct);

  while(1)
  {
    if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0))
    {
      GPIO_SetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_15);
    }
    else
    {
      GPIO_ResetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_15);
    }

    GPIO_SetBits(GPIOD, GPIO_Pin_14);
    for (long i = 0; i < SystemCoreClock/30; i++) { __NOP(); }
    GPIO_ResetBits(GPIOD, GPIO_Pin_14);
    for (long i = 0; i < SystemCoreClock/30; i++) { __NOP(); }
  }

  return 0;
}
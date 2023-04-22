#include "stm32f4xx.h"

void v_GPIO_Init();
void v_USART1_Init();
void v_USART_SendText(volatile char *s);

int main(void)
{
  v_GPIO_Init();
  v_USART1_Init();

  while (1)
  {
    // GPIO_ToggleBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
    // v_USART_SendText("hello\r\n");
    // GPIO_ToggleBits(GPIOD, GPIO_Pin_12);
    // for (long i = 0; i < SystemCoreClock/13; i++) { __NOP(); }
  }

  return 0;
}

void v_GPIO_Init()
{
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOD, ENABLE);

  /* USART Pins */
  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStruct);

  GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);

  /* LED Pins */
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOD, &GPIO_InitStruct);
}

void v_USART1_Init()
{
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

  USART_InitTypeDef USART_InitStruct;
  USART_InitStruct.USART_BaudRate = 115200;
  USART_InitStruct.USART_WordLength = USART_WordLength_8b;
  USART_InitStruct.USART_StopBits = USART_StopBits_1;
  USART_InitStruct.USART_Parity = USART_Parity_No;
  USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
  USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_Init(USART1, &USART_InitStruct);
  USART_Cmd(USART1, ENABLE);

  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
  NVIC_EnableIRQ(USART1_IRQn);
}

void v_USART_SendText(volatile char *s)
{
  while(*s)
  {
    while( !USART_GetFlagStatus(USART1, USART_FLAG_TXE) );
    USART_SendData(USART1, *s++);
  }
}

void USART1_IRQHandler(void)
{
  char c;

  if (USART_GetITStatus(USART1, USART_IT_RXNE))
  {
    c = USART_ReceiveData(USART1);

    switch (c)
    {
    case 'a':
      GPIO_ToggleBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
      v_USART_SendText("Toggle all LEDs\r\n");
      break;
    case '1':
      GPIO_SetBits(GPIOD, GPIO_Pin_12);
      v_USART_SendText("LED1 ON\r\n");
      break;
    case '2':
      GPIO_SetBits(GPIOD, GPIO_Pin_13);
      v_USART_SendText("LED2 ON\r\n");
      break;
    case '3':
      GPIO_SetBits(GPIOD, GPIO_Pin_14);
      v_USART_SendText("LED3 ON\r\n");
      break;
    case '4':
      GPIO_SetBits(GPIOD, GPIO_Pin_15);
      v_USART_SendText("LED4 ON\r\n");
      break;
    case '!':
      GPIO_ResetBits(GPIOD, GPIO_Pin_12);
      v_USART_SendText("LED1 OFF\r\n");
      break;
    case '@':
      GPIO_ResetBits(GPIOD, GPIO_Pin_13);
      v_USART_SendText("LED2 OFF\r\n");
      break;
    case '#':
      GPIO_ResetBits(GPIOD, GPIO_Pin_14);
      v_USART_SendText("LED3 OFF\r\n");
      break;
    case '$':
      GPIO_ResetBits(GPIOD, GPIO_Pin_15);
      v_USART_SendText("LED4 OFF\r\n");
      break;
    break;
    }
  }
}

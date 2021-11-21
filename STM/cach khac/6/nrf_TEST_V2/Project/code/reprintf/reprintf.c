/**
  ******************************************************************************
  * @file       reprintf.c
  * @author     LinhKien69
  * @version    V1.0.0
  * @date       28/02/2015
  ******************************************************************************
**/


#include <stdio.h>
#include "reprintf.h"
#include "stm32f10x_usart.h"
extern int8_t target;
PUTCHAR_PROTOTYPE
{
   USART_SendData(USART1, (uint8_t) ch);
   while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
   {}
   return ch;
}







/******************* (C) 2015/03 LinhKien69 ARM Cortex-M3 **********END OF FILE*********/
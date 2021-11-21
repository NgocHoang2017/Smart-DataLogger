/**
  ******************************************************************************
  * @file       main.c
  * @author     LinhKien69
  * @version    V1.0.0
  * @date       28/02/2015
  ******************************************************************************
**/

#ifndef __MAIN_H
#define __MAIN_H
#ifdef __cplusplus
 extern "C" {
#endif 
#include "stm32f10x.h"
//#define CE    GPIO_Pin_1  //
//#define SCK   GPIO_Pin_10 //
////#define IRQ      PORTB.0
//#define MISO   GPIO_Pin_5       //Chan In C
//#define CSN    GPIO_Pin_2 //
//#define MOSI   GPIO_Pin_11//


void RCC_Configuration(void);
//void Delay_ms(__IO uint32_t nTime);
void Delay_ms(uint32_t a);

#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

#ifdef __cplusplus
}
#endif


#endif /* __MAIN_H */



/******************* (C) 2015/03 LinhKien69 ARM Cortex-M3 **********END OF FILE*********/

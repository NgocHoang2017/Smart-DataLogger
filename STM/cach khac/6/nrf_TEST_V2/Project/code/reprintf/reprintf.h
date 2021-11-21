/**
  ******************************************************************************
  * @file       reprintf.h
  * @author     LinhKien69
  * @version    V1.0.0
  * @date       28/02/2015
  ******************************************************************************
**/

#ifndef __REPRINTF_H
#define __REPRINTF_H
#ifdef __cplusplus
 extern "C" {
#endif 
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
#ifdef __cplusplus
}
#endif
#endif







/******************* (C) 2015/03 LinhKien69 ARM Cortex-M3 **********END OF FILE*********/
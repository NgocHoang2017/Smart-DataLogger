/**
  ******************************************************************************
  * @file       main.c
  * @author     LinhKien69
  * @version    V1.0.0
  * @date       28/02/2015
  ******************************************************************************
**/


#include "main.h"
#include "reprintf.h"
#include "stm32_usart.h"
#include "nrf24l01.h"
int a=1,hs=1,i;
unsigned char RxBuf[32];
uint8_t str[16];
//tao delay
void Delay_ms(uint32_t a)
{
	uint32_t n;
	n=100*a;
	while(n--);
}
int main(void)
{ 
	RCC_nrf24l01();
	GPIO_nrf24l01();
	init_NRF24L01();  // Khoi tao NRF
	SetRX_Mode();     // Cau hinh la MODE NHAN
	usart1_init(GPIOA,GPIO_Pin_9,GPIO_Pin_10,115200);
	GPIO_WriteBit(GPIOC, GPIO_Pin_13, (BitAction)(0));
	GPIO_WriteBit(GPIOA, GPIO_Pin_3, (BitAction)(0));
	GPIO_WriteBit(GPIOC, GPIO_Pin_2, (BitAction)(0));
	while(1)
	{	
			if(nRF24L01_RxPacket(RxBuf)==1)   // Neu nhan duoc du lieu
			{     
				if(RxBuf[0] == 'A') // Neu phan tu dau trong mang nhan duoc la 'A'
				{
					GPIO_WriteBit(GPIOA, GPIO_Pin_3, 1-GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_3)); //Dao trang thai chan C3
				}
				for(i=0;i<32;i++)
				printf("%d",RxBuf[i]);//In chuoi mang nhan duoc len PC
				printf("\n");
				SetRX_Mode();         
			}
			a = GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8);
			if(a==0)
			if(hs == 1)
			{
				SetTX_Mode();               // Cau hinh la MODE PHAT
				nRF24L01_TxPacket(TxBuf); // Transmit Tx buffer data 
				SPI_RW_Reg(WRITE_RE+STATUS,0XFF); // Clear Bit Send 
				SetRX_Mode();  // Tro ve trang thai là MODE NHAN
			}
			hs = a;
		}
}		
 
/******************* (C) 2015/03 LinhKien69 ARM Cortex-M3 **********END OF FILE*********/
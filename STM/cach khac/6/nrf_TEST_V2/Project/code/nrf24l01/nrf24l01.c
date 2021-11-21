/**
  ******************************************************************************
  * @file       nrf24l01.c
  * @author     LinhKien69
  * @version    V1.0.0
  * @date       28/02/2015
  ******************************************************************************
**/

#include "nrf24l01.h"
#include <stdio.h>
#include "reprintf.h"
#include "stm32f10x_usart.h"
#include "main.h"
unsigned char TxBuf[32]=
{
  'A',2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,29,30,31,32
};

unsigned char  TX_ADDRESS[TX_ADR_WIDTH]= {0x68,0x31,0x08,0x10,0x01};   //dia chi truyen nhan TX giong RX
unsigned char  RX_ADDRESS[RX_ADR_WIDTH]= {0x68,0x31,0x08,0x10,0x01};   //
unsigned char SPI_Write_Buf(unsigned char reg,unsigned char *pBuf,unsigned char uchars);
void RCC_nrf24l01()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC |RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOA, ENABLE);//CAP CLOCK CHO PORT A,B,C
}
void GPIO_nrf24l01()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_13|GPIO_Pin_3;// chon chan so 3,13
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;// chon tan so(de mac dinh)
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //chon chuc nang la chan dau ra
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//CE
	GPIO_InitStructure.GPIO_Pin =   CE_NRF24L01;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;// chon tan so(de mac dinh)
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP; //chon chuc nang la chan dau ra
	GPIO_Init(Port_CE_NRF24L01, &GPIO_InitStructure);
	//SCK
	GPIO_InitStructure.GPIO_Pin =   SCK_NRF24L01;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;// chon tan so(de mac dinh)
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP; //chon chuc nang la chan dau ra
	GPIO_Init(Port_SCK_NRF24L01, &GPIO_InitStructure);
	//MISO
	GPIO_InitStructure.GPIO_Pin =   MISO_NRF24L01;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;// chon tan so(de mac dinh)
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IN_FLOATING; //chon chuc nang la chan dau vao
	GPIO_Init(Port_MISO_NRF24L01, &GPIO_InitStructure);
	//CSN
	GPIO_InitStructure.GPIO_Pin =   CSN_NRF24L01;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;// chon tan so(de mac dinh)
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP; //chon chuc nang la chan dau ra
	GPIO_Init(Port_CSN_NRF24L01, &GPIO_InitStructure);
	//MOSI
	GPIO_InitStructure.GPIO_Pin =  MOSI_NRF24L01;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;// chon tan so(de mac dinh)
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP; //chon chuc nang la chan dau ra
	GPIO_Init(Port_MOSI_NRF24L01, &GPIO_InitStructure);
	//PHIM BAM
	GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_8|GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;// chon tan so(de mac dinh)
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IPU; //chon chuc nang la chan dau ra luon o muc cao
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	
}
//Lenh gui data den NRF
unsigned char SPI_RW(unsigned char Buff)
{
	 unsigned char bit_ctr;
   for(bit_ctr=0;bit_ctr<8;bit_ctr++) // output 8-bit
      {
      if((Buff & 0x80)==0x80)
			GPIO_WriteBit(Port_MOSI_NRF24L01,MOSI_NRF24L01, (BitAction)(1));     // output 'uchar', MSB to MOSI    
      else  GPIO_WriteBit(Port_MOSI_NRF24L01,MOSI_NRF24L01, (BitAction)(0)); 
			Delay_ms(5);
      Buff = (Buff << 1);           // shift next bit into MSB..
      GPIO_WriteBit(Port_SCK_NRF24L01,SCK_NRF24L01, (BitAction)(1));                    // Set SCK high..          
      Delay_ms(5);
      Buff |= GPIO_ReadInputDataBit(Port_MISO_NRF24L01,MISO_NRF24L01);              // capture current MISO bit
      GPIO_WriteBit(Port_SCK_NRF24L01,SCK_NRF24L01, (BitAction)(0));                // ..then set SCK low again
      }
      return(Buff);                   // return read uchar
}
//Doc gia tri tra ve tu cac thanh ghi NRF
unsigned char SPI_Read(unsigned char reg)
{
   unsigned char reg_val;
	 GPIO_WriteBit(Port_CSN_NRF24L01,CSN_NRF24L01, (BitAction)(0));           // CSN low, initialize SPI communication...
   SPI_RW(reg);            // Select register to read from..
   reg_val = SPI_RW(0);    // ..then read registervalue
   GPIO_WriteBit(Port_CSN_NRF24L01,CSN_NRF24L01, (BitAction)(1));                    // CSN high, terminate SPI communication

   return(reg_val);        // return register value
}
/****************************************************************************************************/
//unsigned char SPI_RW_Reg(unsigned char reg, unsigned char value)
/****************************************************************************************************/
unsigned char SPI_RW_Reg(unsigned char reg,unsigned char value)
{
   unsigned char status1;

   GPIO_WriteBit(Port_CSN_NRF24L01,CSN_NRF24L01, (BitAction)(0));                          // CSN low, init SPI transaction
   status1 = SPI_RW(reg);      // select register
   SPI_RW(value);             // ..and write value to it..
   GPIO_WriteBit(Port_CSN_NRF24L01,CSN_NRF24L01, (BitAction)(1));                       // CSN high again

   return(status1);            // return nRF24L01 status uchar
}
/****************************************************************************************************/
//unsigned char SPI_Read_Buf(unsigned char reg, unsigned char *pBuf, unsigned char uchars)
//
/****************************************************************************************************/
unsigned char SPI_Read_Buf(unsigned char reg,unsigned char *pBuf,unsigned char uchars)
{
   unsigned char status1,uchar_ctr;

   GPIO_WriteBit(Port_CSN_NRF24L01,CSN_NRF24L01, (BitAction)(0));                               // Set CSN low, init SPI tranaction
   status1 = SPI_RW(reg);             // Select register to write to and read status uchar

   for(uchar_ctr=0;uchar_ctr<uchars;uchar_ctr++)
      pBuf[uchar_ctr] = SPI_RW(0);    //

   GPIO_WriteBit(Port_CSN_NRF24L01,CSN_NRF24L01, (BitAction)(1));        

   return(status1);                    // return nRF24L01 status uchar
}
/*********************************************************************************************************/
//uint SPI_Write_Buf(uchar reg, uchar *pBuf, uchar uchars)
/*****************************************************************************************************/
unsigned char SPI_Write_Buf(unsigned char reg,unsigned char *pBuf,unsigned char uchars)
{
   unsigned char status1,uchar_ctr;
   GPIO_WriteBit(Port_CSN_NRF24L01,CSN_NRF24L01, (BitAction)(0));                    //SPI
   status1 = SPI_RW(reg);
   for(uchar_ctr=0; uchar_ctr<uchars; uchar_ctr++) //
      SPI_RW(*pBuf++);
   GPIO_WriteBit(Port_CSN_NRF24L01,CSN_NRF24L01, (BitAction)(1));                  //SPI
   return(status1);    //
}

/****************************************************************************************************/
//void SetRX_Mode(void)
//
/****************************************************************************************************/
void init_NRF24L01(void)
{
    //inerDelay_us(100);
     Delay_ms(100);
   GPIO_WriteBit(Port_CE_NRF24L01,CE_NRF24L01, (BitAction)(0));   // chip enable
   GPIO_WriteBit(Port_CSN_NRF24L01,CSN_NRF24L01, (BitAction)(1));    // Spi disable
   GPIO_WriteBit(Port_SCK_NRF24L01,SCK_NRF24L01, (BitAction)(0));    // Spi clock line init high
   SPI_Write_Buf(WRITE_RE + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);    //
   SPI_Write_Buf(WRITE_RE + RX_ADDR_P0, RX_ADDRESS, RX_ADR_WIDTH); //
   SPI_RW_Reg(WRITE_RE + EN_AA, 0x01);      // EN P0, 2-->P1
   SPI_RW_Reg(WRITE_RE + EN_RXADDR, 0x01);  //Enable data P0
   SPI_RW_Reg(WRITE_RE + RF_CH, 0);        // Chanel 0 RF = 2400 + RF_CH* (1or 2 M)
   SPI_RW_Reg(WRITE_RE + RX_PW_P0, RX_PLOAD_WIDTH); // Do rong data truyen 32 byte
   SPI_RW_Reg(WRITE_RE + RF_SETUP, 0x07);         // 1M, 0dbm
   SPI_RW_Reg(WRITE_RE + CONFIG, 0x0e);          // Enable CRC, 2 byte CRC, Send

}
void SetRX_Mode(void)
{
   GPIO_WriteBit(Port_CE_NRF24L01,CE_NRF24L01, (BitAction)(0));        
   SPI_RW_Reg(WRITE_RE + CONFIG, 0x0f);         // Enable CRC, 2 byte CRC, Recive
   GPIO_WriteBit(Port_CE_NRF24L01,CE_NRF24L01, (BitAction)(1));        
   Delay_ms(130);    //
}
/****************************************************************************************************/
//void SetTX_Mode(void)
//
/****************************************************************************************************/
void SetTX_Mode(void)
{
   GPIO_WriteBit(Port_CE_NRF24L01,CE_NRF24L01, (BitAction)(0));        
   SPI_RW_Reg(WRITE_RE + CONFIG, 0x0e);         // Enable CRC, 2 byte CRC, Send
   GPIO_WriteBit(Port_CE_NRF24L01,CE_NRF24L01, (BitAction)(1));        
   Delay_ms(130);    //
}

/******************************************************************************************************/
//unsigned char nRF24L01_RxPacket(unsigned char* rx_buf)
/******************************************************************************************************/
unsigned char nRF24L01_RxPacket(unsigned char* rx_buf)
{
    unsigned char revale=0;
   unsigned char sta;
  
   //if(RX_DR)            // Data in RX FIFO
   sta=SPI_Read(STATUS);   // Read Status  
    if((sta&0x40)!=0)      // Data in RX FIFO
   {
     GPIO_WriteBit(Port_CE_NRF24L01,CE_NRF24L01, (BitAction)(0));               //SPI
     SPI_Read_Buf(RD_RX_PLOAD,rx_buf,TX_PLOAD_WIDTH);// read receive payload from RX_FIFO buffer
      revale =1;   
   }
   SPI_RW_Reg(WRITE_RE+STATUS,sta);   
   return revale;
}
/***********************************************************************************************************/
//void nRF24L01_TxPacket(unsigned char * tx_buf)
//
/**********************************************************************************************************/
void nRF24L01_TxPacket(unsigned char * tx_buf)
{
   GPIO_WriteBit(Port_CE_NRF24L01,CE_NRF24L01, (BitAction)(0));            
   SPI_Write_Buf(WRITE_RE + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH); // Send Address
   SPI_Write_Buf(WR_TX_PLOAD, tx_buf, TX_PLOAD_WIDTH);           //send data
   SPI_RW_Reg(WRITE_RE + CONFIG, 0x0e);          // Send Out
   GPIO_WriteBit(Port_CE_NRF24L01,CE_NRF24L01, (BitAction)(1));             
}


/******************* (C) 2015/03 LinhKien69 ARM Cortex-M3 **********END OF FILE*********/

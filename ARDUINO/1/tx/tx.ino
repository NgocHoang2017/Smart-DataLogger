#include <SPI.h>                                            // Подключаем библиотеку для работы с шиной SPI
#include <nRF24L01.h>                                      // Подключаем файл настроек из библиотеки RF24
#include "RF24.h"   
RF24 radio(4, 15);  // khai bao chan(CE, CSN)  
int led_test = 5;
int led_rx = 16;  
int led_tx = 16;
unsigned char  CTR_TX_ADDRESS[5]= {0x68,0x31,0x08,0x10,0x01};   //5 bytes
char data[32] = "du lieu nhiet do va do am";   


void setup()
{
  pinMode(led_test,OUTPUT);
  pinMode(led_tx,OUTPUT);
  pinMode(led_rx,OUTPUT);
  
  Serial.begin(115200);
  Serial.println("MATTER");

  
  SPI.setHwCs(true);
  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST); 
  radio.begin();                                        // khai bao  bat dau nRF24L01+
  radio.setChannel(4);                                  // chon kenh 1
  radio.setDataRate(RF24_1MBPS);                   // Указываем скорость передачи данных (RF24_250KBPS, RF24_1MBPS, RF24_2MBPS), RF24_1MBPS - 1Мбит/сек
  radio.setPALevel(RF24_PA_MAX);
}
void loop()
{
  Serial.println("che do truyen");
  radio.openWritingPipe (CTR_TX_ADDRESS);
  radio.write(&data, sizeof(data));
  
}


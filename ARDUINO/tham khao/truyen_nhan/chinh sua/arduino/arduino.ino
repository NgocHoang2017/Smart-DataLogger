#include <SPI.h>                                            // Подключаем библиотеку для работы с шиной SPI
#include <nRF24L01.h>                                      // Подключаем файл настроек из библиотеки RF24
#include "RF24.h"   
RF24 radio(4, 15);  // khai bao chan(CE, CSN)  
int led_test = 5;
int led_received = 16;  
unsigned char  CTR_TX_ADDRESS[5]= {0x68,0x31,0x08,0x10,0x01};   //5 bytes
void setup()
{
  pinMode(led_test,OUTPUT);
  pinMode(led_received,OUTPUT);
  Serial.begin(115200);
  Serial.println("MATTER");

  
  SPI.setHwCs(true);
  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST); 
  radio.begin();                                        // khai bao  bat dau nRF24L01+
  radio.setChannel(1);                                  // chon kenh 1
  radio.setDataRate(RF24_1MBPS);                   // Указываем скорость передачи данных (RF24_250KBPS, RF24_1MBPS, RF24_2MBPS), RF24_1MBPS - 1Мбит/сек
  radio.setPALevel(RF24_PA_MAX);                 // Указываем мощность передатчика (RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_HIGH=-6dBm, RF24_PA_MAX=0dBm)
  //radio.openWritingPipe (0x6831081001);               // luong 1 va cau hinh dia chi truyen
  radio.openReadingPipe(1, CTR_TX_ADDRESS);
  //radio.startListening();
  //delay(1000);
}
void loop()
{
  radio.startListening();
  char data[32];   
  digitalWrite(led_test,HIGH);
  delay(100);
  if(radio.available())
  {
    digitalWrite(led_received,HIGH);
    delay(1000);
    radio.read(&data, sizeof(data));   
    Serial.print("Received : ");
    Serial.print(data);
  }
  digitalWrite(led_received,LOW);
  digitalWrite(led_test,LOW);
  delay(1000);
  //Serial.print("sangtran");
  //data[0] = 1; // заполняем массив
  //data[1] = 1;   
  //radio.write(&data, sizeof(data)); // отправляем данные и указываем сколько байт пакет
}


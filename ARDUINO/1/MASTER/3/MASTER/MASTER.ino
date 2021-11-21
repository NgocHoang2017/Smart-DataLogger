//----THU VIEN CHO NRF24L01----//
#include <SPI.h>                                            // khai bao file thu vien trong duong cai dat
#include <nRF24L01.h>                                      // khai bao file thu vien define thanh ghi NRF trong cai dat
#include "RF24.h" //khai bao file thu vien trong project
//----THU VIEN CHO NRF24L01----//

//--thu vien EEPROM--//
#include <EEPROM.h>
//--thu vien EEPROM--//

//-------KHAI BAO BIEN DUNG TRONG NRF24L01-------------//
RF24 radio(4, 15);  // khai bao chan(CE, CSN)  
int led_test = 5;
int led_rx = 16;  
int led_tx = 16;
int a=0;
int b=0;
int slave[10];
unsigned char  ID1a[5]= {0x11,0x11,0x11,0x11,0x11};   //5 bytes dia chi truyen/nhan
unsigned char  ID2a[5]= {0x22,0x22,0x22,0x22,0x22};   //5 bytes dia chi truyen/nhan
char data[32];
char check_1a[32] = "yeu cau slave 1a doc du lieu\r\n";   
char check_2a[32] = "yeu cau slave 2a doc du lieu\r\n";   
char ok[32] = "ok\r\n";  

static unsigned long thoi_gian_cho = 0;//dat bien dem thoi gian cho nhan duoc giu lieu
static unsigned long thoi_gian_check = 0;
//-------KHAI BAO BIEN DUNG TRONG NRF24L01-------------//


void setup()
{
  pinMode(led_test,OUTPUT);
  pinMode(led_tx,OUTPUT);
  pinMode(led_rx,OUTPUT);
  EEPROM.begin(512);//khai bao su dung eeprom dung luong 512
  Serial.begin(115200);
  
  SPI.setHwCs(true);
  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST); 
  radio.begin();                                        // khai bao  bat dau nRF24L01+
  radio.setChannel(4);                                  // chon kenh
  radio.setDataRate(RF24_1MBPS);                   // chon toc do truyen (RF24_250KBPS, RF24_1MBPS, RF24_2MBPS), RF24_1MBPS - 1Мбит/сек
  radio.setPALevel(RF24_PA_MAX);                    //dung luong toi da

  //-------dung xoa bo nho epprom vi tri 0 ve gia tri 0------//
  EEPROM.write(1,0);
  EEPROM.write(2,0);
  //-------dung xoa bo nho epprom vi tri 0 ve gia tri 0------//
}
void loop()
{
  //------kiem tra xem da doc du lieu SLAVE 1A trong thoi gian cho phep chua tu trong bo nho EEPROM--------//
  /* 
     =1 la da giao tiep thanh cong
     =0 la chua giao tiep => tien hanh vao che do goi SLAVE
  */
  slave[1]=EEPROM.read(1);
  slave[2]=EEPROM.read(2);
  //Serial.println(slave[1]);
  //Serial.println(slave[2]);
  //--------------------------------------------------------------------------------//

  if(slave[1]==0)
  {
    Slave1a();
  }
  if(slave[2]==0)
  {
    Slave2a();
  }
  
}





void Slave1a()
{
  a=0;
  b=0;
  thoi_gian_cho = 0;
  thoi_gian_check=0;
  
  //truyen yeu cau bat dau doc du lieu
  Serial.println("yeu cau slave 1a doc du lieu\r\n");
  radio.openWritingPipe (ID1a);   //mo kenh
  radio.write(&check_1a, sizeof(check_1a));
  //vao che do doc sau khi truyen yeu cau doc du lieu

  //-------------------------vao che do doc du lieu------------------------//
  radio.openReadingPipe(1, ID1a);
  delay(500);
  while(a==0)//doi den khi nhan duoc chuoi
  {
    Serial.println  ("dang doi du lieu...\r\n");
    radio.startListening();//can than thoi gian chay ham nay qua dong ho thoi gian cua nodemcu
    thoi_gian_cho +=1;
    //Serial.println(thoi_gian_cho);

    rx_1a();
    if(thoi_gian_cho > 50)
    {
      a=1;//reset lai 
      radio.stopListening();
      Serial.println  ("thuc hien gui lai yeu cau truyen du lieu voi slave 1a\r\n");
    }
    yield();
  }
  //-------------------------vao che do doc du lieu------------------------//
}
void rx_1a()
{
  if(radio.available())
  {
    Serial.println("co phan hoi tu slave 1a\r\n");
    radio.read(&data, sizeof(data));
    Serial.print("Received : ");
    Serial.print(data);
    radio.stopListening();//ngung che do nhan du lieu

    while(b==0)
    {
      thoi_gian_check+=1;
      Serial.print("gui ok de check lan cuoi\r\n");
      //Serial.println(thoi_gian_check);
      radio.openWritingPipe (ID1a);   //mo kenh
      radio.write(&ok, sizeof(ok));
      if(thoi_gian_check>50)
      {
        thoi_gian_cho=6000;
        b=1;//thoat che do gui
        a=1;//xac nhan da nhan duoc du lieu va thoat khoi che do doi nhan du lieu
        EEPROM.write(1,1);//gi vao EEPROM vi tri 1 so 1
        Serial.println("******************KET THUC***************************");
      }
      yield();
    }
    //vao che do doc sau khi truyen yeu cau doc du lieu
  }
  else
  {
    delay(200);
    radio.stopListening();
  }
  //radio.stopListening();
}









void Slave2a()
{
  a=0;
  b=0;
  thoi_gian_cho = 0;
  thoi_gian_check=0;
  
  //truyen yeu cau bat dau doc du lieu
  Serial.println("yeu cau slave 2a doc du lieu\r\n");
  radio.openWritingPipe (ID2a);   //mo kenh
  radio.write(&check_2a, sizeof(check_2a));
  //vao che do doc sau khi truyen yeu cau doc du lieu

  //-------------------------vao che do doc du lieu------------------------//
  radio.openReadingPipe(1, ID2a);
  delay(500);
  while(a==0)//doi den khi nhan duoc chuoi
  {
    Serial.println  ("dang doi du lieu...\r\n");
    radio.startListening();//can than thoi gian chay ham nay qua dong ho thoi gian cua nodemcu
    thoi_gian_cho +=1;
    //Serial.println(thoi_gian_cho);

    rx_2a();
    if(thoi_gian_cho > 50)
    {
      a=1;//reset lai 
      radio.stopListening();
      Serial.println  ("thuc hien gui lai yeu cau truyen du lieu voi slave 2a\r\n");
    }
    yield();
  }
  //-------------------------vao che do doc du lieu------------------------//
}
void rx_2a()
{
  if(radio.available())
  {
    Serial.println("co phan hoi tu slave 2a\r\n");
    radio.read(&data, sizeof(data));
    Serial.print("Received : ");
    Serial.print(data);
    radio.stopListening();//ngung che do nhan du lieu

    while(b==0)
    {
      thoi_gian_check+=1;
      Serial.print("gui ok de check lan cuoi\r\n");
      //Serial.println(thoi_gian_check);
      radio.openWritingPipe (ID2a);   //mo kenh
      radio.write(&ok, sizeof(ok));
      if(thoi_gian_check>50)
      {
        thoi_gian_cho=6000;
        b=1;//thoat che do gui
        a=1;//xac nhan da nhan duoc du lieu va thoat khoi che do doi nhan du lieu
        EEPROM.write(2,1);//gi vao EEPROM vi tri 2 so 1
        Serial.println("******************KET THUC 2a***************************");
      }
      yield();
    }
    //vao che do doc sau khi truyen yeu cau doc du lieu
  }
  else
  {
    delay(200);
    radio.stopListening();
  }
  //radio.stopListening();
}













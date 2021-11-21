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
unsigned char  CTR_TX_ADDRESS[5]= {0x68,0x31,0x08,0x10,0x01};   //5 bytes dia chi truyen/nhan
char data[32];
char check[32] = "yeu cau slave 1a doc du lieu\r\n";   
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
  EEPROM.write(0,0);
  //-------dung xoa bo nho epprom vi tri 0 ve gia tri 0------//
}
void loop()
{
  //------kiem tra xem da doc du lieu SLAVE 1A trong thoi gian cho phep chua tu trong bo nho EEPROM--------//
  /* 
     =1 la da giao tiep thanh cong
     =0 la chua giao tiep => tien hanh vao che do goi SLAVE
  */
  slave[1]=EEPROM.read(0);
  //Serial.println(slave[1]);
  //--------------------------------------------------------------------------------//

  
  if(slave[1]!=1)
  {
    a=0;
    b=0;
    thoi_gian_cho = 0;
    thoi_gian_check=0;
    
    //truyen yeu cau bat dau doc du lieu
    Serial.println("yeu cau slave 1a doc du lieu\r\n");
    radio.openWritingPipe (CTR_TX_ADDRESS);   //mo kenh
    radio.write(&check, sizeof(check));
    //vao che do doc sau khi truyen yeu cau doc du lieu

    //-------------------------vao che do doc du lieu------------------------//
    radio.openReadingPipe(1, CTR_TX_ADDRESS);
    while(a==0)//doi den khi nhan duoc chuoi
    {
      Serial.println  ("dang doi du lieu...\r\n");
      radio.startListening();//can than thoi gian chay ham nay qua dong ho thoi gian cua nodemcu
      thoi_gian_cho +=1;
      //Serial.println(thoi_gian_cho);

      rx();
      if(thoi_gian_cho > 50)
      {
        a=1;//reset lai 
        radio.stopListening();
        Serial.println  ("thuc hien gui lai yeu cau truyen du lieu voi slave\r\n");
      }
      yield();
    }
    //-------------------------vao che do doc du lieu------------------------//
  }
}


void rx()
{
  if(radio.available())
  {
    Serial.println("co phan hoi tu slave\r\n");
    radio.read(&data, sizeof(data));
    Serial.print("Received : ");
    Serial.print(data);
    radio.stopListening();//ngung che do nhan du lieu

    while(b==0)
    {
      thoi_gian_check+=1;
      Serial.print("gui ok de check lan cuoi\r\n");
      //Serial.println(thoi_gian_check);
      radio.openWritingPipe (CTR_TX_ADDRESS);   //mo kenh
      radio.write(&ok, sizeof(ok));
      if(thoi_gian_check>50)
      {
        thoi_gian_cho=6000;
        b=1;//thoat che do gui
        a=1;//xac nhan da nhan duoc du lieu va thoat khoi che do doi nhan du lieu
        EEPROM.write(0,1);//gi vao EEPROM vi tri 0 so 1
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



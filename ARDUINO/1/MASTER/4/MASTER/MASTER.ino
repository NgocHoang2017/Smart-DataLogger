//----------------------------------------------------------------------THU VIEN--------------------------------------------------------------------//
//----THU VIEN KET NOI WIFI BANG HTTP----//
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
//----THU VIEN KET NOI WIFI BANG HTTP----//


//----THU VIEN CHO NRF24L01----//
#include <SPI.h>                                            // khai bao file thu vien trong duong cai dat
#include <RF24.h> //khai bao file thu vien trong project
#include <nRF24L01.h>                                      // khai bao file thu vien define thanh ghi NRF trong cai dat
//#include <printf.h> //khai bao file thu vien trong project

//----THU VIEN CHO NRF24L01----//

//--thu vien EEPROM--//
#include <EEPROM.h>
//--thu vien EEPROM--//
//----------------------------------------------------------------------THU VIEN--------------------------------------------------------------------//


//----------------------------------------------------------------------BIEN--------------------------------------------------------------------//
//-------KHAI BAO BIEN DUNG TRONG CHE DO WIFI-------------//
const char *ssid = "DreamLand"; // MK WIFI
const char *password ="emlaymaymedungxaiwifichuanhaem69212169"; //Pass WIFI
const char *host = "192.168.1.6";  // Dia chi IP may
int id1=0;
//-------KHAI BAO BIEN DUNG TRONG CHE DO WIFI-------------//

//-------KHAI BAO BIEN DUNG TRONG NRF24L01-------------//
RF24 radio(4, 15);  // khai bao chan(CE, CSN)  
int led_test = 5;
int led_rx = 16;  
int led_tx = 16;
int a=0;
int b=0;
int slave[10];
const unsigned char  ID1a[5]= {0x11,0x11,0x11,0x11,0x01};   //5 bytes dia chi truyen/nhan-----------const dung de bien kieu bien nay thanh hang so khong thay doi duoc
const unsigned char  ID2a[5]= {0x11,0x11,0x11,0x11,0x02};   //5 bytes dia chi truyen/nhan
char data[32];
char check_1a[32] = "yeu cau slave 1a doc du lieu\r\n";   
char check_2a[32] = "yeu cau slave 2a doc du lieu\r\n";   
char ok[32] = "ok\r\n";  

static unsigned long thoi_gian_cho = 0;//dat bien dem thoi gian cho nhan duoc giu lieu
static unsigned long thoi_gian_check = 0;
//-------KHAI BAO BIEN DUNG TRONG NRF24L01-------------//
//----------------------------------------------------------------------BIEN--------------------------------------------------------------------//

void setup()
{
  
  pinMode(led_test,OUTPUT);
  pinMode(led_tx,OUTPUT);
  pinMode(led_rx,OUTPUT);

  //-----WIFI------//
  delay(1000);
  Serial.begin(115200);
  WiFi.mode(WIFI_OFF);        //Prevents reconnection issue (taking too long to connect)
  delay(1000);
  WiFi.mode(WIFI_STA);        //This line hides the viewing of ESP as wifi hotspot
  
  WiFi.begin(ssid, password);     //Connect to your WiFi router
  Serial.println("");

  Serial.print("Connecting");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }

  // Ket noi duoc neu thanh cong
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  
  //-----WIFI------//


  //-----NRF24L01------//
  //--neu muon thay doi cai hinh spi--//
  SPI.setHwCs(true);                          //dung hay khong dung che do chon chip tren chan cs(thuong thi chi 1 slave thi de false)
  SPI.begin();                                //khoi dong spi
  SPI.setDataMode(SPI_MODE0);                 //SPI_MODE0 0x00 - CPOL: 0 CPHA: 0 |||||CPOL, CPHA: liên quan đến việc thiết lập data dich chuyển là cạnh lên hay cạnh xuống của clock.
  SPI.setBitOrder(MSBFIRST);                  //bit cao di truoc
  //--neu muon thay doi cai hinh spi--//
  radio.begin();                                  // khai bao  bat dau nRF24L01+
  //radio.enableAckPayload();                       //cho phep tuy chinh luong truyen goi ack
  radio.setChannel(4);                            // chon kenh
  radio.setDataRate(RF24_1MBPS);                  // chon toc do truyen (RF24_250KBPS, RF24_1MBPS, RF24_2MBPS), RF24_1MBPS - 1Мбит/сек
  radio.setPALevel(RF24_PA_MAX);                  // dung luong toi da
  
  //yeu cau mo kenh (radio.openWritingPipe (ID2a)) phai duoc thuc hien ngay truoc luc truyen moi truyen duoc
  radio.openReadingPipe(0, ID1a);                 // ham mo luong 1 lang nghe du lieu den o dia chi ID1a//-------luu y: mo la mo luon
  radio.openReadingPipe(0, ID2a);                 // ham mo luong 2 lang nghe du lieu den o dia chi ID2a//
  //radio.stopListening();
  //radio.printDetails();
  //-----NRF24L01------//
  
  //-----EEPROM------//
  EEPROM.begin(512);//khai bao su dung eeprom dung luong 512
  //-------dung xoa bo nho epprom vi tri 0 ve gia tri 0------//
  EEPROM.write(1,0);
  EEPROM.write(2,0);
  //-------dung xoa bo nho epprom vi tri 0 ve gia tri 0------//
  //-----EEPROM------//
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
    Serial.println("");
    Serial.println("");
    Serial.println("");
    Serial.println("");
    delay(5000);
    
  }
  if(slave[2]==0)
  {
    Slave2a();
    Serial.println("");
    Serial.println("");
    Serial.println("");
    Serial.println("");

    delay(5000);
  }
  
}






void Slave1a()
{
  a=0;
  b=0;
  thoi_gian_cho = 0;
  thoi_gian_check=0;
  radio.stopListening();
  //radio.setAutoAck(1,true);

  //truyen yeu cau bat dau doc du lieu moi truong
  radio.openWritingPipe (ID1a);   //mo kenh truyen co dia chi ID2a(mac dinh luon luon truyen tren luong 0(khong doi duoc) vi vay truoc khi truyen phai goi ham nay de neu nhu truoc do co dung luong 0 de doc thi ham nay se chuyen thanh viet tren luong 0)
  Serial.print("yeu cau slave 1a doc du lieu\r\n");
  //Serial.println(check_tx);
  //vao che do doc sau khi truyen yeu cau doc du lieu
  //radio.printDetails();
  //-------------------------vao che do doc du lieu------------------------//
  if(radio.write(&check_1a, sizeof(check_1a)))//neu truyen da thanh cong
  {
    //delay(2000);
    radio.openReadingPipe(0, ID1a);
    radio.startListening();//can than thoi gian chay ham nay qua dong ho thoi gian cua nodemcu
    while(a==0)//doi den khi nhan duoc chuoi
    {
      Serial.println  ("dang doi du lieu...\r\n");
      thoi_gian_cho +=1;
      //Serial.println(thoi_gian_cho);
  
      rx_1a();
      if(thoi_gian_cho > 9000)
      {
        a=1;//reset lai 
        radio.stopListening();
        Serial.println  ("thuc hien gui lai yeu cau truyen du lieu voi slave 1a\r\n");
      }
      yield();
    }
    //-------------------------vao che do doc du lieu------------------------//
  }
  else
  {
    Serial.println("FAILED");
  }
}
void rx_1a()
{
  if(radio.available())
  {
    Serial.println("co phan hoi tu slave 1a\r\n");
    radio.read(&data, sizeof(data));//doc 32 byte (sizeof(data) bang 32) cua du lieu den va luu vao trong bien data
    Serial.print("Received : ");
    Serial.print(data);//show chuoi den
    radio.stopListening();//ngung che do nhan du lieu
    while(b==0)
    {
      thoi_gian_check+=1;
      Serial.print("gui ok de check lan cuoi\r\n");
      //Serial.println(thoi_gian_check);
      radio.openWritingPipe (ID1a);

      if((thoi_gian_check>50))
      {
        thoi_gian_cho=10000;//thoat slave
        b=1;//thoat che do gui
        a=1;//thoat slave
        String data = "C=loi&H=loi&motor=loi";
        truyen_len_server();
        data[32]={};
        Serial.println("******************KET THUC 1a-false***************************");
      }
      if(radio.write(&ok, sizeof(ok)))
      {
        thoi_gian_cho=10000;//thoat slave
        b=1;//thoat che do gui
        a=1;//thoat slave
        EEPROM.write(1,1);//gi vao EEPROM vi tri 1 so 1
        truyen_len_server();
        data[32]={};
        Serial.println("******************KET THUC 1a-ok***************************");
      }
      yield();
    }

    
  }
  //radio.stopListening();
}






void Slave2a()
{
  a=0;
  b=0;
  thoi_gian_cho = 0;
  thoi_gian_check=0;
  radio.stopListening();
  //radio.setAutoAck(1,true);

  //truyen yeu cau bat dau doc du lieu moi truong
  radio.openWritingPipe (ID2a);   //mo kenh truyen co dia chi ID2a(mac dinh luon luon truyen tren luong 0(khong doi duoc) vi vay truoc khi truyen phai goi ham nay de neu nhu truoc do co dung luong 0 de doc thi ham nay se chuyen thanh viet tren luong 0)
  Serial.print("yeu cau slave 2a doc du lieu\r\n");
  //vao che do doc sau khi truyen yeu cau doc du lieu
  //radio.printDetails();
  //-------------------------vao che do doc du lieu------------------------//
  if(radio.write(&check_2a, sizeof(check_2a)))//neu truyen da thanh cong
  {
    //delay(2000);
    radio.openReadingPipe(0, ID2a);
    radio.startListening();//can than thoi gian chay ham nay qua dong ho thoi gian cua nodemcu
    while(a==0)//doi den khi nhan duoc chuoi
    {
      Serial.println  ("dang doi du lieu...\r\n");
      thoi_gian_cho +=1;
      //Serial.println(thoi_gian_cho);
  
      rx_2a();
      if(thoi_gian_cho > 9000)
      {
        a=1;//reset lai 
        radio.stopListening();
        Serial.println  ("thuc hien gui lai yeu cau truyen du lieu voi slave 2a\r\n");    
      }
      yield();
    }
    //-------------------------vao che do doc du lieu------------------------//
  }
  else
  {
    Serial.println("FAILED");
  }
}
void rx_2a()
{
  if(radio.available())
  {
    Serial.println("co phan hoi tu slave 2a\r\n");
    radio.read(&data, sizeof(data));//doc 32 byte (sizeof(data) bang 32) cua du lieu den va luu vao trong bien data
    Serial.print("Received : ");
    Serial.print(data);//show chuoi den
    radio.stopListening();//ngung che do nhan du lieu
    while(b==0)
    {
      thoi_gian_check+=1;
      Serial.print("gui ok de check lan cuoi\r\n");
      //Serial.println(thoi_gian_check);
      radio.openWritingPipe (ID2a);
      if((thoi_gian_check>50))
      {
        thoi_gian_cho=10000;//thoat slave
        b=1;//thoat che do gui
        a=1;//thoat slave
        String data="C=loi&H=loi&motor=loi";
        truyen_len_server();
        data[32]={};
        Serial.println("******************KET THUC 2a-false***************************");
      }
      if(radio.write(&ok, sizeof(ok))){
        thoi_gian_cho=10000;//thoat slave
        b=1;//thoat che do gui
        a=1;//thoat slave
        EEPROM.write(2,1);//gi vao EEPROM vi tri 2 so 1
        truyen_len_server();
        data[32]={};
        Serial.println("******************KET THUC 2a-ok***************************");
      }

      yield();
    }

    
  }
  //radio.stopListening();
}








void truyen_len_server()
{
  HTTPClient http;    

  //String ADCData, station, postData;
  //int adcvalue=50; //bien du lieu
  //int bcd=20;  //bien du lieu
  //ADCData = String(adcvalue);   //String to interger conversion
  //station = String(bcd);
  //Serial.println(ADCData);
  //Serial.println(station);
  // bien nhiet_do,do_am giong bien trong database

  //---gui du lieu len server php--------//
  Serial.print("Send data: ");
  //postData = "nhiet_do=" + ADCData + "&do_am=" + station ;// Chuoi up len 
  Serial.println(data);
  http.begin("http://192.168.1.6:80/doan/esp.php");              //duong dan file php... vi dang ket noi wifi nen phai lay ip may thay vi localhost
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");    //Specify content-type header
  int httpCode = http.POST(data);   //Send the request
  //---gui du lieu len server php--------//

  //---nhan phan hoi tu server php--------//
  String payload = http.getString();    //Get the response payload
  Serial.println(httpCode);   //Print HTTP return code
  Serial.println(payload);    //Print request response payload
  //---nhan phan hoi tu server php--------//

  
  http.end();  //Close connection
  
  delay(1000);  //Post Data at every 2 seconds
}



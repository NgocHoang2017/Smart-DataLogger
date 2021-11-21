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

//--thu vien xu ly chuoi--//
#include <string.h>
//--thu vien xu ly chuoi--//
//----------------------------------------------------------------------THU VIEN--------------------------------------------------------------------//


//----------------------------------------------------------------------BIEN--------------------------------------------------------------------//
//-------KHAI BAO BIEN DUNG TRONG CHE DO WIFI-------------//
const char *ssid = "DreamLand"; // MK WIFI
const char *password ="emlaymaymedungxaiwifichuanhaem69212169"; //Pass WIFI
//const char *ssid = "BATU-U01KNTEwRk4"; // MK WIFI
//const char *password ="sangtran0897"; //Pass WIFI
//const char *host = "192.168.43.106";  // Dia chi IP may(dien thoai)
const char *host = "192.168.1.7";//Dia chi IP may
//const char *host = "https://smartdataloggervn.000webhostapp.com";  
//-------KHAI BAO BIEN DUNG TRONG CHE DO WIFI-------------//

//-------KHAI BAO BIEN DUNG TRONG NRF24L01-------------//
RF24 radio(4, 15);  // khai bao chan(CE, CSN)  
int led_test = 5;
int led_rx = 16;  
int led_tx = 16;
int a=0;
int b=0;
int c=0;
int next_1=0;
int next_2=0;
int slave[10];
const unsigned char  ID1a[5]= {0x11,0x11,0x11,0x11,0x01};   //5 bytes dia chi truyen/nhan-----------const dung de bien kieu bien nay thanh hang so khong thay doi duoc
const unsigned char  ID1b[5]= {0x11,0x11,0x11,0x11,0x03};
const unsigned char  ID2a[5]= {0x11,0x11,0x11,0x11,0x02};   //5 bytes dia chi truyen/nhan
const unsigned char  ID2b[5]= {0x11,0x11,0x11,0x11,0x04};   //5 bytes dia chi truyen/nhan
char data[32];//du lieu nhan
char data_b[32];//du lieu nhan Slave b
char kiem_data[10];//dung kiem tra motor
char check_1a[32] = "yeu cau slave 1a doc du lieu\r\n";     
char check_2a[32] = "yeu cau slave 2a doc du lieu\r\n";   
char ok[32] = "ok\r\n";  
char id_1[100]="id=1&";//du lieu truyen len server 1
char id_2[100]="id=2&";//du lieu truyen len server 2
static unsigned long thoi_gian_cho = 0;//dat bien dem thoi gian cho nhan duoc giu lieu
static unsigned long thoi_gian_check = 0;
static unsigned long thoi_gian_gui_lenh = 0;//dung cho Slave b
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
  
//  //-----EEPROM------//
//  EEPROM.begin(512);//khai bao su dung eeprom dung luong 512
//  //-------dung xoa bo nho epprom vi tri 0 ve gia tri 0------//
//  EEPROM.write(1,0);
//  EEPROM.write(2,0);
//  //-------dung xoa bo nho epprom vi tri 0 ve gia tri 0------//
//  //-----EEPROM------//
}
void loop()
{
//  //------kiem tra xem da doc du lieu SLAVE 1A trong thoi gian cho phep chua tu trong bo nho EEPROM--------//
//  /* 
//     =1 la da giao tiep thanh cong
//     =0 la chua giao tiep => tien hanh vao che do goi SLAVE
//  */
//  slave[1]=EEPROM.read(1);
//  slave[2]=EEPROM.read(2);
//  //Serial.println(slave[1]);
//  //Serial.println(slave[2]);
//  //--------------------------------------------------------------------------------//
//
//  if(slave[1]==0)
//  {
//    
//  }
//  if(slave[2]==0)
//  {
//    
//  }
//--------------------------------------------------------------------------------//



  
  Slave1a();
  if(next_1==1)
  {
    //-------------xu li truyen yeu cau bat tat bom-------------//
    strncpy(kiem_data, data+20,sizeof(data)-20);//gan vao chuoi kiem_data  bat dau tu ki tu thu 20 den do dai cua chuoi data
    //Serial.println(kiem_data);
    if(strcmp(kiem_data,"ON_1b")==0 || strcmp(kiem_data,"OFF_1b")==0)
    {
      Slave1b();
    }
    else
    {
      Serial.println("loi he thong");
      strcpy(data, "");
      strcpy(data, "C=loi&H=loi&motor=loi");//cap nhat he thong loi
      truyen_len_serve_slave1();
    }
    strcpy(kiem_data, "");//xoa cac ki tu trong kiem_data
    strcpy(data, "");//xoa cac ki tu trong data
    //-------------xu li truyen yeu cau bat tat bom-------------//
  }

  Serial.println("********************************************************************************************************************************************************************************************");
  Serial.println("");
  //khoang thoi gian giua viec goi cac Slave
  for(int i=0;i<5;i++)
  {
    //Serial.println("");
    delay(500);
  }
  //khoang thoi gian giua viec goi cac Slave

  
  Slave2a();
  if(next_2==1)
  {
    //-------------xu li truyen yeu cau bat tat bom-------------//
    strncpy(kiem_data, data+20,sizeof(data)-20);//gan vao chuoi kiem_data  bat dau tu ki tu thu 20 den do dai cua chuoi data
    //Serial.println(kiem_data);
    if(strcmp(kiem_data,"ON_2b")==0 || strcmp(kiem_data,"OFF_2b")==0)
    {
      Slave2b();
    }
    else
    {
      Serial.println("loi he thong");
      strcpy(data, "");
      strcpy(data, "C=loi&H=loi&motor=loi");//cap nhat he thong loi
      truyen_len_serve_slave2();
    }
    strcpy(kiem_data, "");//xoa cac ki tu trong kiem_data
    strcpy(data, "");//xoa cac ki tu trong data
    //-------------xu li truyen yeu cau bat tat bom-------------//

  }





  //khoang thoi gian cap nhat lai//
  for(int i=0;i<100;i++)
  {
    Serial.println("");
    delay(10);
  }
  //khoang thoi gian cap nhat lai//
}









void Slave1a()
{
  a=0;
  b=0;
  c=0;
  next_1=0;
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
  //delay(1000);
  //-------------------------vao che do doc du lieu------------------------//
  if(radio.write(&check_1a, sizeof(check_1a)))//neu truyen da thanh cong
  {
    //delay(2000);
    radio.openReadingPipe(0, ID1a);
    radio.startListening();//can than thoi gian chay ham nay qua dong ho thoi gian cua nodemcu
    Serial.println  ("dang doi du lieu...\r\n");
    while(a==0)//doi den khi nhan duoc chuoi
    {
      delay(5);//delay de co the nhan du lieu duoc <=> voi khoang thoi gian Serial.println  ("dang doi du lieu...\r\n");
      //Serial.println  ("dang doi du lieu...\r\n");
      thoi_gian_cho +=1;
      //Serial.println(thoi_gian_cho);
  
      rx_1a();
      if(thoi_gian_cho > 9000 && c==0)
      {
        a=1;//reset lai 
        radio.stopListening();

        
        strcpy(data, "C=loi&H=loi&motor=loi2");//cap nhat he thong loi
        truyen_len_serve_slave1();
        strcpy(data, "");//xoa cac ki tu trong data
        
        //Serial.println  ("thuc hien gui lai yeu cau truyen du lieu voi slave 1a\r\n");
      }
      yield();
    }
    //-------------------------vao che do doc du lieu------------------------//
  }
  else
  {
    //Serial.println("FAILED");
    strcpy(data, "C=loi&H=loi&motor=loi1");//cap nhat he thong loi
    truyen_len_serve_slave1();
    //strcpy(data, "");//xoa cac ki tu trong data
  }
}
void rx_1a()
{
  if(radio.available())
  {
    Serial.println("co phan hoi tu slave 1a\r\n");
    radio.read(&data, sizeof(data));//doc 32 byte (sizeof(data) bang 32) cua du lieu den va luu vao trong bien data
    Serial.print("Received : ");
    Serial.println(data);//show chuoi den
    radio.stopListening();//ngung che do nhan du lieu
    Serial.print("gui ok de check lan cuoi\r\n");
    while(b==0)
    {
      thoi_gian_check+=1;
      Serial.println();
      //Serial.print("gui ok de check lan cuoi\r\n");
      //Serial.println(thoi_gian_check);
      radio.openWritingPipe (ID1a);

      if((thoi_gian_check>50))
      {
        thoi_gian_cho=10000;//thoat slave
        b=1;//thoat che do gui
        a=1;//thoat slave
        c=1;//de khong vao che do cho du lieu nua
        strcpy(data, "C=loi&H=loi&motor=loi3");//cap nhat he thong loi
        Serial.println(data);
        truyen_len_serve_slave1();
        //strcpy(data, "");//xoa cac ki tu trong data
        Serial.println("******************KET THUC 1a-false***************************");
      }
      if(radio.write(&ok, sizeof(ok)))
      {
        thoi_gian_cho=10000;//thoat slave
        b=1;//thoat che do gui
        a=1;//thoat slave
        c=1;//de khong vao che do cho du lieu nua
        //EEPROM.write(1,1);//gi vao EEPROM vi tri 1 so 1
        next_1=1;
        //truyen_len_serve_slave1();
        //strcpy(data, "");//xoa cac ki tu trong data
        //Serial.println(data);
        
        Serial.println("******************KET THUC 1a-ok***************************");
      }
      yield();
    }

    
  }
  //radio.stopListening();
}

//----------------------------------------//

void Slave1b()
{
  a=0;
  b=0;
  c=0;
  thoi_gian_cho = 0;
  thoi_gian_check=0;
  radio.stopListening();

  //truyen yeu cau bat dau doc du lieu bom 1b
  radio.openWritingPipe (ID1b);   //mo kenh truyen co dia chi ID2a(mac dinh luon luon truyen tren luong 0(khong doi duoc) vi vay truoc khi truyen phai goi ham nay de neu nhu truoc do co dung luong 0 de doc thi ham nay se chuyen thanh viet tren luong 0)
  Serial.print("yeu cau slave 1b: ");
  Serial.println(kiem_data);
  //vao che do doc sau khi truyen yeu cau doc du lieu
  //radio.printDetails();
  //delay(1000);
  //-------------------------vao che do doc du lieu------------------------//
  if(radio.write(&kiem_data, sizeof(kiem_data)))//neu truyen da thanh cong
  {
    //delay(2000);
    radio.openReadingPipe(0, ID1b);
    radio.startListening();//can than thoi gian chay ham nay qua dong ho thoi gian cua nodemcu
    Serial.println  ("dang doi du lieu 1b...\r\n");
    while(a==0)//doi den khi nhan duoc chuoi
    {
      delay(5);//delay de co the nhan du lieu duoc <=> voi khoang thoi gian Serial.println  ("dang doi du lieu...\r\n");
      //Serial.println  ("dang doi du lieu 1b...\r\n");
      thoi_gian_cho +=1;
      //Serial.println(thoi_gian_cho);
  
      rx_1b();
      if(thoi_gian_cho > 9000 && c==0)
      {
        a=1;//reset lai 
        radio.stopListening();
        strcpy(kiem_data, "");//xoa cac ki tu trong kiem_data
        strncpy(kiem_data, data+0,20);//gan vao chuoi kiem_data gia tri tu 0 den 20 cua data
        strcpy(data, kiem_data); // Gán chuỗi data có giá trị là chuỗi kiem_data
        strcat(data, "loi5"); // nỗi chuỗi "loi" vào sau chuỗi data
        //Serial.println("data");
        truyen_len_serve_slave1();
        //Serial.println  ("thuc hien gui lai yeu cau truyen du lieu voi slave 1b\r\n");
      }
      yield();
    }
    //-------------------------vao che do doc du lieu------------------------//
  }
  else
  {
    strcpy(kiem_data, "");//xoa cac ki tu trong kiem_data
    strncpy(kiem_data, data+0,20);//gan vao chuoi kiem_data gia tri tu 0 den 20 cua data
    strcpy(data, kiem_data); // Gán chuỗi data có giá trị là chuỗi kiem_data
    strcat(data, "loi4"); // nỗi chuỗi "loi" vào sau chuỗi data
    //Serial.println("data");
    truyen_len_serve_slave1();
  }
}
void rx_1b()
{
  if(radio.available())
  {
    Serial.println("co phan hoi tu slave 1b");
    radio.read(&data_b, sizeof(data_b));//doc 32 byte (sizeof(data) bang 32) cua du lieu den va luu vao trong bien data
    Serial.print("Received : ");
    Serial.println(data_b);//show chuoi den
    radio.stopListening();//ngung che do nhan du lieu
    Serial.println("gui lenh bat-tat bom");
    while(b==0)
    {
      thoi_gian_check+=1;
      delay(5);//delay de co the truyen du lieu duoc <=> voi khoang thoi gian Serial.println  ("gui lenh bat-tat bom\r\n");
      //Serial.println("gui lenh bat-tat bom");
      //Serial.println(thoi_gian_check);
      radio.openWritingPipe (ID1b);
      if((thoi_gian_check>50))
      {
        thoi_gian_cho=10000;//thoat slave
        b=1;//thoat che do gui
        a=1;//thoat slave
        c=1;//de khong vao che do cho du lieu nua
        
        strcpy(kiem_data, "");//xoa cac ki tu trong kiem_data
        strncpy(kiem_data, data+0,20);//gan vao chuoi kiem_data gia tri tu 0 den 20 cua data
        strcpy(data, kiem_data); // Gán chuỗi data có giá trị là chuỗi kiem_data
        strcat(data, "loi6"); // nỗi chuỗi "loi" vào sau chuỗi data
        //Serial.println("data");
        truyen_len_serve_slave1();
      }
      if(strcmp(data_b,"ON_1b")==0 || strcmp(data_b,"OFF_1b")==0 || strcmp(data_b,"Khong tat duoc bom")==0 || strcmp(data_b,"Khong tat duoc bom")==0)//neu da dung trang thai yeu cau thi khong gui gui
      {
        if(radio.write(&ok, sizeof(ok)))
        {
          thoi_gian_cho=10000;//thoat slave
          b=1;//thoat che do gui
          a=1;//thoat slave
          c=1;//de khong vao che do cho du lieu nua
          truyen_len_serve_slave1();
          //strcpy(data, "");//xoa cac ki tu trong data
          //Serial.println(data);
        }
      }
      else
      {
        thoi_gian_cho=10000;//thoat slave
        b=1;//thoat che do gui
        a=1;//thoat slave
        c=1;//de khong vao che do cho du lieu nua
        strcpy(kiem_data, "");//xoa cac ki tu trong kiem_data
        strncpy(kiem_data, data+0,20);//gan vao chuoi kiem_data gia tri tu 0 den 20 cua data
        strcpy(data, kiem_data); // Gán chuỗi data có giá trị là chuỗi kiem_data
        strcat(data, "loi7"); // nỗi chuỗi "loi" vào sau chuỗi data
        //Serial.println("data");
        truyen_len_serve_slave1();
      }
      yield();
    } 
  }
}

//----------------------------------------------------------------------------------------------------------------------1---------------------------------------------------------------//
//----------------------------------------------------------------//-------------------------------------------------------------------------------------------------------//-----------//
//----------------------------------------------------------------------------------------------------------------------2---------------------------------------------------------------//




void Slave2a()
{
  a=0;
  b=0;
  c=0;
  next_2=0;
  thoi_gian_cho = 0;
  thoi_gian_check=0;
  radio.stopListening();
  //radio.setAutoAck(1,true);

  //truyen yeu cau bat dau doc du lieu moi truong
  radio.openWritingPipe (ID2a);   //mo kenh truyen co dia chi ID2a(mac dinh luon luon truyen tren luong 0(khong doi duoc) vi vay truoc khi truyen phai goi ham nay de neu nhu truoc do co dung luong 0 de doc thi ham nay se chuyen thanh viet tren luong 0)
  Serial.print("yeu cau slave 2a doc du lieu\r\n");
  //Serial.println(check_tx);
  //vao che do doc sau khi truyen yeu cau doc du lieu
  //radio.printDetails();
  //-------------------------vao che do doc du lieu------------------------//
  if(radio.write(&check_2a, sizeof(check_2a)))//neu truyen da thanh cong
  {
    //delay(2000);
    radio.openReadingPipe(0, ID2a);
    radio.startListening();//can than thoi gian chay ham nay qua dong ho thoi gian cua nodemcu
    Serial.println  ("dang doi du lieu...\r\n");
    while(a==0)//doi den khi nhan duoc chuoi
    {
      delay(5);//delay de co the nhan du lieu duoc <=> voi khoang thoi gian Serial.println  ("dang doi du lieu...\r\n");
      //Serial.println  ("dang doi du lieu...\r\n");
      thoi_gian_cho +=1;
      //Serial.println(thoi_gian_cho);
  
      rx_2a();
      if(thoi_gian_cho > 9000 && c==0)
      {
        a=1;//reset lai 
        radio.stopListening();

        
        strcpy(data, "C=loi&H=loi&motor=loi2");//cap nhat he thong loi
        truyen_len_serve_slave2();
        strcpy(data, "");//xoa cac ki tu trong data
        
        //Serial.println  ("thuc hien gui lai yeu cau truyen du lieu voi slave 2a\r\n");
      }
      yield();
    }
    //-------------------------vao che do doc du lieu------------------------//
  }
  else
  {
    //Serial.println("FAILED");
    strcpy(data, "C=loi&H=loi&motor=loi1");//cap nhat he thong loi
    truyen_len_serve_slave2();
    //strcpy(data, "");//xoa cac ki tu trong data
  }
}
void rx_2a()
{
  if(radio.available())
  {
    Serial.println("co phan hoi tu slave 2a\r\n");
    radio.read(&data, sizeof(data));//doc 32 byte (sizeof(data) bang 32) cua du lieu den va luu vao trong bien data
    Serial.print("Received : ");
    Serial.println(data);//show chuoi den
    radio.stopListening();//ngung che do nhan du lieu
    Serial.print("gui ok de check lan cuoi\r\n");
    while(b==0)
    {
      thoi_gian_check+=1;
      Serial.println();
      //Serial.print("gui ok de check lan cuoi\r\n");
      //Serial.println(thoi_gian_check);
      radio.openWritingPipe (ID2a);

      if((thoi_gian_check>50))
      {
        thoi_gian_cho=10000;//thoat slave
        b=1;//thoat che do gui
        a=1;//thoat slave
        c=1;//de khong vao che do cho du lieu nua
        strcpy(data, "C=loi&H=loi&motor=loi3");//cap nhat he thong loi
        Serial.println(data);
        truyen_len_serve_slave2();
        //strcpy(data, "");//xoa cac ki tu trong data
        Serial.println("******************KET THUC 2a-false***************************");
      }
      if(radio.write(&ok, sizeof(ok)))
      {
        thoi_gian_cho=10000;//thoat slave
        b=1;//thoat che do gui
        a=1;//thoat slave
        c=1;//de khong vao che do cho du lieu nua
        //EEPROM.write(2,1);//gi vao EEPROM vi tri 1 so 1
        next_2=1;
        //truyen_len_serve_slave1();
        //strcpy(data, "");//xoa cac ki tu trong data
        //Serial.println(data);
        
        Serial.println("******************KET THUC 2a-ok***************************");
      }
      yield();
    } 
  }
  //radio.stopListening();
}


//-------------------------------------------------------2B-----------------------------------------------//

void Slave2b()
{
  a=0;
  b=0;
  c=0;
  thoi_gian_cho = 0;
  thoi_gian_check=0;
  radio.stopListening();

  //truyen yeu cau bat dau doc du lieu bom 1b
  radio.openWritingPipe (ID2b);   //mo kenh truyen co dia chi ID2a(mac dinh luon luon truyen tren luong 0(khong doi duoc) vi vay truoc khi truyen phai goi ham nay de neu nhu truoc do co dung luong 0 de doc thi ham nay se chuyen thanh viet tren luong 0)
  Serial.print("yeu cau slave 2b: ");
  Serial.println(kiem_data);
  //vao che do doc sau khi truyen yeu cau doc du lieu
  //radio.printDetails();
  //-------------------------vao che do doc du lieu------------------------//
  if(radio.write(&kiem_data, sizeof(kiem_data)))//neu truyen da thanh cong
  {
    //delay(2000);
    radio.openReadingPipe(0, ID2b);
    radio.startListening();//can than thoi gian chay ham nay qua dong ho thoi gian cua nodemcu
    Serial.println  ("dang doi du lieu 2b...\r\n");
    while(a==0)//doi den khi nhan duoc chuoi
    {
      delay(5);//delay de co the nhan du lieu duoc <=> voi khoang thoi gian Serial.println  ("dang doi du lieu...\r\n");
      //Serial.println  ("dang doi du lieu 2b...\r\n");
      thoi_gian_cho +=1;
      //Serial.println(thoi_gian_cho);
  
      rx_2b();
      if(thoi_gian_cho > 9000 && c==0)
      {
        a=1;//reset lai 
        radio.stopListening();
        strcpy(kiem_data, "");//xoa cac ki tu trong kiem_data
        strncpy(kiem_data, data+0,20);//gan vao chuoi kiem_data gia tri tu 0 den 20 cua data
        strcpy(data, kiem_data); // Gán chuỗi data có giá trị là chuỗi kiem_data
        strcat(data, "loi5"); // nỗi chuỗi "loi" vào sau chuỗi data
        //Serial.println("data");
        truyen_len_serve_slave2();
        Serial.println("loi5");
        //Serial.println  ("thuc hien gui lai yeu cau truyen du lieu voi slave 1b\r\n");
      }
      yield();
    }
    //-------------------------vao che do doc du lieu------------------------//
  }
  else
  {
    strcpy(kiem_data, "");//xoa cac ki tu trong kiem_data
    strncpy(kiem_data, data+0,20);//gan vao chuoi kiem_data gia tri tu 0 den 20 cua data
    strcpy(data, kiem_data); // Gán chuỗi data có giá trị là chuỗi kiem_data
    strcat(data, "loi4"); // nỗi chuỗi "loi" vào sau chuỗi data
    //Serial.println("data");
    truyen_len_serve_slave2();
  }
}
void rx_2b()
{
  if(radio.available())
  {
    Serial.println("co phan hoi tu slave 2b");
    radio.read(&data_b, sizeof(data_b));//doc 32 byte (sizeof(data) bang 32) cua du lieu den va luu vao trong bien data
    Serial.print("Received : ");
    Serial.println(data_b);//show chuoi den
    radio.stopListening();//ngung che do nhan du lieu
    Serial.println("gui lenh bat-tat bom");
    while(b==0)
    {
      thoi_gian_check+=1;
      delay(5);//delay de co the truyen du lieu duoc <=> voi khoang thoi gian Serial.println  ("gui lenh bat-tat bom\r\n");
      //Serial.println("gui lenh bat-tat bom");
      //Serial.println(thoi_gian_check);
      radio.openWritingPipe (ID2b);
      if((thoi_gian_check>50))
      {
        thoi_gian_cho=10000;//thoat slave
        b=1;//thoat che do gui
        a=1;//thoat slave
        c=1;//de khong vao che do cho du lieu nua
        
        strcpy(kiem_data, "");//xoa cac ki tu trong kiem_data
        strncpy(kiem_data, data+0,20);//gan vao chuoi kiem_data gia tri tu 0 den 20 cua data
        strcpy(data, kiem_data); // Gán chuỗi data có giá trị là chuỗi kiem_data
        strcat(data, "loi6"); // nỗi chuỗi "loi" vào sau chuỗi data
        //Serial.println("data");
        truyen_len_serve_slave2();
      }
      if(strcmp(data_b,"ON_2b")==0 || strcmp(data_b,"OFF_2b")==0 || strcmp(data_b,"Khong tat duoc bom")==0 || strcmp(data_b,"Khong tat duoc bom")==0)//neu da dung trang thai yeu cau thi khong gui gui
      {
        if(radio.write(&ok, sizeof(ok)))
        {
          thoi_gian_cho=10000;//thoat slave
          b=1;//thoat che do gui
          a=1;//thoat slave
          c=1;//de khong vao che do cho du lieu nua
          truyen_len_serve_slave2();
          //strcpy(data, "");//xoa cac ki tu trong data
          //Serial.println(data);
        }
      }
      else
      {
        thoi_gian_cho=10000;//thoat slave
        b=1;//thoat che do gui
        a=1;//thoat slave
        c=1;//de khong vao che do cho du lieu nua
        strcpy(kiem_data, "");//xoa cac ki tu trong kiem_data
        strncpy(kiem_data, data+0,20);//gan vao chuoi kiem_data gia tri tu 0 den 20 cua data
        strcpy(data, kiem_data); // Gán chuỗi data có giá trị là chuỗi kiem_data
        strcat(data, "loi7"); // nỗi chuỗi "loi" vào sau chuỗi data
        //Serial.println("data");
        truyen_len_serve_slave2();
      }
      yield();
    } 
  }
}


//----------------------------------------------------------------------------------------------------------------------2---------------------------------------------------------------//







void truyen_len_serve_slave1()
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
  strcat(id_1, data); // nỗi chuỗi data vào sau chuỗi id_1
  //---gui du lieu len server php--------//
  
  Serial.print("Send data: ");
  //postData = "nhiet_do=" + ADCData + "&do_am=" + station ;// Chuoi up len 
  Serial.println(id_1);
  //http.begin("http://192.168.43.106:80/doan/esp.php");              //duong dan file php... vi dang ket noi wifi nen phai lay ip may thay vi localhost
  http.begin("http://192.168.1.7:80/doan/esp.php");              //duong dan file php... vi dang ket noi wifi nen phai lay ip may thay vi localhost
  //http.begin("https://smartdataloggervn.000webhostapp.com/esp.php");    
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");    //Specify content-type header
  int httpCode = http.POST(id_1);   //Send the request
  //---gui du lieu len server php--------//

  //---nhan phan hoi tu server php--------//
  String payload = http.getString();    //Get the response payload
  Serial.println(httpCode);   //Print HTTP return code
  Serial.println(payload);    //Print request response payload
  //---nhan phan hoi tu server php--------//

  
  http.end();  //Close connection
  strcpy(id_1, "id=1&");
  delay(1000);  //Post Data at every 2 seconds
}

void truyen_len_serve_slave2()
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
  strcat(id_2, data); // nỗi chuỗi data vào sau chuỗi id_2
  //---gui du lieu len server php--------//
  Serial.print("Send data: ");
  //postData = "nhiet_do=" + ADCData + "&do_am=" + station ;// Chuoi up len 
  Serial.println(id_2);
  //http.begin("http://192.168.43.106:80/doan/esp.php");              //duong dan file php... vi dang ket noi wifi nen phai lay ip may thay vi localhost
  http.begin("http://192.168.1.7:80/doan/esp.php");              //duong dan file php... vi dang ket noi wifi nen phai lay ip may thay vi localhost
  //http.begin("https://smartdataloggervn.000webhostapp.com/esp.php");    
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");    //Specify content-type header
  int httpCode = http.POST(id_2);   //Send the request
  //---gui du lieu len server php--------//

  //---nhan phan hoi tu server php--------//
  String payload = http.getString();    //Get the response payload
  Serial.println(httpCode);   //Print HTTP return code
  Serial.println(payload);    //Print request response payload
  //---nhan phan hoi tu server php--------//

  
  http.end();  //Close connection
  //strcpy(id_2, "");
  strcpy(id_2, "id=2&"); // khoi phuc du lieu cho id_2
  delay(1000);  //Post Data at every 2 seconds
}



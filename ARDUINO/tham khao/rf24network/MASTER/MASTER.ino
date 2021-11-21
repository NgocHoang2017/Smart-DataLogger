//----------------------------------------------------------------------THU VIEN--------------------------------------------------------------------//
//----THU VIEN KET NOI WIFI BANG HTTP----//
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
//----THU VIEN KET NOI WIFI BANG HTTP----//


//----THU VIEN CHO NRF24L01----//
#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
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
RF24Network network(radio);      // Include the radio in the network
int led_test = 5;
int led_rx = 16;  
int led_tx = 16;
int a=0;
int b=0;
int slave[10];
const unsigned char  this_node[5]  = {0x00,0x00,0x00,0x00,0x00};   //1 bytes dia chi truyen/nhan-----------const dung de bien kieu bien nay thanh hang so khong thay doi duoc
const uint16_t  ID1a[5]  = {0x00,0x00,0x00,0x00,0x01};
const unsigned char  ID2a[5]  = {0x00,0x00,0x00,0x00,0x02};   //5 bytes dia chi truyen/nhan
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
  SPI.begin();
  radio.begin();
  network.begin(4, this_node[5]);  //(channel, node address)
  radio.setDataRate(RF24_1MBPS);
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
    //Slave1a();
  }
  if(slave[2]==0)
  {
    Slave2a();
    delay(2000);
  }
  
}













void Slave2a()
{
  bool check_tx = false;//bien kiem tra lan gui dau
  a=0;
  b=0;
  thoi_gian_cho = 0;
  thoi_gian_check=0;
  network.update();


  //truyen yeu cau bat dau doc du lieu
  Serial.println("yeu cau slave 2a doc du lieu\r\n");
  RF24NetworkHeader header(ID2a[5]);
  check_tx = network.write(header, &check_2a, sizeof(check_2a));//gui toan chuoi check_2a
  //vao che do doc sau khi truyen yeu cau doc du lieu
  
  //-------------------------vao che do doc du lieu------------------------//
  if(check_tx)//neu truyen da thanh cong
  {
    //delay(2000);
    network.update();
    while(a==0)//doi den khi nhan duoc chuoi
    {
      Serial.println  ("dang doi du lieu...\r\n");
      thoi_gian_cho +=1;
      //Serial.println(thoi_gian_cho);
  
      rx_2a();
      if(thoi_gian_cho > 9000)
      {
        a=1;//reset lai 
        Serial.println  ("thuc hien gui lai yeu cau truyen du lieu voi slave 2a\r\n");
      }
      yield();
    }
    //-------------------------vao che do doc du lieu------------------------//
  }
  else
  {
    
  }
}
void rx_2a()
{
  bool t = true;
  while(network.available())
  {
    RF24NetworkHeader header2;
    char data2[32];
    Serial.println("co phan hoi tu slave 2a\r\n");
    network.read(header2, &data2, sizeof(data2));//doc 32 byte (sizeof(data) bang 32) cua du lieu den va luu vao trong bien data
    Serial.print("Received : ");
    Serial.print(data2);//show chuoi den
    
    if (header2.from_node == 0) 
    {    // If data comes from Slalve 2a
      t = false;
    }
  }
  if(!t)
  {
//    while(b==0)
//    {
//      thoi_gian_check+=1;
//      Serial.print("gui ok de check lan cuoi\r\n");
//      //Serial.println(thoi_gian_check);
//      radio.write(&ok, sizeof(ok));
//      if(thoi_gian_check>50)
//      {
//        thoi_gian_cho=10000;
//        b=1;//thoat che do gui
//        a=1;//xac nhan da nhan duoc du lieu va thoat khoi che do doi nhan du lieu
//        EEPROM.write(2,1);//gi vao EEPROM vi tri 2 so 1
//        truyen_len_server();
//        data[32]={};
//        Serial.println("******************KET THUC 2a***************************");
//      }
//      yield();
//    }
    //vao che do doc sau khi truyen yeu cau doc du lieu
  }
  else
  {
    
  }
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



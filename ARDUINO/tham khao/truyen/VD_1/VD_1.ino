
//#include <Wire.h>



#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define led_on HIGH
#define led_off LOW
#define led_Pin 2 //D0
#define led_Pin1 3 //D1
#define led_Pin2 4 //D2
#define led_Pin3 5  //D3
#define led_Pin4 8  
// Khai bao ten cho cac chan Arduino


// Khai bao bien va du lieu
char ReceiveData;
char TransmitData[] = "Ket noi RF \r\n";
//char SW[] = "SW";
unsigned char count = 0;  // Chua so lan nhan SW (0-255)
String buf_string = "";   // Chua du lieu o dang chuoi so.
//Khoi tao giao tiep cho LCD
//LiquidCrystal lcd(22, 24, 26, 23, 25, 27, 29);

void setup()
{
  pinMode(led_Pin, OUTPUT);
  pinMode(led_Pin1, OUTPUT);
  pinMode(led_Pin2, OUTPUT);
  pinMode(led_Pin3, OUTPUT);
  pinMode(led_Pin4,OUTPUT);
  digitalWrite(led_Pin, led_off);
  digitalWrite(led_Pin1, led_off);
  digitalWrite(led_Pin2, led_off);
  digitalWrite(led_Pin3, led_off);
  digitalWrite(led_Pin4,led_off);
  //Serial3.begin(9600,SERIAL_8N1);
  Serial.begin(9600);



  Serial.write(TransmitData);     // Truyen du lieu qua UART
  //Serial.write(0x00);
}
void loop()
{

  if (Serial.available() > 0)
  {
    ReceiveData = Serial.read();     // Doc du lieu tu Serial Port

    switch (ReceiveData)
    {
      case '1':
      {
        digitalWrite(led_Pin, led_on);
        Serial.println("LED 1: ON");
        delay(500);
        digitalWrite(led_Pin, led_off);
        break;
      }

      case '2':
      {
        digitalWrite(led_Pin1, led_on);
        Serial.println("LED 2: ON");
        delay(500);
        digitalWrite(led_Pin1, led_off);
        break;
      }

      case '3':
      {
        digitalWrite(led_Pin2, led_on);
        Serial.println("LED 3 : ON");
        delay(500);
        digitalWrite(led_Pin2, led_off);
        break;
      }

      case '4':
      {
        digitalWrite(led_Pin3, led_on);
        Serial.println("LED 4: ON");
        delay(500);
        digitalWrite(led_Pin3,led_off);
        break;
      }
      case '5':
      {
       digitalWrite(led_Pin, led_on);
       digitalWrite(led_Pin1, led_on);
       digitalWrite(led_Pin2, led_on);
       digitalWrite(led_Pin3, led_on);
       delay(500);
       digitalWrite(led_Pin, led_off);
       digitalWrite(led_Pin1, led_off);
       digitalWrite(led_Pin2, led_off);
       digitalWrite(led_Pin3, led_off);
       Serial.println("LED : ON");
       
       
       
       break;
      
       
      }

      case '0':
      {
        digitalWrite(led_Pin, led_off);
        digitalWrite(led_Pin1, led_off);
        digitalWrite(led_Pin2, led_off);
        digitalWrite(led_Pin3, led_off);
        digitalWrite(led_Pin4,led_on);
        delay(500);
        digitalWrite(led_Pin4,led_off);
        Serial.println("LED : OFF");
        break;
      }
      
    }
  }

}

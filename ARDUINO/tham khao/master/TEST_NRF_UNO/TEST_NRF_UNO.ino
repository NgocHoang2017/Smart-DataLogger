
// Receiver

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
RF24 myRadio (8,10);
const int LED=6;
byte addresses[][6] = {"1Node"};
int dataReceived;
void setup()
{
pinMode(LED,OUTPUT);
Serial.begin(115200);
Serial.println(F("RF24/Simple Receive data Test"));
myRadio.begin();
myRadio.setChannel(108);
myRadio.setPALevel(RF24_PA_MIN);
myRadio.openReadingPipe(1, addresses[0]);
myRadio.startListening();
}
void loop()
{
if (myRadio.available())
{
while (myRadio.available())
{
myRadio.read( &dataReceived, sizeof(dataReceived) );
}
Serial.print("Data received = ");
Serial.println(dataReceived);
if(dataReceived==10)
{
  digitalWrite(LED,HIGH);
  delay(500);
  digitalWrite(LED,LOW);
 
}
else
{
  digitalWrite(LED,LOW);
  delay(500);
}
}
}

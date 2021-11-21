//*************Branchement**************************************************************
//                                                 
//    |------------------------------|---|---|                      
//    |  |___           ________     | 8 | 7 |      
//    |   ___|         (________)    |---|---|     
//    |  |___                        | 6 | 5 |      
//    |   ___|     |                 |---|---|    
//    |  |___      |                 | 4 | 3 |     
//    |   ___|     |                 |---|---|     
//    |  |_________|                 | 2 ||1||     
//    |------------------------------|---|---|    
//
//                                               
//**************************************************************************************

// TRANSSMIT
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
RF24 myRadio (15, 2);//ce,csn
const int SW1=16;
int button;
byte addresses[][6] = {"1Node"};
int dataTransmitted;
void setup()
{
  pinMode(SW1,INPUT);
  //dataTransmitted=10;
  button=0;
  Serial.begin(115200);
  Serial.println(F("RF24/Simple Transmit data Test"));
  //int dataTransmitted = 10;
  myRadio.begin();
  myRadio.setChannel(108);
  myRadio.setPALevel(RF24_PA_MIN);
  myRadio.openWritingPipe( addresses[0]);
  delay(1000);
}
void loop()
{

  dataTransmitted=10;
  delay(2000);
  myRadio.write( &dataTransmitted, sizeof(dataTransmitted) );
  Serial.println(F("Data Transmitted="));
  Serial.println(dataTransmitted);
}


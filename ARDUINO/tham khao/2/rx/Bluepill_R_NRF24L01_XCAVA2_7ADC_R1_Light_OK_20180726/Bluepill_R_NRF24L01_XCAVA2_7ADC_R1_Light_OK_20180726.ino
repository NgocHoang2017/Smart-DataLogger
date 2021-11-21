
// =======================================================================================================
// BUILD OPTIONS (comment out unneeded options)
// =======================================================================================================

#define DEBUG // if not commented out, Serial.print() is active! For debugging only!!


// =======================================================================================================
// INCLUDE LIRBARIES
// =======================================================================================================

// Libraries
#include <SPI.h>
#include <RF24_STM32.h>
// #include <RF24.h> 
// #include "nRF24L01.h"
// #include <printf.h>
#include <Servo.h>
#include <SimpleTimer.h> 
#include <statusLED.h> 

// #include "readVCC.h"

// =======================================================================================================
// PIN ASSIGNMENTS & GLOBAL VARIABLES
// =======================================================================================================

// For XCAVA2

// For Working
const int Servo1_Pin = PA0;       
const int Servo2_Pin = PA1;       
const int Servo3_Pin = PA2;       
const int Servo4_Pin = PA3;       // Reserve

// For Driving and Spinning
const int DriveLeft_Pin = PB0;
const int DriveRight_Pin = PB1;
const int Spin_Pin = PA8;

#define LED1 PB5
#define LED2 PB6
#define LED3 PB7

// Light 
#define LIGHT1 PC13
#define LIGHT2 PC14
#define LIGHT3 PC15

int Servo1_Value = 90;
int Servo2_Value = 90;
int Servo3_Value = 90;
int Servo4_Value = 90;

int LeftDriveESC_Value = 90;
int RightDriveESC_Value = 90;

int SpinESC_Value = 90;

// Vehicle address
int vehicleNumber = 1; // This number must be unique for each vehicle!
const int maxVehicleNumber = 1;

// the ID number of the used "radio pipe" must match with the selected ID on the transmitter!
const uint64_t pipeIn[maxVehicleNumber] = {0xE9E8F0F0B1LL};

// Hardware configuration: Set up nRF24L01 radio on hardware SPI bus & pins 48 (CE) & 49 (CSN)
RF24 radio(PB8, PB9);

// The size of this struct should not exceed 32 bytes
struct RcData 
{
  // For work control
  unsigned long Joy1_Val_X;
  unsigned long Joy1_Val_Y;
  unsigned long Joy2_Val_X;
  unsigned long Joy2_Val_Y;  
  
  unsigned long Joy3_Val_X;    // Drive Left
  unsigned long Joy4_Val_X;    // Drive Right
  unsigned long Joy5_Val_Y;    // Spin

  boolean SW1_state;
  boolean SW2_state;
  boolean SW3_state;
};
RcData data;

// This struct defines data, which are embedded inside the ACK payload
struct ackPayload {
  float vcc; // vehicle vcc voltage
  float batteryVoltage; // vehicle battery voltage
  boolean batteryOk; // the vehicle battery voltage is OK!
  byte channel = 1; // the channel number
};
ackPayload payload;

// Create Servo objects
Servo Servo1;
Servo Servo2;
Servo Servo3; 
Servo Servo4;

Servo LeftDriveESC;
Servo RightDriveESC;
Servo SpinESC;

// Status LED objects
statusLED battLED(false);     // yellow: ON = battery LOW, flashing = battery VERY LOW
statusLED greenLED(false);    // green: ON = ransmitter ON
statusLED yellowLED(false);   // yellow: ON = ransmitter ON
statusLED redLED(false);      // red: ON = ransmitter temporarilly OFF, flashing = Communication failed

statusLED LeftLED(false);
statusLED RightLED(false);

// Timer
SimpleTimer timer;

void servoSetup()
{
    Servo1.attach(Servo1_Pin, 1000, 2000);          // attaches the servo on pin and set min/max 
    Servo2.attach(Servo2_Pin, 1000, 2000);          // Pin 3
    Servo3.attach(Servo3_Pin, 1000, 2000);          // Pin 4
    Servo4.attach(Servo4_Pin, 1000, 2000);          // Pin 5
     
    LeftDriveESC.attach(DriveLeft_Pin, 1000, 2000);
    RightDriveESC.attach(DriveRight_Pin, 1000, 2000);
    SpinESC.attach(Spin_Pin, 1000, 2000);
}

void defaultPosition()
{
  
  data.Joy1_Val_X = 2048;
  data.Joy1_Val_Y = 2048;
  data.Joy2_Val_X = 2048;
  data.Joy2_Val_Y = 2048;   
   
  data.Joy3_Val_X = 2048; 
  data.Joy4_Val_X = 2048;
  data.Joy5_Val_Y = 2048;

  data.SW1_state = 0;
  data.SW2_state = 0;
  data.SW3_state = 0;
  
  Servo1.write(90);
  Servo2.write(90);
  Servo3.write(90);
  Servo4.write(90);   

  LeftDriveESC.write(90);
  RightDriveESC.write(90);
  SpinESC.write(90);
 
}

void setupRadio() 
{
  radio.begin();
  // radio.setChannel(NRFchannel[chPointer]);
  radio.setChannel(1); 

  radio.powerUp();

  // Set Power Amplifier (PA) level to one of four levels: RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH and RF24_PA_MAX

  radio.setPALevel(RF24_PA_HIGH); 
  radio.setDataRate(RF24_250KBPS);
  radio.setAutoAck(true); // Ensure autoACK is enabled
  radio.enableAckPayload();
  radio.enableDynamicPayloads();
  radio.setRetries(5, 5);                    // 5x250us delay (blocking!!), max. 5 retries
  //radio.setCRCLength(RF24_CRC_8);          // Use 8-bit CRC for performance
}

// =======================================================================================================
// MAIN ARDUINO SETUP (1x during startup)
// =======================================================================================================

void setup() 
{
  #ifdef DEBUG
  Serial.begin(115200);
  // printf_begin();
  delay(1000);
  #endif

  // LED setup
  
  greenLED.begin(LED1); // Green LED pin 22
  yellowLED.begin(LED2); // Yellow LED pin 24
  redLED.begin(LED3);   // Red LED pin 26

  // Light 
  LeftLED.begin(LIGHT1);
  RightLED.begin(LIGHT2);
  pinMode(LIGHT3, OUTPUT);

  // battLED.begin(28);  // Battery LED pin 28

  // Radio setup
    setupRadio(); 

  // servo setup
  servoSetup();

  #ifdef DEBUG
  radio.printDetails();
  delay(1000);
  #endif

  radio.openReadingPipe(1, pipeIn[vehicleNumber - 1]);
  radio.startListening();

  // All axis to neutral position
  defaultPosition();

  // timer.setInterval(100, checkBattery); // Check battery voltage every 100ms
}

// =======================================================================================================
// LED
// =======================================================================================================

void led() 
{

  // Red LED (ON = battery empty, blinking = OK)
  if (payload.batteryOk) 
  {
    battLED.flash(140, 150, 500, vehicleNumber); // ON, OFF, PAUSE, PULSES
  } 
  else 
  {
    battLED.off(); // Always ON = battery low voltage
  }
}

// =======================================================================================================
// READ RADIO DATA
// =======================================================================================================

void readRadio() 
{
  static unsigned long lastRecvTime = 0;
  byte pipeNo;

   if (radio.available(&pipeNo)) 
   {
      greenLED.on();
      yellowLED.off();
      redLED.off();
      radio.writeAckPayload(pipeNo, &payload, sizeof(struct ackPayload) );  // prepare the ACK payload
      radio.read(&data, sizeof(struct RcData)); // read the radia data and send out the ACK payload
      lastRecvTime = millis();
      
      #ifdef DEBUG
      Serial.println("============================================================================================================");
      Serial.println("Communication available");
      Serial.print("Joy1_X");
      Serial.print("\t");
      Serial.print("Joy1_Y");
      Serial.print("\t");
      Serial.print("Joy2_X");
      Serial.print("\t");
      Serial.print("Joy2_Y");
      Serial.print("\t");
      
      Serial.print("Joy3_X");
      Serial.print("\t");
      Serial.print("Joy4_X");
      Serial.print("\t");
      Serial.print("Joy5_Y");
      Serial.print("\t");  
     
      Serial.print("SW1");
      Serial.print("\t");
      Serial.print("SW2");
      Serial.print("\t");
      Serial.print("SW3");
      Serial.print("\t");
      Serial.print("\n");
      
      Serial.print(data.Joy1_Val_X);
      Serial.print("\t");
      Serial.print(data.Joy1_Val_Y);
      Serial.print("\t");
      Serial.print(data.Joy2_Val_X);
      Serial.print("\t");
      Serial.print(data.Joy2_Val_Y);
      Serial.print("\t");
      
      Serial.print(data.Joy3_Val_X);
      Serial.print("\t");
      Serial.print(data.Joy4_Val_X);
      Serial.print("\t");
      Serial.print(data.Joy5_Val_Y);
      Serial.print("\t");
      
      Serial.print(data.SW1_state);
      Serial.print("\t");
      Serial.print(data.SW2_state);
      Serial.print("\t");
      Serial.print(data.SW3_state);
      Serial.print("\t");
      Serial.print("\n"); 
      Serial.println("============================================================================================================");
  
      #endif
  }
  else
  {
    Serial.println("Communication is temporarilly off!!!!!!!!!!!!!");
    yellowLED.on();
  }

  if (millis() - lastRecvTime > 1000) 
  { 
    redLED.flash(30, 100, 0, 0); // 30, 100);
    greenLED.off();
    yellowLED.off();
    // bring all servos to their middle position, if no RC signal is received during 1s!
    defaultPosition();    
    
    #ifdef DEBUG
    Serial.println("Error - No Radio Available - Check Transmitter!");
    #endif
  }
  /*
  else 
  {
      // greenLED.flash(30, 100, 0, 0); // 30, 100
      greenLED.on();
      redLED.off();
      
      #ifdef DEBUG
      Serial.println("Data successfully transmitted");
      Serial.print("Vcc : ");
      Serial.println(payload.vcc);
      #endif
    }
    */
   
}

// =======================================================================================================
// CHECK RX BATTERY VOLTAGE (Battery is NOT monitored)
// =======================================================================================================

/*
void checkBattery() 
{
  payload.vcc = readVcc() / 1000.0 ;

  if (payload.vcc >= 3.0) 
  {
    payload.batteryOk = true;
    #ifdef DEBUG
    Serial.print(payload.vcc);
    Serial.println(" Vcc OK");
    #endif
  } 
  else 
  {
    payload.batteryOk = false;
    #ifdef DEBUG
    Serial.print(payload.vcc);
    Serial.println(" Vcc Low!");
    #endif
  }
}

*/

// =======================================================================================================
// MAIN LOOP
// =======================================================================================================

void loop() 
{
  // Timer
  timer.run();

  // Read radio data from transmitter
  readRadio();

  WorkControl(data.Joy1_Val_X, data.Joy1_Val_Y, data.Joy2_Val_X, data.Joy2_Val_Y); 

  DriveSpinControl(data.Joy3_Val_X, data.Joy4_Val_X, data.Joy5_Val_Y);

  // Light control
  LIGHT_control(data.SW1_state, data.SW2_state, data.SW3_state);

  Direction_LIGHT_control(data.Joy3_Val_X, data.Joy4_Val_X);
 
  // LED
  led();
}


// =======================================================================================================
// WRITE SERVO POSITIONS
// =======================================================================================================


// Work Control
void WorkControl(int Joy1_Value_X, int Joy1_Value_Y, int Joy2_Value_X, int Joy2_Value_Y) 
{
    // Bucket
    if(0 <= Joy1_Value_X && Joy1_Value_X <= 1900)
    {
      Servo1_Value = map(Joy1_Value_X, 0, 1900, 5, 80);     
    }
    else if (2200 <= Joy1_Value_X && Joy1_Value_X <= 4095)
    {
      Servo1_Value = map(Joy1_Value_X, 2200, 4095, 100, 175);
    }
    else
    {
      Servo1_Value = 90;
    }

    // Arm
    if(0 <= Joy1_Value_Y && Joy1_Value_Y <= 1900)
    {
      Servo2_Value = map(Joy1_Value_Y, 0, 1900, 5, 80);     
    }
    else if (2200 <= Joy1_Value_Y && Joy1_Value_Y <= 4095)
    {
      Servo2_Value = map(Joy1_Value_Y, 2200, 4095, 100, 175);
    }
    else
    {
      Servo2_Value = 90;
    }
    
    // Boom
    if(0 <= Joy2_Value_X && Joy2_Value_X <= 1900)
    {
      Servo3_Value = map(Joy2_Value_X, 0, 1900, 5, 80);      
    }
    else if (2200 <= Joy2_Value_X && Joy2_Value_X <= 4095)
    {
      Servo3_Value = map(Joy2_Value_X, 2200, 4095, 100, 175);
    }
    else
    {
      Servo3_Value = 90;
    }

    // Reserve
    if(0 <= Joy2_Value_Y && Joy2_Value_Y <= 1900)
    {
      Servo4_Value = map(Joy2_Value_Y, 0, 1900, 5, 80);
    }
    else if (2200 <= Joy2_Value_Y && Joy2_Value_Y <= 4095)
    {
      Servo4_Value = map(Joy2_Value_Y, 2200, 4095, 100, 175);
    }
    else
    {
      Servo4_Value = 90;
    }     


    Serial.print("Servo1_Value = ");
    Serial.println(Servo1_Value);
    Servo1.write(Servo1_Value);
    // Servo1.writeMicroseconds(Servo1_Value);
    
    Serial.print("Servo2_Value = ");
    Serial.println(Servo2_Value);
    Servo2.write(Servo2_Value);
    //Servo2.writeMicroseconds(Servo2_Value);

    Serial.print("Servo3_Value = ");
    Serial.println(Servo3_Value);
    Servo3.write(Servo3_Value);
    
    Serial.print("Servo4_Value = ");
    Serial.println(Servo4_Value);
    Servo4.write(Servo4_Value);    

    Serial.println("");   
   
}

// Drive and Spin Control
void DriveSpinControl(int Joy3_Value_X, int Joy4_Value_X, int Joy5_Value_Y) 
{
    // Drive left
    if(0 <= Joy3_Value_X && Joy3_Value_X <= 1900)
    {
      LeftDriveESC_Value = map(Joy3_Value_X, 0, 1900, 5, 80);     
    }
    else if (2200 <= Joy3_Value_X && Joy3_Value_X <= 4100)
    {
      LeftDriveESC_Value = map(Joy3_Value_X, 2200, 4100, 100, 175);
    }
    else
    {
      LeftDriveESC_Value = 90;
    }

    // Drive right
    if(0 <= Joy4_Value_X && Joy4_Value_X <= 1900)
    {
      // RightDriveESC_Value = map(Joy4_Value_X, 0, 45, 10, 80);     
      RightDriveESC_Value = map(Joy4_Value_X, 0, 1900, 175, 100); 
    }
    else if (2200 <= Joy4_Value_X && Joy4_Value_X <= 4100)
    {
      // RightDriveESC_Value = map(Joy4_Value_X, 55, 100, 100, 170);
      RightDriveESC_Value = map(Joy4_Value_X, 2200, 4100, 80, 5);
    }
    else
    {
      RightDriveESC_Value = 90;
    }

    // Spin
    if(0 <= Joy5_Value_Y && Joy5_Value_Y <= 1900)
    {
      SpinESC_Value = map(Joy5_Value_Y, 0, 1900, 5, 80);     
    }
    else if (2200 <= Joy5_Value_Y && Joy5_Value_Y <= 4100)
    {
      SpinESC_Value = map(Joy5_Value_Y, 2200, 4100, 100, 175);
    }
    else
    {
      SpinESC_Value = 90;
    }
        
    Serial.print("LeftDriveESC_Value = ");
    Serial.println(LeftDriveESC_Value);
    LeftDriveESC.write(LeftDriveESC_Value);

    Serial.print("RightDriveESC_Value = ");
    Serial.println(RightDriveESC_Value);
    RightDriveESC.write(RightDriveESC_Value);
    
    Serial.print("SpinESC_Value = ");
    Serial.println(SpinESC_Value);
    SpinESC.write(SpinESC_Value);
} 

// Light control function
void LIGHT_control(unsigned int SW1_state, unsigned int SW2_state,unsigned int SW3_state)
{
  /*
  if (SW1_state)
  {
    digitalWrite(LIGHT1, HIGH);
  }
  else
  {
    digitalWrite(LIGHT1, LOW);    
  }
  
  // SW2
  if (SW1_state)
  {
    digitalWrite(LIGHT2, HIGH);
  }
  else
  {
    digitalWrite(LIGHT2, LOW);    
  }
 */ 
  // SW3
  if (SW1_state)
  {
    digitalWrite(LIGHT3, HIGH);
  }
  else
  {
    digitalWrite(LIGHT3, LOW);    
  }

}


void Direction_LIGHT_control(int Joy3_Value_X, int Joy4_Value_X)
{
  if((0 <= Joy3_Value_X && Joy3_Value_X <= 1900) || (2200 <= Joy3_Value_X && Joy3_Value_X <= 4095))
  {
     // digitalWrite(LIGHT1, HIGH);
     // delay(1000);
     // digitalWrite(LIGHT1, LOW);

     LeftLED.flash(30, 100, 0, 0); // 30, 100);
  }
  else
  {
     // digitalWrite(LIGHT1, LOW);
     LeftLED.off();
  }  

  if((0 <= Joy4_Value_X && Joy4_Value_X <= 1900) || (2200 <= Joy4_Value_X && Joy4_Value_X <= 4095))
  {
     // digitalWrite(LIGHT2, HIGH);
     // delay(1000);
     // digitalWrite(LIGHT2, LOW);
     RightLED.flash(30, 100, 0, 0); // 30, 100);
  }
  else
  {
     // digitalWrite(LIGHT2, LOW);
     RightLED.off();
  }  
}


// Safe mode opertion function

/*
void resetdata()
{    
    Servo1.write(90);
    Servo2.write(90);
    Servo3.write(90);
    Servo4.write(90);   
    Servo5.write(90);
    Servo6.write(90);

    ServoCAM1.write(90);
    ServoCAM2.write(90);

    LeftDriveESC.write(90);
    RightDriveESC.write(90);

    SpinESC.write(90);
}
*/

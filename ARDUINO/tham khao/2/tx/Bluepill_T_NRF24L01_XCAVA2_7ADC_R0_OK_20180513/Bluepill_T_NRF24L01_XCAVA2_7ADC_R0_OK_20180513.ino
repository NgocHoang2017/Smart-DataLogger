
// *****************************************************************
// Include libraries
// ****************************************************************
#include <SPI.h> 
// #include <nRF24L01.h>
#include <RF24_STM32.h>
// #include <RF24.h>
#include <statusLED.h> 
#include "transmitterConfig.h"

// ***************************************************************
// Pin assignments and global variables
// ***************************************************************

/* NRF24L01 Pin Assign
 *  SCK (Green) PA5
 *  MISO (Blue) PA6
 *  MOSI (Yellow) PA70
 *  CE  PB8
 *  CSN PB9
*/

#define CE_PIN   PB8
#define CSN_PIN  PB9

// For Working
#define Joy1_X PA0       // joystick1 x, Bucket
#define Joy1_Y PA1       // joystick1 y, Arm
#define Joy2_X PA2       // joystick2 x, Boom
#define Joy2_Y PA3       // joystick2 y, Reserve

// For Drive & Spin
#define Joy3_X PB0       // joystick3 x axis
#define Joy4_X PB1       // Drive joystick left

#define Joy5_Y PA4       // Drive joystic right

#define SW1 PC13          // Switch 1
#define SW2 PC14          // Switch 2
#define SW3 PC15          // Switch 3

#define LED1 PB5
#define LED2 PB6
#define LED3 PB7

#define DEBUG // if not commented out, Serial.print() is active! For debugging only!!

byte transmissionMode = 1; // Radio mode is active by default
// Vehicle address
int vehicleNumber = 1; // Vehicle number one is active by default

// Did the receiver acknowledge the sent data?
boolean transmissionState;

// Radio channels (126 channels are supported)
byte chPointer = 0; // Channel 1 (the first entry of the array) is active by default
const byte NRFchannel[] { 1, 2 };

// the ID number of the used "radio pipe" must match with the selected ID on the transmitter!
// 10 ID's are available @ the moment
const uint64_t pipeOut[] = {0xE9E8F0F0B1LL};
const int maxVehicleNumber = (sizeof(pipeOut) / (sizeof(uint64_t)));

// Status LED objects (false = not inverted)
statusLED greenLED(false); // green: ON = ransmitter ON, flashing = Communication with vehicle OK
statusLED redLED(false);   // red: ON = battery empty
statusLED yellowLED(false); // yellow : ON = 

// #define DATARATE RF24_2MBPS
// #define DATARATE RF24_1MBPS
// #define DATARATE RF24_250KBPS

RF24 radio(CE_PIN, CSN_PIN); 

// SPIClass SPI_2(2);

struct RcData {
  unsigned long Joy1_Val_X;
  unsigned long Joy1_Val_Y;
  unsigned long Joy2_Val_X;
  unsigned long Joy2_Val_Y;  
  
  unsigned long Joy3_Val_X;
  unsigned long Joy4_Val_X;
  unsigned long Joy5_Val_Y;  

  /*
  unsigned int Joy1_Val_X;
  unsigned int Joy1_Val_Y;
  unsigned int Joy2_Val_X;
  unsigned int Joy2_Val_Y;  
  
  unsigned int Joy3_Val_X;
  unsigned int Joy4_Val_X;
  unsigned int Joy5_Val_Y;  
  */
  
  boolean SW1_state;
  boolean SW2_state;
  boolean SW3_state;
} data;

struct ackPayload
{
  float vcc;              // vehicle vcc voltage
  float batteryVoltage;   // vehicle battery voltage
  boolean batteryOk;      // the vehicle battery voltage is OK!
  byte channel = 1;       // the channel number
} payload;

void resetData( )
{
    data.Joy1_Val_X = 50;
    data.Joy1_Val_Y = 50;
    data.Joy2_Val_X = 50;
    data.Joy2_Val_Y = 50;   
     
    data.Joy3_Val_X = 50;
    data.Joy4_Val_X = 50;
    data.Joy5_Val_Y = 50;


    data.SW1_state = 0;
    data.SW2_state = 0;
    data.SW3_state = 0;    
}

void pinModes()
{
  
  pinMode(PA0, INPUT_ANALOG);
  pinMode(PA1, INPUT_ANALOG);
  pinMode(PA2, INPUT_ANALOG);
  pinMode(PA3, INPUT_ANALOG);

  pinMode(PA4, INPUT_ANALOG); // SPIN
  pinMode(PB0, INPUT_ANALOG); // DIRVE LEFT
  pinMode(PB1, INPUT_ANALOG); // DRIVE RIGHT

  
  pinMode(SW1, INPUT);
  pinMode(SW2, INPUT);
  pinMode(SW3, INPUT);
  
}

// ***********************************************************************************
// Radio setup
// ***********************************************************************************

void setupRadio() 
{
  radio.begin();
  // radio.setChannel(NRFchannel[chPointer]);
  radio.setChannel(1); // matching mega

  radio.powerUp();

  // Set Power Amplifier (PA) level to one of four levels: RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH and RF24_PA_MAX

  // radio.setPALevel(RF24_PA_MAX); // Independent NRF24L01 3.3 PSU, so "FULL" transmission level allowed
  radio.setPALevel(RF24_PA_HIGH); // matching mega
  radio.setDataRate(RF24_250KBPS);
  radio.setAutoAck(true); // Ensure autoACK is enabled
  radio.enableAckPayload();
  radio.enableDynamicPayloads();
  radio.setRetries(5, 5);                    // 5x250us delay (blocking!!), max. 5 retries
  //radio.setCRCLength(RF24_CRC_8);          // Use 8-bit CRC for performance

  #ifdef DEBUG
  radio.printDetails();
  delay(1800);
  #endif
  
  radio.openWritingPipe(pipeOut[vehicleNumber - 1]); // Vehicle Number 1 = Array number 0, so -1!

  radio.write(&data, sizeof(RcData));
}

// ***********************************************************************************
// setup
// ***********************************************************************************

void setup() 
{
  pinModes();
  Serial.begin(115200);

  // Disable STM32 JTAG debug pins! Allow to use Pin PB3 and PB4
  // See http://www.stm32duino.com/viewtopic.phc?f=3&t=2084&p=27945&hilit=pb3#p27916
  disableDebugPorts();
  setupRadio();

  pinMode(SW1, INPUT_PULLUP);
  pinMode(SW2, INPUT_PULLUP);
  pinMode(SW3, INPUT_PULLUP);

  pinMode(Joy1_X, INPUT_ANALOG);
  pinMode(Joy1_Y, INPUT_ANALOG);
  pinMode(Joy2_X, INPUT_ANALOG);
  pinMode(Joy2_Y, INPUT_ANALOG);
  
  pinMode(Joy3_X, INPUT_ANALOG);
  pinMode(Joy4_X, INPUT_ANALOG);
  pinMode(Joy5_Y, INPUT_ANALOG);

/*  
  // Setup SPI 1
  SPI.begin(); // Initial the SPI_1 port
  SPI.setBitOrder(MSBFIRST); // Set the SPI_1 bit order
  SPI.setDataMode(SPI_MODE0); // Set the SPI_1 data mode
  SPI.setClockDivider(SPI_CLOCK_DIV16); // Slow speed (72/16 = 4.5 MHz SPI_1 speed)

  // Setup SPI 2
  SPI_2.begin(); // Initial the SPI_2 port
  SPI_2.setBitOrder(MSBFIRST); // Set the SPI_2 bit order
  SPI_2.setDataMode(SPI_MODE0); // Set the SPI_2 data mode
  SPI_2.setClockDivider(SPI_CLOCK_DIV16); // Slow speed (72/16 = 4.5 MHz SPI_1 speed)
*/  

  // LED setup
  greenLED.begin(LED1);     // Green LED 
  yellowLED.begin(LED2);    // Yellow LED
  redLED.begin(LED3);       // Red LED 
    
  resetData( );
}

// ***********************************************************************************
// loop
// ***********************************************************************************

void loop()   
{   // Start of loop() 

   ReadValues ();
   transmitRadio();
          
}   // End of loop()


// ***********************************************************************************
// Read joysticks, variable resisters, switches
// ***********************************************************************************

void ReadValues ()
{
     data.Joy1_Val_X = analogRead(Joy1_X);
     data.Joy1_Val_Y = analogRead(Joy1_Y);
     data.Joy2_Val_X = analogRead(Joy2_X);
     data.Joy2_Val_Y = analogRead(Joy2_Y);
     
     data.Joy3_Val_X = analogRead(Joy3_X);
     data.Joy4_Val_X = analogRead(Joy4_X);
     
     data.Joy5_Val_Y = analogRead(Joy5_Y);
     /*
     unsigned int Joy1_Val_X_temp = analogRead(Joy1_X);
     data.Joy1_Val_X = Joy1_Val_X_temp*100/4095;
     unsigned int Joy1_Val_Y_temp = analogRead(Joy1_Y);
     data.Joy1_Val_Y = Joy1_Val_Y_temp*100/4095;
     unsigned int Joy2_Val_X_temp = analogRead(Joy2_X);
     data.Joy2_Val_X = Joy2_Val_X_temp*100/4095;
     unsigned int Joy2_Val_Y_temp = analogRead(Joy2_Y);
     data.Joy2_Val_Y = Joy2_Val_Y_temp*100/4095;

     unsigned int Joy3_Val_X_temp = analogRead(Joy3_X);
     data.Joy3_Val_X = Joy3_Val_X_temp*100/4095;
     unsigned int Joy4_Val_X_temp = analogRead(Joy4_X);
     data.Joy4_Val_X = Joy4_Val_X_temp*100/4095;
     
     unsigned int Joy5_Val_Y_temp = analogRead(Joy5_Y);
     data.Joy5_Val_Y = Joy2_Val_Y_temp*100/4095;
     */
     
     data.SW1_state = digitalRead(SW1);
     data.SW2_state = digitalRead(SW2);
     data.SW3_state = digitalRead(SW3);     
     
     // Debugging
     #ifdef DEBUG
     Serial.println("Display Serial print joystick values");
     Serial.println("======================================");
     Serial.print("Joy1_X_Value : ");
     Serial.println(data.Joy1_Val_X);
     Serial.print("Joy1_Y_Value : ");
     Serial.println(data.Joy1_Val_Y);
     Serial.print("Joy2_X_Value : ");
     Serial.println(data.Joy2_Val_X);
     Serial.print("Joy2_Y_Value : ");
     Serial.println(data.Joy2_Val_Y);
     Serial.println("");
     
     Serial.print("Joy3_X_Value : ");
     Serial.println(data.Joy3_Val_X);
     Serial.print("Joy4_X_Value : ");
     Serial.println(data.Joy4_Val_X);
     Serial.print("Joy5_Y_Value : ");
     Serial.println(data.Joy5_Val_Y);
   
     Serial.println("");     
 
     Serial.print("SW1_State :");
     Serial.println(data.SW1_state);
     Serial.print("SW2_State :");
     Serial.println(data.SW2_state);
     Serial.print("SW3_State :");
     Serial.println(data.SW3_state);
     Serial.println("_______________________________________");
     Serial.println(""); 
     #endif     
}

// ***********************************************************************************
// Transmit radio
// ***********************************************************************************

void transmitRadio ( )
{

  static boolean previousTransmissionState;
  static float previousRxVcc;
  static float previousRxVbatt;
  static boolean previousBattState;
  static unsigned long previousSuccessfulTransmission;

  if (transmissionMode == 1) // If radio mode is active
  { 

    // Send radio data and check if transmission was successful
    if (radio.write(&data, sizeof(struct RcData)) ) 
    {
      if (radio.isAckPayloadAvailable()) 
      {
        radio.read(&payload, sizeof(struct ackPayload)); // read the payload, if available
        previousSuccessfulTransmission = millis();
      }
    }

    // if the transmission was not confirmed (from the receiver) after > 1s...
    if (millis() - previousSuccessfulTransmission > 1000) 
    {
      redLED.flash(30, 100, 0, 0); // 30, 100
      greenLED.off();      
      transmissionState = false;
      memset(&payload, 0, sizeof(payload)); // clear the payload array, if transmission error
      
      #ifdef DEBUG
      Serial.println("Data transmission error, check receiver!");
      #endif
    }
    else 
    {
      greenLED.on();
      redLED.off();
      transmissionState = true;
      #ifdef DEBUG
      Serial.println("Data successfully transmitted");
      Serial.print("Vcc : ");
      Serial.println(payload.vcc);
      #endif
    }

    #ifdef DEBUG
    Serial.println(F_CPU / 1000000, DEC); 
    #endif
  }
  else // transmissionMode == 0
  {     
    radio.powerDown();
  }
}


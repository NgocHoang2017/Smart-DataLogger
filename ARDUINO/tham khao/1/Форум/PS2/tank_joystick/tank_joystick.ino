#include <PS2X_lib.h>  //for v1.6

PS2X ps2x; // create PS2 Controller Class

//13 Clock       зеленый
//12 Data        серый
//11 Command     синий
//10 Attention   коричневый
//   3.3V        белый
//   GND         черный

int error = 0; 
byte type = 0;
byte vibrate = 0;
boolean L_napr = 2;
byte L_drive = 3;
boolean R_napr = 4;
byte R_drive = 5;
byte X;
byte Y;
boolean XN;
boolean YN;

void setup(){
  error = ps2x.config_gamepad(13,11,10,12, true, true);   //Настройка выводов: (clock, command, attention, data, Pressures?, Rumble?)
  pinMode(L_napr, OUTPUT);
  pinMode(L_drive, OUTPUT);
  pinMode(R_napr, OUTPUT);
  pinMode(R_drive, OUTPUT);
}


void loop(){

  ps2x.read_gamepad(false, vibrate);
  //===========================================================================================
  //                                        Граничные условия для X и Y
  //===========================================================================================

  if (ps2x.Analog(PSS_RX) == 128){
    X = 0;
  } 
  if (ps2x.Analog(PSS_RX) < 128){
    X = 255-ps2x.Analog(PSS_RX)*2;
    XN = 0;
  }
  if (ps2x.Analog(PSS_RX) > 128){
    X = ps2x.Analog(PSS_RX)*2-255;
    XN = 1;
  }    
  if (ps2x.Analog(PSS_RY) == 128){
    Y = 0;
  } 
  if (ps2x.Analog(PSS_RY) < 128){
    Y = 255-ps2x.Analog(PSS_RY)*2;
    YN = 1;
  }
  if (ps2x.Analog(PSS_RY) > 128){
    Y = ps2x.Analog(PSS_RY)*2-255;
    YN = 0;
  }    

  //===========================================================================================
  //                                        Остановка
  //===========================================================================================
  if (X == 0 && Y == 0){
    digitalWrite(L_napr, LOW);
    analogWrite(L_drive, 0);
    digitalWrite(R_napr, LOW);
    analogWrite(R_drive, 0);
  }
  //===========================================================================================
  //                                        Только вперед
  //===========================================================================================
  if (Y > 0 && YN == 1){
    digitalWrite(L_napr, HIGH);
    analogWrite(L_drive, 255-Y);
    digitalWrite(R_napr, HIGH);
    analogWrite(R_drive, 255-Y);
  }
  //===========================================================================================
  //                                        Только назад
  //===========================================================================================  
  if (Y > 0 && YN == 0){
    digitalWrite(L_napr, LOW);
    analogWrite(L_drive, Y);
    digitalWrite(R_napr, LOW);
    analogWrite(R_drive, Y);
  }
  //===========================================================================================
  //                                        Только вправо
  //===========================================================================================  
  if (X > 0 && Y == 0 && XN == 1){
    digitalWrite(L_napr, HIGH);
    analogWrite(L_drive, 255-X);
    digitalWrite(R_napr, LOW);
    analogWrite(R_drive, 0);
  }
  //===========================================================================================
  //                                        Только влево
  //===========================================================================================  
  if (X > 0 && Y == 0 && XN == 0){
    digitalWrite(L_napr, LOW);
    analogWrite(L_drive, 0);
    digitalWrite(R_napr, HIGH);
    analogWrite(R_drive, 255-X);
  }
  //===========================================================================================
  //                                        Вперед налево
  //===========================================================================================  
  if (X > 0 && Y > 0 && XN == 0 && YN == 1){
    digitalWrite(L_napr, HIGH);
    analogWrite(L_drive, X);
    digitalWrite(R_napr, HIGH);
    analogWrite(R_drive, 255-Y);
  }
  //===========================================================================================
  //                                        Вперед направо
  //===========================================================================================  
  if (X > 0 && Y > 0 && XN == 1 && YN == 1){
    digitalWrite(L_napr, HIGH);
    analogWrite(L_drive, 255-Y);
    digitalWrite(R_napr, HIGH);
    analogWrite(R_drive, X);
  }
  //===========================================================================================
  //                                        Назад налево
  //===========================================================================================  
  if (X > 0 && Y > 0 && XN == 0 && YN == 0){
    digitalWrite(L_napr, LOW);
    analogWrite(L_drive, 255-X);
    digitalWrite(R_napr, LOW);
    analogWrite(R_drive, Y);
  }
  //===========================================================================================
  //                                        Назад направо
  //===========================================================================================  
  if (X > 0 && Y > 0 && XN == 1 && YN == 0){
    digitalWrite(L_napr, LOW);
    analogWrite(L_drive, Y);
    digitalWrite(R_napr, LOW);
    analogWrite(R_drive, 255-X);
  }


}












//..........................................................................................................................//
//..........................................................................................................................//
//                                  Hovercraft Bluetooth Control code for Group 5                                           //
//                                          Written by Andrew and Luke                                                      //
//..........................................................................................................................//
//..........................................................................................................................//
//Includes a functioning timer, payload dropping counter, servo control, bluetooth integration, and LCD for monotoring.     //
//Uses the built in arduino servo and  LCD Library as well as the Curcuits at home USB Host sheild library                  //
//The USB Host sheild Library can be found at https://github.com/felis/USB_Host_Shield_2.0                                  //
//..........................................................................................................................//
//..........................................................................................................................//

//Include statments for libraries
#include <PS4BT.h>
#include <usbhub.h>
#include <Servo.h>
#include <LiquidCrystal.h>

//Initilize the LCD pins
LiquidCrystal lcd(2,3,4,5,6,7);
// VSS GROUND
// VDD 5V
// V0 MIDDLE POTENTIOMETER
// RS 2
// RW GROUND
// E 3
// D4 4
// D5 5
// D6 6
// D7 7
// A 5V
// R GROUND

// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#include <SPI.h>
#endif

//Pairing the Controller
USB Usb;
BTD Btd(&Usb);
PS4BT PS4(&Btd, PAIR);
bool printAngle, printTouch;
uint8_t oldL2Value, oldR2Value;

// state of LCD screen variable
int xstate = 0;

// timiing variables
long next_time = 0;
long time_next1 = 0;
long start_time;
long elapsed;

//timer state variable
int toggle = 0;

//servo position
int pos = 0;

//Payloads dropped display
int payload = 0;

//Define servo variables
Servo myservo;

//..........................................................................................................................//
//..........................................................................................................................//
//Setup

void setup() {
  Serial.begin(115200);
  
  //Set servo pin and starting state
  myservo.attach(8);
  myservo.write(0);
  
  //start the LCD
  lcd.begin(16,2);
  
  //USB host sheild code to satisfy the IDE
  #if !defined(__MIPSEL__)
    while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
  #endif
  if (Usb.Init() == -1) {
    Serial.print(F("\r\nOSC did not start"));
    while (1); // Halt
  }
  Serial.print(F("\r\nPS4 Bluetooth Library Started"));
}

//..........................................................................................................................//
//..........................................................................................................................//
//Loop

void loop() {
  Usb.Task();

//..........................................................................................................................//
//..........................................................................................................................//
//Determine state for LCD

  if (PS4.connected()) {
    long current_time = millis();
    if (PS4.getAnalogHat(LeftHatY) > 137 || PS4.getAnalogHat(LeftHatY) < 117) {
      if (next_time < current_time){
        next_time = current_time + 500;
        if ((xstate==0 && PS4.getAnalogHat(LeftHatY)>200) || (xstate==2 && PS4.getAnalogHat(LeftHatY)<50)){
          xstate=1;
        }
        else{
          if ((xstate==1 && PS4.getAnalogHat(LeftHatY)<50) || (xstate==2 && PS4.getAnalogHat(LeftHatY)>200)){
            xstate=0;
          }
          else{
            if ((xstate==1 && PS4.getAnalogHat(LeftHatY)>200) || (xstate==0 && PS4.getAnalogHat(LeftHatY)<50)){
              xstate=2;
            }
          } 
        } 
      }
    }
    
//..........................................................................................................................// 
//..........................................................................................................................//
//Print to LCD
    
    if (xstate==0){
      lcd.setCursor(0,0);
      lcd.print("Schrodinger's   ");
      lcd.setCursor(0,1);
      lcd.print("           CrAfT");
    }
      
    if (xstate==1){
      lcd.setCursor(0,0);
      lcd.print("Payloads Dropped");
      lcd.setCursor(0,1);
      lcd.print(payload);
      lcd.print("                ");
    }
      
    if (xstate==2){
      lcd.setCursor(0,0);
      lcd.print("  Time Elapsed  ");
      lcd.setCursor(0,1);
      lcd.print(elapsed/1000);
      lcd.print("                ");
    }

//..........................................................................................................................//
//..........................................................................................................................//
//Payload Delivery
  
    if ((PS4.getAnalogButton(R2))&&(current_time > time_next1)&&(pos==0)) {
      pos = pos+45;
      time_next1= millis() + 500;
      myservo.write (pos);
      payload = 1;
    }
    else{
      if ((PS4.getAnalogButton(R2))&&(current_time > time_next1)&&((pos==45)||(pos==105))) {
        pos = pos+60;
        myservo.write (pos);
        time_next1= millis() + 500;
        payload = payload + 1;
      } 
      else{
        if ((PS4.getAnalogButton(R2))&&(current_time > time_next1)&&(pos==165)) {
          pos = 0;
          myservo.write (pos);
          time_next1= millis() + 500;
        }
      }
    }
    
//..........................................................................................................................//
//..........................................................................................................................//
//Timer

    if ((PS4.getButtonClick(CROSS))&&(toggle==0)) {
      toggle = toggle + 1;
      start_time = millis();
    }
    else{
      if(PS4.getButtonClick(CROSS)){
        toggle = toggle + 1;
      }
    }
    
    if (toggle%2==1) {
      elapsed = (millis()-start_time);
    }
    
    if (toggle%2==0) {
      start_time=(start_time+(millis()-elapsed));
    }
    
    if ((PS4.getButtonClick(CIRCLE))) {
      toggle = 0;
      elapsed=0;
    }

//..........................................................................................................................//
//..........................................................................................................................//
//Disconnect

    if (PS4.getButtonClick(PS)) {
      Serial.print(F("\r\nPS"));
      PS4.disconnect();
      //someway to reset the arduino, maybe a wire connected to the arduino reset??
      //this would allow for the ps4 controller to reset the arduino in the event of an issue
    }
  } 
}
//..........................................................................................................................//

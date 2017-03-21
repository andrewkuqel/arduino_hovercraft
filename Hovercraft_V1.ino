/*
 Example sketch for the PS4 Bluetooth library - developed by Kristian Lauszus
 For more information visit my blog: http://blog.tkjelectronics.dk/ or
 send me an e-mail:  kristianl@tkjelectronics.com
 */

#include <PS4BT.h>
#include <usbhub.h>
#include <Servo.h>
#include "LiquidCrystal.h"
#include <StopWatch.h>

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

USB Usb;
//USBHub Hub1(&Usb); // Some dongles have a hub inside
BTD Btd(&Usb); // You have to create the Bluetooth Dongle instance like so

/* You can create the instance of the PS4BT class in two ways */
// This will start an inquiry and then pair with the PS4 controller - you only have to do this once
// You will need to hold down the PS and Share button at the same time, the PS4 controller will then start to blink rapidly indicating that it is in pairing mode
PS4BT PS4(&Btd, PAIR);

// After that you can simply create the instance like so and then press the PS button on the device
//PS4BT PS4(&Btd);

bool printAngle, printTouch;
uint8_t oldL2Value, oldR2Value;
StopWatch sw_millis;    // MILLIS (default)
StopWatch sw_secs(StopWatch::SECONDS);
int xstate = 0;
long next_time = 0;
long time_next1 = 0;
int pos = 0;
int payload = 0;
Servo myservo;


//..........................................................................................................................
//Setup

void setup() {
  Serial.begin(115200);
  myservo.attach(8);
  myservo.write(0);
  lcd.begin(16,2);
#if !defined(__MIPSEL__)
  while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif
  if (Usb.Init() == -1) {
    Serial.print(F("\r\nOSC did not start"));
    while (1); // Halt
  }
  Serial.print(F("\r\nPS4 Bluetooth Library Started"));
}

//..........................................................................................................................
//Loop

void loop() {
  Usb.Task();


//..........................................................................................................................
//Determine state for LCD

  if (PS4.connected()) {
  long current_time = millis();
  if (PS4.getAnalogHat(LeftHatX) > 137 || PS4.getAnalogHat(LeftHatX) < 117 || PS4.getAnalogHat(LeftHatY) > 137 || PS4.getAnalogHat(LeftHatY) < 117 || PS4.getAnalogHat(RightHatX) > 137 || PS4.getAnalogHat(RightHatX) < 117 || PS4.getAnalogHat(RightHatY) > 137 || PS4.getAnalogHat(RightHatY) < 117) {
     if (next_time < current_time)
     {
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

 
//..........................................................................................................................
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
      }
      
    if (xstate==2){
      lcd.setCursor(0,0);
      lcd.print("  Time Elapsed  ");
      lcd.setCursor(0,1);
      lcd.print(sw_secs.elapsed());
      }

        
//..........................................................................................................................
//Payload Delivery

  
  if ((PS4.getAnalogButton(R2))&&(current_time > time_next1)&&(pos==0)) {
    
    pos = pos+45;
    time_next1= millis() + 500;
    myservo.write (pos);
    payload = 1;

  }
  else
  {
     if ((PS4.getAnalogButton(R2))&&(current_time > time_next1)&&((pos==45)||(pos==105))) {
    pos = pos+60;
    myservo.write (pos);
    time_next1= millis() + 500;
    payload = payload + 1;

  } 
    else{
    if ((PS4.getAnalogButton(R2))&&(current_time > time_next1)&&(pos==160)) {
    pos = 0;
    myservo.write (pos);
    time_next1= millis() + 500;
    } 
  }
}
//..........................................................................................................................
//Timer

if ((PS4.getButtonClick(CROSS))&&(toggle==0)) {
    toggle = !toggle;
    sw_secs.start();
    }
if ((PS4.getButtonClick(CROSS))&&(toggle==1)) {
    toggle = !toggle;
    sw_secs.stop();
    }
    if ((PS4.getButtonClick(CROSS))&&(toggle==1)) {
    toggle = !toggle;
    sw_secs.stop();
    }
if ((PS4.getButtonClick(CIRCLE))) {
    toggle = !toggle;
    sw_secs.reset();
    }

//..........................................................................................................................
//Disconnect
    if (PS4.getButtonClick(PS)) {
      Serial.print(F("\r\nPS"));
      PS4.disconnect();
    }
  }  }

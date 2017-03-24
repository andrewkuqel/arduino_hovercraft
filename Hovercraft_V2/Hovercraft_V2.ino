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
long stop_time=0;

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
  
  //Open a serial channel
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
  
  //Alert Serial Monitor that the Conroller can be connected
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

  //Test to see if the conroller is connected
  if (PS4.connected()) {
    
    //Set a variable for the current time
    long current_time = millis();

    //Check if joystick has been moved and the correct amount of time has passed
    if (PS4.getAnalogHat(LeftHatY) > 137 || PS4.getAnalogHat(LeftHatY) < 117) {
      if (next_time < current_time){
        
        //If the condition is passed change the time for the next use and adjust the state of the LCD
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

    //Depending on the state output to LCD
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

    //If the  R2 button is pressed for the first time move the servo 45 degrees
    if ((PS4.getAnalogButton(R2))&&(current_time > time_next1)&&(pos==0)) {
      pos = pos+45;
      time_next1= millis() + 500;
      myservo.write (pos);
      payload = 1;
    }
    else{
      
      //If the  R2 button is pressed for the second or third time move the servo 60 degrees
      if ((PS4.getAnalogButton(R2))&&(current_time > time_next1)&&((pos==45)||(pos==105))) {
        pos = pos+60;
        myservo.write (pos);
        time_next1= millis() + 500;
        payload = payload + 1;
      } 
      else{
        
        //If the  R2 button is pressed for the forth time time move the servo back to 0 degrees
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

    // If the Cross is pressed toggle the state of the timer
    if (PS4.getButtonClick(CROSS)) {
      if (toggle==0){
        start_time = millis();
      }
      toggle = toggle + 1;
    }
    
    //If the toggle state is odd, have to timer count from the last value
    Serial.print(toggle);
    if (toggle%2==1) {
      elapsed = (millis()-start_time+stop_time);
    }

    //If the toggle state is even, have the timer stop and display the last value
    if (toggle%2==0) {
      start_time=millis();
      stop_time=elapsed;
    }
    
    //if the Circle is pressed reset the timer
    if ((PS4.getButtonClick(CIRCLE))) {
      toggle = 0;
      elapsed=0;
    }
    
    //If the Triangle is pressed reset the position
    if ((PS4.getButtonClick(TRIANGLE))) {
      pos=0;
      myservo.write (pos);
      payload=0;
    }
//..........................................................................................................................//
//..........................................................................................................................//
//Disconnect

    //If the PS button is pressed then disconect the conreoller and wait 1 sec befor resetting the arduino
    if (PS4.getButtonClick(PS)) {
      Serial.print(F("\r\nPS"));
      PS4.disconnect();
    }
  } 
}
//..........................................................................................................................//

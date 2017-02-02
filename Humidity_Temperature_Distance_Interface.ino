//Libraries to include
#include "LiquidCrystal.h"
#include <dht.h>

//key pins for sensors
#define trigPin 10
#define echoPin 11
#define DHT11_PIN 12

dht DHT;

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

//Define inputs for Joystick controller
int xPin = A1;
int yPin = A0;
int buttonPin = 7;

//Define initial values for Joystick variables
int xPosition = 0;
int yPosition = 0;
int buttonState = 0;
int x = 0;

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600); 
  
  //Define pinmode
  pinMode(xPin, INPUT);
  pinMode(yPin, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  //activate pull-up resistor on the push-button pin
  pinMode(buttonPin, INPUT_PULLUP); 
  
  //Define size for led matrix
  lcd.begin(16,2);
  
}

void loop() {
   
  //Read input from Joystick controller
  xPosition = analogRead(xPin);
  yPosition = analogRead(yPin);
  buttonState = digitalRead(buttonPin);
  delay(100);
  
  //Define which sensor to display based on current screen and controller input
  if ((x==0 && xPosition>600) || (x==2 && xPosition<400)){
    x=1;
  }
  else{
    if ((x==1 && xPosition<400) || (x==2 && xPosition>600)){
      x=0;
    }
    else{
      if ((x==1 && xPosition>600) || (x==0 && xPosition<400)){
        x=2;
      }
    }
  }
  
    //Check if Distance should be outputed
    if (x==1){
      
      //define distance variables
      int duration, distance, i;
      i = 0;
      
      //Measure distance
      digitalWrite(trigPin, HIGH);
      delayMicroseconds(1000);
      digitalWrite(trigPin, LOW);
      duration = pulseIn(echoPin, HIGH);
      
      //Calculate distance
      distance = (duration/5) / 29.1;
      
      //Output distance to the serial monitor
      Serial.println(distance);
      
      //Check if distance is short enough to be displayed
      if (distance < 16){
        
        //Output distance to LCD
        lcd.setCursor(0,0);
        lcd.print("DISTANCE         ");
        lcd.setCursor(0,1);
        lcd.print("                ");
        
        //Loop through the length of the distance to display a scale
        while(i < distance){
          lcd.setCursor(i,1);
          lcd.print(".");
          i = i+1;
        }  
      }
      
      //Tell user if no object is detected
      else{
        lcd.setCursor(0,0);
        lcd.print("   NO OBJECT");
        lcd.setCursor(0,1);
        lcd.print("                ");
      }
      
      //Give time for user to see the distance befor changing
      delay(500);
    }
    
    //Check if temperature should be displayed
    if(x==0){
      
      //Not really sure what this does
      int chk = DHT.read11(DHT11_PIN);
      
      //Print out the temperature to the serial monitor
      Serial.print("Temperature = ");
      Serial.println(DHT.temperature);
      
      //Print out the temperature to the LCD
      lcd.setCursor(0,0);
      lcd.print("TEMPERATURE     ");
      lcd.setCursor(0,1);
      lcd.print(DHT.temperature);
      
      //Give time for sensors to collect next reading
      delay(1000);
    }
    
    //Checks if humidity should be displayed
    if(x==2){
      
      //Again not sure what this does
      int chk = DHT.read11(DHT11_PIN);
      
      //Prints out the humidity to the serial monitor
      Serial.print("Humidity = ");
      Serial.println(DHT.humidity);
      
      //Prints out the humidity to the LCD
      lcd.setCursor(0,0);
      lcd.print("HUMIDITY     ");
      lcd.setCursor(0,1);
      lcd.print(DHT.humidity);
      
      //Gives time for the sensor to recieve more input
      delay(1000);
    }
}

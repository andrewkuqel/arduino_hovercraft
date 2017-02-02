#include "LiquidCrystal.h"

// initialize the library by providing the nuber of pins to it
LiquidCrystal lcd(8,9,4,5,6,7);
// VSS GROUND
// VDD 5V
// V0 MIDDLE POTENTIOMETER
// RS 8
// RW GROUND
// E 9
// D4 4
// D5 5
// D6 6
// D7 7
// A 5V
// R GROUND

void setup() {
lcd.begin(16,2);
}
void loop(){
  lcd.setCursor(0,0);
  lcd.print(" SKYNET LOADING");
  int i = 0;
  while(i < 16){
    lcd.setCursor(i,1);
    lcd.print(".");
    delay(500);
    i = i+1;
    }
  lcd.setCursor(0,1);
  lcd.print("                ");
}

/*
  This code writes from the serial in to an LCD.
  Created for A-Team S-Band system for COSGC
  Robert Belter 10/30/2015
 */

// include the library code:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(20, 4);
  // Print a message to the LCD.
  lcd.print("RF Beacon Debug Unit");
  delay(1000);
  Serial.begin(57600);
}

void loop() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 1);
  if(Serial.available()){
    lcd.clear();
    lcd.print("RF Beacon Debug Unit");
  }
  while(Serial.available()){
    lcd.write(Serial.read());
  }
}


//XBEE 2.4GHZ System For Delivering Location Relative Bearing in Degrees.
//Finalized by Jack Maydan based off Adam St. Amard's earlier versions.
//
//This program works based on the Spark Fun Arduino FIO v3.3 with an XBEE transmitter hooked to an extended antennae.
//The board also is hooked to a 3 axis magnetometer. 
//
//The entire module rotates, calculates the bearing based off magnetomer, 
//and transmits it through the patch antennae.
//
//This code is open source and free to use, its derivatives should follow the same guidelines.

#include <XBee.h>
#include <Wire.h>

XBee xbee = XBee();
int compassAddress = 0x42 >> 1;
int currentVector = 0;

void setup(){
  Wire.begin();
  Serial.begin(57600);
  Serial1.begin(57600);
  xbee.setSerial(Serial1);
}

void loop(){
  currentVector = getVector();
  uint8_t payload[] = {currentVector/2};

  //Address of receiving device can be anything while in broadcasting mode
  Tx16Request tx = Tx16Request(0x5678, payload, sizeof(payload));
  xbee.send(tx);
  
  //Delay must be longer than the readPacket timeout on the receiving module
  delay(10);
}


/*--------------------------------------------------------------
This the the fucntion which gathers the heading from the compass.
----------------------------------------------------------------*/
float getVector () {
  float reading = -1;
  int x, y, z; 
  
  // step 1: instruct sensor to read echoes 
  Wire.beginTransmission(compassAddress);  // transmit to device
  // the address specified in the datasheet is 66 (0x42) 
  // but i2c adressing uses the high 7 bits so it's 33 
  Wire.write('A');          // command sensor to measure angle  
  Wire.endTransmission();  // stop transmitting 

  // step 2: wait for readings to happen 
  delay(7);               // datasheet suggests at least 6000 microseconds 

  // step 3: request reading from sensor 
  Wire.requestFrom(compassAddress, 6);  // request 2 bytes from slave device #33 

  // step 4: receive reading from sensor 
  if(2 <= Wire.available())     // if two bytes were received 
  { 
   x = Wire.read()<<8; //X msb
    x |= Wire.read(); //X lsb
    z = Wire.read()<<8; //Z msb
    z |= Wire.read(); //Z lsb
    y = Wire.read()<<8; //Y msb
    y |= Wire.read(); //Y lsb
   
  } 
 float heading = atan2(y,x);
   if(heading < 0)
      heading += 2*PI;
reading = heading * 180/M_PI;
    Serial.print("Theta: ");
   Serial.println(reading); // print the heading/bearing
   return(reading);    // return the heading or bearing
}


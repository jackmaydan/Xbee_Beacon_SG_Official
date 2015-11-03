//XBEE 2.4GHZ Transmitter System For Delivering Location Relative Bearing in Degrees.
//Finalized by Jack Maydan based off Adam St. Amard's earlier versions.
//Edited by Robert Belter 10/30/2015

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
//#include <HMC5883L.h>
#define address 0x1E 

//#define calibration_mode

XBee xbee = XBee();
int compassAddress = 0x42 >> 1;

uint8_t payload[4] = {0,0,0,0};


union{
  float f;
  uint8_t b[4];
}heading_converter;

//Callibration values, can be ignored
#ifdef calibration_mode
int xmax, ymax, zmax = -1000;
int xmin, ymin, zmin = 1000;
#endif

//Axis offsets
int xoff = -27;
int yoff = 300;
int zoff = 200;

void setup(){
  Wire.begin();
  Serial.begin(57600);
  Serial1.begin(57600);
  xbee.setSerial(Serial1);
  Wire.beginTransmission(address); //open communication with HMC5883
  Wire.write(0x02); //select mode register
  Wire.write(0x00); //continuous measurement mode
  Wire.endTransmission();
}

void loop(){
  getVector();
  Serial.print("Theta: ");
  Serial.println(heading_converter.f, 2); // print the heading/bearing
  //Copy heading into payload
  memcpy(payload, heading_converter.b, 4);
  //Address of receiving device can be anything while in broadcasting mode
  Tx16Request tx = Tx16Request(0x5678, payload, sizeof(payload));
  xbee.send(tx);
  
  //Delay must be longer than the readPacket timeout on the receiving module
  delay(10);
}


/*--------------------------------------------------------------
This the the fucntion which gathers the heading from the compass.
----------------------------------------------------------------*/
void getVector () {
  float reading = -1;
  int x, y, z; 
  
  // step 1: instruct sensor to read echoes 
  Wire.beginTransmission(address);  // transmit to device
  // the address specified in the datasheet is 66 (0x42) 
  // but i2c adressing uses the high 7 bits so it's 33 
  Wire.write(0x03);          // command sensor to measure angle  
  Wire.endTransmission();  // stop transmitting 

  // step 2: wait for readings to happen 
  delay(7);               // datasheet suggests at least 6000 microseconds 

  // step 3: request reading from sensor 
  Wire.requestFrom(address, 6);  // request 2 bytes from slave device #33 

  // step 4: receive reading from sensor 
  if(2 <= Wire.available())     // if two bytes were received 
  { 
    x = Wire.read()<<8; //X msb
    x |= Wire.read(); //X lsb
    z = Wire.read()<<8; //Z msb
    z |= Wire.read(); //Z lsb
    y = Wire.read()<<8; //Y msb/
    y |= Wire.read(); //Y lsb
   
  } 
  //Adjust values by offsets
  x += xoff;
  y += yoff;
  z += zoff;
#ifdef calibration_mode
  if(x<xmin) xmin = x;
  if(x>xmax) xmax = x;
  if(y<ymin) ymin = y;
  if(y>ymax) ymax = y;
  if(z<zmin) zmin = z;
  if(z>zmax) zmax = z;
  char output[100];
  sprintf(output, "x: %d - %d, y: %d - %d, z: %d - %d", xmin, xmax, ymin, ymax, zmin, zmax);
  Serial.println(output);
#endif
  float heading = atan2(y,x);
  heading += PI/2;
  if(heading < 0)
    heading += 2*PI;
  if(heading > 2*PI)
    heading -= 2*PI;
  reading = heading * 180/PI;
  delay(50);
  heading_converter.f = reading;    // return the heading or bearing
}


#include <XBee.h>
#include <Wire.h>

XBee xbee = XBee();
int compassAddress = 0x42 >> 1;
int currentVector = 0;
int x;
int y;
int z;

void setup() {
  Wire.begin();
  Serial.begin(57600);
  Serial1.begin(57600);
  xbee.setSerial(Serial1);
}

void loop() {
  currentVector = getVector();
  uint8_t payload[] = {currentVector / 2};

  //Address of receiving device can be anything while in broadcasting mode
  Tx16Request tx = Tx16Request(0x5678, payload, sizeof(payload));
  xbee.send(tx);

  //Delay must be longer than the readPacket timeout on the receiving module
  delay(10);
}


/*--------------------------------------------------------------
This the the fucntion which gathers the heading from the compass.
----------------------------------------------------------------*/
int getVector () {
  int reading = 0;

  // step 1: instruct sensor to read echoes
  Wire.beginTransmission(compassAddress);  // transmit to device
  // the address specified in the datasheet is 66 (0x42)
  // but i2c adressing uses the high 7 bits so it's 33
  Wire.write('A');          // command sensor to measure angle  *
  Wire.endTransmission();  // stop transmitting

  // step 2: wait for readings to happen
  delay(7);               // datasheet suggests at least 6000 microseconds

  // step 3: request reading from sensor
  Wire.requestFrom(compassAddress, 2);  // request 2 bytes from slave device #33

  // step 4: receive reading from sensor
  if (2 <= Wire.available())    // if two bytes were received
  {
    x = Wire.read()<<8; //X msb
    x |= Wire.read(); //X lsb
    z = Wire.read()<<8; //Z msb
    z |= Wire.read(); //Z lsb
    y = Wire.read()<<8; //Y msb
    y |= Wire.read(); //Y lsb
    //Print out values of each axis
  Serial.print("x: ");
  Serial.print(x);
  Serial.print("  y: ");
  Serial.print(y);
  Serial.print("  z: ");
  Serial.println(z);
  
  delay(250);

    return (z);   // print the reading
  }
}


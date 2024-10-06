//=============================================================================
// Copyright (C) 2024  Emmanuel Mazurier <contact@bibliob.us>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//=============================================================================

#include <AltSoftSerial.h>
#include <FastLED.h>
AltSoftSerial softSerial; // for pin 8 = RX, pin 9 = TX

// Declare our NeoPixel strip object:
#define NUM_LEDS 32
#define LED_PIN  2
CRGB leds[NUM_LEDS];

// for Serial message parser
const byte buffSize = 40;
char inputBuffer[buffSize];
const char startMarker = '<';
const char endMarker = '>';
byte bytesRecvd = 0;
boolean readInProgress = false;
boolean newDataFromSerial = false;

int buttonState = 0;        // current state of resetbutton
int lastButtonState = 0;    // previous state of the button
int buttonPush = 0;         // counter

// message structure
int ledStart = 0;
int ledOffset = 0;
int red = -1;
int green = -1;
int blue = -1;

int pinReset = 12;
unsigned long curMillis;

//=============

void setup() {
  
  Serial.begin(9600);
  softSerial.begin(9600);

  // init Leds strip
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
  FastLED.clear(true);

  // declare the reset pin as an input
  pinMode(pinReset, INPUT);
  
  // tell the serial output we are ready
  Serial.println("SoftSerial is ready");
}

//=============

void loop() {

  // manage button state and leds reset if asked
  buttonState = digitalRead(pinReset);
  if (buttonState != lastButtonState) {
    if (buttonState == HIGH) {
      buttonPush++;
      if (buttonPush == 1) { //force all ledstatus off
        resetLeds();
        buttonPush = 0;
      }  
    }
  }
  lastButtonState = buttonState;
  
  curMillis = millis();
  
  // listen for new messages
  getDataFromSerial(); 
  
  // display leds position for given message
  if(newDataFromSerial) { 
    lightLEDsFromMessage(); 
    serialDebug();
  }
}

//============

void resetLeds() {
  // reinit leds
  FastLED.clear();
  FastLED.show();
}

//============

void lightLEDsFromMessage() {
    
    //FastLED.setBrightness(80);
    
    // reset message
    if(ledStart==-1) {
      resetLeds();
    }
    
    // light on leds coordinates and colors
    for (int i=ledStart; i<(ledStart + ledOffset); i++) { 
      if(red > 0 && green > 0 && blue > 0)
        leds[i] = CRGB(red,green,blue);
      else
        leds[i] = CRGB::Blue;
    }
    FastLED.show();
}

//=============

void getDataFromSerial() {

 // receive data from Serial and save it into inputBuffer
 if(softSerial.available() > 0) {

    char x = softSerial.read();

    // the order of these IF clauses is significant

    if (x == endMarker) {
      inputBuffer[bytesRecvd] = 0;
      parseData();
      readInProgress = false;
      newDataFromSerial = true;      
    }
    
    if(readInProgress) {
      inputBuffer[bytesRecvd] = x;
      bytesRecvd ++;
      if (bytesRecvd == buffSize) {
        bytesRecvd = buffSize - 1;
      }
    }

    if (x == startMarker) { 
      bytesRecvd = 0; 
      readInProgress = true;
    }
  }
    
}

//=============

void parseData() {

  // split the data into its parts
  char * strtokIndx; // this is used by strtok() as an index
  strtokIndx = strtok(inputBuffer,",");      // get the first part - X coord
  ledStart = atoi(strtokIndx);               // convert this part to an integer
  strtokIndx = strtok(NULL, ",");           // get X offset
  ledOffset = atoi(strtokIndx);               
  strtokIndx = strtok(NULL, ",");           // get red value 
  red = atoi(strtokIndx);
  strtokIndx = strtok(NULL, ",");           // get green value
  green = atoi(strtokIndx);   
  strtokIndx = strtok(NULL, ",");           // get blue value
  blue = atoi(strtokIndx); 
 
  delay(10);                                // must use delay to parse several messages

}

//=============

void serialDebug() {
  // for Serial output
  if (newDataFromSerial) {    
    newDataFromSerial = false;
    String position = "Position: " + String(ledStart) + ", Offset: " + String(ledOffset);
    Serial.print(position);
    String color = " red: " + String(red)+", green: " + String(green) + ", blue: " + String(blue);
    Serial.print(color);      
    Serial.print(" bytesRecvd ");
    Serial.print(bytesRecvd);
    Serial.print(" Time ");
    Serial.println(curMillis >> 9); // divide by 512 is approx = half-seconds
  }
}


// LPD8806 LED Code for testing IR sensor/LED strip integration for Chandelierium
/*************************************************************************************/
#include "LPD8806.h" // Library Here: https://github.com/adafruit/LPD8806
#include "SPI.h"

//Set up pin numbers for LED strip.
int totalLEDs = 12; // Length of the LED strip.
int dataPin = 2;    
int clockPin = 3; 
LPD8806 strip = LPD8806(totalLEDs, dataPin, clockPin);

//Set up pin numbers for IR sensor.
int sensorPin = A0;    

// SMOOTHING: Define the number of samples for smoothing the sensor signal. 
const int numReadings = 10;
int readings[numReadings];      // the readings from the analog input
int index = 0;                  // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average

// "Mood light" color cycling
int colorval = 0; // Variable to hold the changing "mood light" strip color.
const int mooddelay = 2; // Increase this to slow down the color cycling.
int moodcounter = 0; // just a counter...

// "Party mode" !!!
uint32_t RGBWheel(uint16_t WheelPos);
const int PartyModeDifficulty = 470; //Upper limit is 500. 

void setup() {
  strip.begin();  // Start up the LED strip
  strip.show();  // Update the strip, to start they are all 'off'
  
  //Initialize smoothing values.
   for (int thisReading = 0; thisReading < numReadings; thisReading++)
    readings[thisReading] = 0;          
}


void loop() {
    // read the value from the sensor and compress ends of scale:
  int dist = analogRead(sensorPin);    
  if(dist>500) dist=500;
  if(dist<200) dist=200;
  
    //Smooth the value over time! subtract the last reading:
  total= total - readings[index];         
  // put sensor reading into array:  
  readings[index] = dist; 
  // add the reading to the total:
  total= total + readings[index];       
  // advance to the next position in the array:  
  index = index + 1;                    
  // if we're at the end of the array...
  if (index >= numReadings)              
    // ...wrap around to the beginning: 
    index = 0;                           
  // calculate the average:
  average = total / numReadings;         

  // Rescale the signal to the length of the LED strip.
  dist = map(average, 200, 500, 0, totalLEDs); 
  
//Light up the strip in "normal mode":  
colorFill(Wheel(colorval), 10, dist);      // blue fill, distance determined by IR reading. 

//Activate "party mode" if you've earned it:
if(average>PartyModeDifficulty) rainbowCycle(0);

//Mood light cycling with variable delay.
if(moodcounter < mooddelay) moodcounter++;
else 
  {
    colorval++;
    moodcounter = 0;
  }
if(colorval>255) colorval = 0; //Loop back to the beginning of the color wheel.

// Clear strip data before start of next cycle
  for (int i=0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, 0);  
  }
}


// Fill the strip with color c, up to a point determined by the IR sensor value.
void colorFill(uint32_t c, uint8_t wait, uint8_t distance) {
  int i;
 
  for (i=0; i <= distance; i++) {
      strip.setPixelColor(i, c); // set all active pixels on
  }
  strip.show(); // display all the pixels!
  delay(wait);
 }
 
 // Cycle through the color wheel: PARTY MODE!
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;
  for (j=0; j < 384 * 5; j++) {     // 5 cycles of all 384 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
      // tricky math! we use each pixel as a fraction of the full 384-color
      // wheel (thats the i / strip.numPixels() part)
      // Then add in j which makes the colors go around per pixel
      // the % 384 is to make the wheel cycle around
      strip.setPixelColor(i, RGBWheel(((i * 384 / strip.numPixels()) + j) % 384));
    }
    strip.show();   // write all the pixels out
    delay(wait);
  }
}


/* Blue-green Color wheel helper function */
//Input a value 0 to 256 to get a color value.
uint32_t Wheel(uint16_t WheelPos) {
  byte r, g, b;
  switch(WheelPos / 128)
  {
    case 0:
      b = 127 - WheelPos % 128; // blue down
      g = WheelPos % 128;       // green up
      r = 0;                    // red off
      break;
    case 1:
      g = 127 - WheelPos % 128; // green down
      b = WheelPos % 128;       // blue up
      r = 0;                    // red off
      break;
   }
  return(strip.Color(r,g,b));    // No red here!
}

/* RGB Color Wheel Helper function */
//Input a value 0 to 384 to get a color value.
uint32_t RGBWheel(uint16_t WheelPos) {
  byte r, g, b;
  switch(WheelPos / 128)
  {
    case 0:
      r = 127 - WheelPos % 128; // red down
      g = WheelPos % 128;       // green up
      b = 0;                    // blue off
      break;
    case 1:
      g = 127 - WheelPos % 128; // green down
      b = WheelPos % 128;       // blue up
      r = 0;                    // red off
      break;
    case 2:
      b = 127 - WheelPos % 128; // blue down
      r = WheelPos % 128;       // red up
      g = 0;                    // green off
      break;
  }
  return(strip.Color(r,g,b));    
}

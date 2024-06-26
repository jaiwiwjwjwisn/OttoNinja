/****************************************
Scrolling Sound Meter Sketch for the 
Adafruit Microphone Amplifier
****************************************/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_LEDBackpack.h>

// Include the Matrix code for display
Adafruit_8x16matrix matrix = Adafruit_8x16matrix();

const int maxScale = 16;
const int redZone = 5;
const int sampleWindow = 50; // Sample window width in mS (50 mS = 20Hz)

void setup() 
{
   Serial.begin(9600);
   matrix.begin(0x70);  // pass in the address
}

void loop() 
{
   unsigned long startMillis= millis();  // Start of sample window
   unsigned int peakToPeak = 0;   // peak-to-peak level

   unsigned int signalMax = 0;
   unsigned int signalMin = 1024;

   while (millis() - startMillis < sampleWindow)
   {
      int sample = analogRead(0); 
      if (sample < 1024 && sample > 0)  // toss out spurious readings
      {
         if (sample > signalMax)
         {
            signalMax = sample;  // save just the max levels
         }
         else if (sample < signalMin)
         {
            signalMin = sample;  // save just the min levels
         }
      }
   }
   peakToPeak = signalMax - signalMin;
   double volts = (peakToPeak * 5) / 1024;  // convert to volts

   Serial.println(volts);
   // map 1v p-p level to the max scale of the display
   int displayPeak = map(peakToPeak, 0, 1023, 0, maxScale);
   if (displayPeak < 0) displayPeak = 0; // make sure the value is not negative

   // Shift the display left
   memmove(matrix.displaybuffer, matrix.displaybuffer + 1, 7 * sizeof(uint16_t));

   // draw the new sample
   for (int i = 0; i <= maxScale; i++)
   {
      if (i == maxScale)  // blank this pixel
      {
         matrix.drawPixel(7, i, 0);
      }
      else if (i < redZone) // draw in green
      {
         matrix.drawPixel(7, i, LED_ON);
      }
      else // Red Alert!  Red Alert!
      {
         matrix.drawPixel(7, i, LED_ON);
      }
   }
   matrix.writeDisplay();  // write the changes we just made to the display

   // Add a delay to reduce CPU usage and make the output more stable
   delay(10);
}

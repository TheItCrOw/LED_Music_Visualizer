#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

/*+
 *colorWipe(strip.Color(255,   0,   0), 50); // Red
  colorWipe(strip.Color(  0, 255,   0), 50); // Green
  colorWipe(strip.Color(  0,   0, 255), 50); // Blue 
  theaterChase(strip.Color(127, 127, 127), 50); // White, half brightness
 * 
 * 
 */

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define STRIP_PIN 6
#define STRING_PIN 5

// Buttons
#define BUTTON_ANIMATION 12
#define BUTTON_COLOR_BLUE 9
#define BUTTON_COLOR_RED 10
#define BUTTON_COLOR_GREEN 8
#define BUTTON_COLOR_PURPLE 11

// How many NeoPixels are attached to the Arduino?
#define STRIP_COUNT 95 // small is 29 
#define STRING_COUNT 95 // small is 29

int minFrequence = 45; // Min input frequence it needs to actually enlighten an LED
int maxFrequence = 2000; // Max frequence we want to visualise.
int lastIndex = 0; // This contains on what LED we stopped on the last loop round
uint32_t currentColor1 = 0;
uint32_t currentColor2 = 0;
uint32_t currentColor3 = 0;
int currentAnimation = 0; // 0 = sync to music, 1 = rainbow

Adafruit_NeoPixel strip(STRIP_COUNT, STRIP_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel string(STRING_COUNT, STRING_PIN, NEO_GRB + NEO_KHZ800);

void setup()                                                                                                                                                                                                                                                                                                                                                                                                                                     
{
  Serial.begin(9600);
  Serial.println("Start");

  // Init the first color
  currentColor1 = strip.Color(0, 0, 255); // deep blue
  currentColor2 = strip.Color(0, 187, 255); // light blue
  currentColor3 = strip.Color(0, 255, 234); // turkis

  // Init the string
  string.begin();
  string.show();
  string.setBrightness(50);
  // Init the LED Strip
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(150); // Set BRIGHTNESS to about 1/5 (max = 255)

  // Init the audio input
  pinMode(STRIP_PIN, OUTPUT);
  pinMode(STRING_PIN, OUTPUT);
  
  // Init the buttons
  pinMode(BUTTON_COLOR_BLUE, INPUT);
  pinMode(BUTTON_COLOR_RED, INPUT);
  pinMode(BUTTON_COLOR_GREEN, INPUT);
  pinMode(BUTTON_COLOR_PURPLE, INPUT);
  pinMode(BUTTON_ANIMATION, INPUT);
  
  digitalWrite(BUTTON_COLOR_BLUE, HIGH);
  digitalWrite(BUTTON_COLOR_RED, HIGH);
  digitalWrite(BUTTON_COLOR_GREEN, HIGH);
  digitalWrite(BUTTON_COLOR_PURPLE, HIGH);
  digitalWrite(BUTTON_ANIMATION, HIGH);

  // Perform a small startup animation
  startupAnimation();
}

//this function will make the LED dim once the Parallax Sound Impact Sensor sends a 1 signal, and then return to it’s original brightness.
void loop()
{
  /*Serial.print("A0: ");
  Serial.print(analogRead(A0));
  Serial.print(" | A1: ");
  Serial.print(analogRead(A1));
  int sensorValue = analogRead(A0) + analogRead(A1);
  float voltage= sensorValue * (5.0 / 1023.0);
  Serial.print(" | voltage: ");
  Serial.print(voltage);
  Serial.println("");*/
  
  changeAnimation();
  changeColor();
  
  // Change the mood of the current animation
  if(currentAnimation == 0)
  {   
    updateLEDs(analogRead(A0) + analogRead(A1));
  }
  else if(currentAnimation == 1)
  {
    rainbow();
  }

  delay(0);
}

// Handle the chaning of animation by a button press
void changeAnimation()
{
  int currentButtonState = digitalRead(BUTTON_ANIMATION);
  
  // Switch the state of the button
  if(currentButtonState == LOW)
  {
    if(currentAnimation == HIGH)
    {
      currentAnimation = 0;
      Serial.println("Syning to music");
      strip.clear();
      string.clear();
    }
    else
    {
      currentAnimation = 1;
      Serial.println("Rainbow");
      strip.clear();
      string.clear();
    }
    delay(500);
  } 
}

// Handle the button events to change the color accordingly
void changeColor()
{
  int buttonStateBlue = digitalRead(BUTTON_COLOR_BLUE);
  if(buttonStateBlue == LOW)
  {
    currentColor1 = strip.Color(0, 0, 255); // deep blue
    currentColor2 = strip.Color(0, 187, 255); // light blue
    currentColor3 = strip.Color(0, 255, 234); // turkis
    Serial.println("Color now blue");
    delay(200);
    return;
  }

  int buttonStateRed = digitalRead(BUTTON_COLOR_RED);
  if(buttonStateRed == LOW)
  {
    currentColor1 = strip.Color(255, 0, 0); // deep red
    currentColor2 = strip.Color(255, 162, 0); // orange
    currentColor3 = strip.Color(255, 255, 0); // yellow
    Serial.println("Color now red");
    delay(200);
    return;
  }

  int buttonStateGreen = digitalRead(BUTTON_COLOR_GREEN);
  if(buttonStateGreen == LOW)
  {
    currentColor1 = strip.Color(3, 252, 169); // blueish green
    currentColor2 = strip.Color(3, 252, 82); // green
    currentColor3 = strip.Color(132, 252, 3); // lime
    Serial.println("Color now green");
    delay(200);
    return;
  }

  int buttonStatePurple = digitalRead(BUTTON_COLOR_PURPLE);
  if(buttonStatePurple == LOW)
  {
    currentColor1 = strip.Color(150, 0, 255); // dark lila
    currentColor2 = strip.Color(208, 0, 255); // light lila
    currentColor3 = strip.Color(255, 0, 238); // pink
    Serial.println("Color now pruple");
    delay(200);
    return;
  }
}

// Update the LEDS according to the given frequence
void updateLEDs(int index)
{    
  // This is the sweet spot, when the video is usually pasued.
  float voltage= index * (5.0 / 1023.0);
  if(voltage > 0.44f && voltage < 0.45f)
  {
    strip.clear();
    string.clear();
    strip.show();
    string.show();

    delay(0);
    return;
  }

  if(index > minFrequence && index < maxFrequence)
  {
      for(int i = 0; i < index / 2; i++)
      {
        setLEDColorByIndex(i);
        delay(0);
      }
      for(int i = index / 2; i < string.numPixels(); i++)
      {
         strip.setPixelColor(i, strip.Color(0, 0, 0));
         string.setPixelColor(i, strip.Color(0, 0, 0));
         delay(0);
      }
      lastIndex = index / 2;
  }
  else
  {
    lastIndex--;
    strip.setPixelColor(lastIndex, strip.Color(0, 0, 0));
    string.setPixelColor(lastIndex, strip.Color(0, 0, 0));
  }

  strip.show();
  string.show();
}

// Sets the currentColor according to the current index
void setLEDColorByIndex(int i)
{
   if(i < strip.numPixels() / 3)
   {
     strip.setPixelColor(i, currentColor1);
     string.setPixelColor(i, currentColor1);
   }
   else if(i >= strip.numPixels() / 3 && i <= strip.numPixels() / 2)
   {
     strip.setPixelColor(i, currentColor2);
     string.setPixelColor(i, currentColor2);
   }
   else if(i > strip.numPixels() / 2)
   {
     strip.setPixelColor(i, currentColor3);
     string.setPixelColor(i, currentColor3);
   }
}

// Makes the led play a rainbow
void rainbow()
{
  for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) 
  {
    // the rainbow takes a while. We have to check if the user wants to chagen animation in the ranbow loop.
    changeAnimation();

    if(currentAnimation == 0)
    {
          return;
    }
    
    for(int i=0; i<strip.numPixels(); i++) 
    {
      int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
      string.setPixelColor(i, string.gamma32(string.ColorHSV(pixelHue)));
    }
    strip.show(); // Update strip with new contents
    string.show();
    delay(10);  // Pause for a moment
  }
}

void startupAnimation()
{
  for(int i = 0; i < string.numPixels(); i++)
  {
    string.setPixelColor(i, string.Color(255,255,255));
    strip.setPixelColor(i, string.Color(255,255,255));

    string.show();
    strip.show();
    delay(15);
  }
}

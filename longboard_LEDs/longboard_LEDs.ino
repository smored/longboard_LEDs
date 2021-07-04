#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define LED_PIN    0

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 120

// Wait time in milliseconds
#define WAIT_TIME 25
#define DEBOUNCE_TIME 20

#define BRAKE_BEGIN 50
#define BRAKE_END 70
#define HEADLIGHT_BEGIN 20
#define HEADLIGHT_END 40

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)


// setup() function -- runs once at startup --------------------------------

void setup() {
  attachInterrupt(digitalPinToInterrupt(10), brakeLightsHigh, RISING);
  attachInterrupt(digitalPinToInterrupt(10), brakeLightsLow, FALLING);
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(255); // Set BRIGHTNESS to about 1/5 (max = 255)
  headLights(0.1f);
  brakeLights(0.1f);
}


// loop() function -- runs repeatedly as long as board is on ---------------

void loop() {
  underglowTracer(0.1f);
}


// Some functions of our own for creating animated effects -----------------

// Fill strip pixels one after another with a color. Strip is NOT cleared
// first; anything there will be covered pixel by pixel. Pass in color
// (as a single 'packed' 32-bit value, which you can get by calling
// strip.Color(red, green, blue) as shown in the loop() function above),
// and a delay time (in milliseconds) between pixels.
void colorWipe(uint32_t color, int wait) {
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}

// Theater-marquee-style chasing lights. Pass in a color (32-bit value,
// a la strip.Color(r,g,b) as mentioned above), and a delay time (in ms)
// between frames.
void theaterChase(uint32_t color, int wait) {
  for(int a=0; a<10; a++) {  // Repeat 10 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in steps of 3...
      for(int c=b; c<strip.numPixels(); c += 3) {
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip.show(); // Update strip with new contents
      delay(wait);  // Pause for a moment
    }
  }
}

// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow(int wait) {
  // Hue of first pixel runs 5 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
  // means we'll make 5*65536/256 = 1280 passes through this outer loop:
  for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
    for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
      // Offset pixel hue by an amount to make one full revolution of the
      // color wheel (range of 65536) along the length of the strip
      // (strip.numPixels() steps):
      int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
      // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      // optionally add saturation and value (brightness) (each 0 to 255).
      // Here we're using just the single-argument hue variant. The result
      // is passed through strip.gamma32() to provide 'truer' colors
      // before assigning to each pixel:
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
    }
    strip.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}

// Rainbow-enhanced theater marquee. Pass delay time (in ms) between frames.
void theaterChaseRainbow(int wait) {
  int firstPixelHue = 0;     // First pixel starts at red (hue 0)
  for(int a=0; a<30; a++) {  // Repeat 30 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in increments of 3...
      for(int c=b; c<strip.numPixels(); c += 3) {
        // hue of pixel 'c' is offset by an amount to make one full
        // revolution of the color wheel (range 65536) along the length
        // of the strip (strip.numPixels() steps):
        int      hue   = firstPixelHue + c * 65536L / strip.numPixels();
        uint32_t color = strip.gamma32(strip.ColorHSV(hue)); // hue -> RGB
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip.show();                // Update strip with new contents
      delay(wait);                 // Pause for a moment
      firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
    }
  }
}

// Fairly certain "brightness" is a keyword from the neopixel library
void brakeLights(float brightness) {
  for(int i=BRAKE_BEGIN; i<BRAKE_END; i++) {
    strip.setPixelColor(i, strip.Color(255*brightness, 0, 0));
  }
  strip.show();
}

// Debounce will verify input signal incase we decide to use physical buttons - TODO: Test this function
//bool debounce(int pin, bool state) {
//  delay(DEBOUNCE_DELAY);
//  return(digitalRead(pin)==state);
//}

void brakeLightsHigh() {
  //if (!debounce(10, 1)) return();
  brakeLights(1.0f);  
}

void brakeLightsLow() {
  //if (!debounce(10, 0)) return();
  brakeLights(0.1f);
}

void headLights(float brightness) {
  for (int i=HEADLIGHT_BEGIN; i<HEADLIGHT_END; i++) {
    strip.setPixelColor(i, strip.Color(255*brightness, 255*brightness, 255*brightness));
  }
  strip.show();
}

void underglowTracer(float brightness) {
  static int colour1Target = 80;
  static int colour2Target = 80+1;

  clearUnderglowLights();
  
  for (int i=0; i>8; i++) {
    float brightnessAdjust = i/8.0f;
  
    if (isUnderglowOutOfPersistentLights(colour1Target+i))
      strip.setPixelColor((colour1Target+i)%LED_COUNT, strip.Color(0,0,255*brightness*brightnessAdjust));

    if (isUnderglowOutOfPersistentLights(colour2Target-i))
      strip.setPixelColor((colour2Target-i)%LED_COUNT, strip.Color(0,0,255*brightness*brightnessAdjust));
  }
  colour1Target--;
  colour2Target++;

  if (colour1Target < 0)
    colour1Target += LED_COUNT;

  if (colour2Target < 0)
    colour2Target += LED_COUNT;

  if (colour1Target%LED_COUNT==20)
    colour1Target = 80;

  if (colour2Target%LED_COUNT==20)
    colour2Target = 80+1;
    
  strip.show();
  delay(WAIT_TIME);
}

//does not strip.show() so there's no flicker on effects, does not affect brake lights or headlights
void clearUnderglowLights() {
  for (int i=0; i<LED_COUNT; i++) {
    if ((i < BRAKE_BEGIN && i > BRAKE_END) || (i < HEADLIGHT_BEGIN && i > HEADLIGHT_END))
    strip.setPixelColor(i, 0, 0, 0);
  }
}

// Returns true or false depending on if we are allowed to write to the LED -
bool isUnderglowOutOfPersistentLights(int led) {
  return (led < BRAKE_BEGIN && led > BRAKE_END) || (led < HEADLIGHT_BEGIN && led > HEADLIGHT_END);
}

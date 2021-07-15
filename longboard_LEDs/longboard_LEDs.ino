// I am WOKE
#include <Adafruit_NeoPixel.h>
#include <ButtonDebounce.h>
#include <elapsedMillis.h>
#include "Accelerometer.h"
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define LED_PIN 23

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 120

// Wait time in milliseconds
#define WAIT_TIME 25
#define DEBOUNCE_TIME 20

#define BRAKE_BEGIN 44
#define BRAKE_END 55
#define HEADLIGHT_BEGIN 114
#define HEADLIGHT_END 120

#define INTERRUPT_PIN 15 // brake lights freak out if this is on 14

#define UNDERGLOW_START 109
#define UNDERGLOW_TRAIL_LENGTH 16

#define HIGHVAL 1.0f
#define LOWVAL 0.1f

#define ACC_THRESHOLD 0.25d // how much to digital low pass to use
#define ACC_G 16383.0d // The number that represents 1g of acceleration
#define GRAVITY 9.81d // How many m/s^2 in 1g

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

ButtonDebounce button(INTERRUPT_PIN, 25);
Acc3D Acc3D_Board1;
elapsedMillis ledOffTime;
unsigned int ledWaitTime;

void setup() {
  accInit();
  accIntInit();
  interrupts();
  pinMode(INTERRUPT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), brakeLightsHigh, FALLING);
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(255); // Set BRIGHTNESS max lmao
  headLights(HIGHVAL);
  brakeLights(LOWVAL);
}


// loop() function -- runs repeatedly as long as board is on ---------------

void loop() {
// static uint16_t hue;
// strip.fill(strip.gamma32(strip.ColorHSV(hue, 255, 140))); 
// strip.show();
 

  if (ledOffTime - ledWaitTime >= 0) {
    ledOffTime = 0;
    underglowTracer(1.0f);
  }
}

int approach() {
  // increase or decrease hue based on distance from previous value to current value
  static int previous;
  int difference;
  Acc3D_Board1 = accUpdate();
  difference = (Acc3D_Board1.AcY - previous);
  previous =  Acc3D_Board1.AcY;

  if (difference != 0) return (difference);
  return -1;
}

double integrateAcc() {
  static int elapsed = millis();
  static double velocity = 0;
  static double oldAcc = 0;
  double acc;
  
  while (!accReadIntStatus()); // Wait until data is ready
  Acc3D_Board1 = accUpdate(); // Update object
  acc = ((Acc3D_Board1.AcY)/ACC_G)*GRAVITY; // Read forward acceleration, convert to m/s^2
  
  Serial.println(acc);
  
  elapsed = millis() - elapsed; // Calculate how long that took

  // Now we calculate the area (integrate)
  // Trapezoidal approximation [A = (a+b)*h/2]
  velocity += ((acc+oldAcc)/2.0d)*((double)elapsed)/1000.0d;
  oldAcc = acc;
  Serial.println(velocity);

  return velocity;
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


/* brakeLights -------------------------------------------------------
 *  This function can be manually called but is automatically called 
 *  by the respective high and low brakeLight functions.
 *  It takes a float ranging from 100% to 0% for brightness
 * -----------------------------------------------------------------*/
void brakeLights(float brightness) { 
  for(int i = BRAKE_BEGIN; i < BRAKE_END; i++) {
    strip.setPixelColor(i, strip.Color(255*brightness, 0, 0));
  }
  strip.show();
}

/* brakeLightsHigh ---------------------------------------------------
 *  This function is called by the ISR and checks to verify the signal, 
 *  then calls the brakeLights function
 * -----------------------------------------------------------------*/
void brakeLightsHigh() {
  button.update();
  if (button.state() == LOW){ // Checks button debounce
    noInterrupts(); // Turns off interrupts while lights are being changed
    brakeLights(HIGHVAL);
  } else {
    return;
  }

  // Check if the signal went high during the noInterrupt period
  if (digitalRead(INTERRUPT_PIN)) {
    brakeLights(LOWVAL); // Edge was missed, so manually simulate it
    quickInterrupt(0); 
    return;
  }
  
  quickInterrupt(1); // Set interrupt to check for a rising edge
}

/* brakeLightsLow ---------------------------------------------------
 *  This function is called by the ISR and checks to verify the signal, 
 *  then calls the brakeLights function
 * -----------------------------------------------------------------*/
void brakeLightsLow() {
  button.update();
  if (button.state() == HIGH){
    noInterrupts();
    brakeLights(LOWVAL);
  } else {
    return;
  }

    // Check if the signal went high during the noInterrupt period
  if (!digitalRead(INTERRUPT_PIN)) {
    brakeLights(HIGHVAL); // Edge was missed, so manually simulate it
    quickInterrupt(1); 
    return;
  }
  
  quickInterrupt(0); // Set interrupt to check for a falling edge
}

/* quickInterrupt ----------------------------------------------------
 *  This function is just to make writing interrupts faster and cleaner
 * -----------------------------------------------------------------*/
void quickInterrupt(bool rising) {
  if (rising) {
    attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), brakeLightsLow, RISING);
  } else {
    attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), brakeLightsHigh, FALLING);
  }
  interrupts();
}

/* headLights --------------------------------------------------------
 *  Same as brakeLights() but white
 * -----------------------------------------------------------------*/
void headLights(float brightness) {
  for (int i = HEADLIGHT_BEGIN; i < HEADLIGHT_END; i++) {
    strip.setPixelColor(i, strip.Color(255*brightness, 255*brightness, 255*brightness));
  }
  strip.show();
}

/* underglowTracer ---------------------------------------------------
 *  ......
 * -----------------------------------------------------------------*/
void underglowTracer(float brightness) {
  static int colour1Target = UNDERGLOW_START+UNDERGLOW_TRAIL_LENGTH;
  static int colour2Target = UNDERGLOW_START-UNDERGLOW_TRAIL_LENGTH;
  static uint16_t glowHue = 0; // hue represented by a 16 bit uint so that it will gracefully overflow into the same colour
  glowHue += 1000; // this line is just for demonstration, replace it with whatever we're using to control the hue

  clearUnderglowLights();
  
  for (int i=0; i < UNDERGLOW_TRAIL_LENGTH; i++) {
    float brightnessAdjust = i / (float)UNDERGLOW_TRAIL_LENGTH;
  
    if (!isLightInPersistentLight(colour1Target-i) && colour1Target-i >= UNDERGLOW_START-LED_COUNT/2 && colour1Target-i <= UNDERGLOW_START)
      strip.setPixelColor((colour1Target-i)%LED_COUNT, 
        strip.ColorHSV(glowHue, 255, 255*brightness*brightnessAdjust));

    if (!isLightInPersistentLight(colour2Target+i) && colour2Target+i - LED_COUNT <= UNDERGLOW_START-LED_COUNT/2 && colour2Target+i >= UNDERGLOW_START)
      strip.setPixelColor((colour2Target+i)%LED_COUNT, 
        strip.ColorHSV(glowHue, 255, 255*brightness*brightnessAdjust));
  }
  colour1Target--;
  colour2Target++;

  if (colour1Target < 0)
    colour1Target += LED_COUNT;

  if (colour2Target < 0)
    colour2Target += LED_COUNT;

  if (colour1Target < UNDERGLOW_START-LED_COUNT/2)
    colour1Target = UNDERGLOW_START+UNDERGLOW_TRAIL_LENGTH;

  if (colour2Target - LED_COUNT > UNDERGLOW_START-LED_COUNT/2)
    colour2Target = UNDERGLOW_START-UNDERGLOW_TRAIL_LENGTH;
    
  strip.show();
  delay(WAIT_TIME);
}

/* clearUnderglowLights ----------------------------------------------
 *  does not strip.show() so there's no flicker on effects, does 
 *  not affect brake lights or headlights
 * -----------------------------------------------------------------*/
void clearUnderglowLights() {
  for (int i=0; i < LED_COUNT; i++) {
    if (!isLightInPersistentLight(i))
      strip.setPixelColor(i, 0, 0, 0);
  }
}

/* isLightInPersistentLight ------------------------------------------
 *  Returns true or false depending on if we are allowed to write to the LED
 * -----------------------------------------------------------------*/
bool isLightInPersistentLight(int led) {
  led %= LED_COUNT;
  return (led >= BRAKE_BEGIN && led < BRAKE_END) || (led >= HEADLIGHT_BEGIN && led < HEADLIGHT_END);
}

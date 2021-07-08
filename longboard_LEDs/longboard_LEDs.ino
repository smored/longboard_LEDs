#include <Adafruit_NeoPixel.h>
#include <ButtonDebounce.h>
#include "Accelerometer.h"
#include "Effects.h"
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

#define BRAKE_BEGIN 25
#define BRAKE_END 45
#define HEADLIGHT_BEGIN 93
#define HEADLIGHT_END 113

#define INTERRUPT_PIN 4

#define UNDERGLOW_START 95
#define UNDERGLOW_TRAIL_LENGTH 16

#define HIGHVAL 1.0f
#define LOWVAL 0.1f

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

void setup() {
<<<<<<< HEAD
  iicInit();
 // intInit();
<<<<<<< Updated upstream
=======
=======
>>>>>>> parent of 114f1ae (Merge branch 'main' of https://github.com/smored/longboard_LEDs)
>>>>>>> Stashed changes
  interrupts();
  pinMode(INTERRUPT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), brakeLightsHigh, FALLING);
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(255); // Set BRIGHTNESS max lmao
  headLights(LOWVAL);
  brakeLights(LOWVAL);
}


// loop() function -- runs repeatedly as long as board is on ---------------

void loop() {
  underglowTracer(0.1f);
<<<<<<< Updated upstream
//  Acc3D_Board1 = iicUpdate();
//  Serial.println(Acc3D_Board1.AccVectorSum);
=======
<<<<<<< HEAD
//  Acc3D_Board1 = iicUpdate();
//  Serial.println(Acc3D_Board1.AccVectorSum);
=======
>>>>>>> parent of 114f1ae (Merge branch 'main' of https://github.com/smored/longboard_LEDs)
>>>>>>> Stashed changes
}


// Some functions of our own for creating animated effects -----------------


<<<<<<< Updated upstream
/* brakeLights -------------------------------------------------------
 *  This function can be manually called but is automatically called 
 *  by the respective high and low brakeLight functions.
 *  It takes a float ranging from 100% to 0% for brightness
 * -----------------------------------------------------------------*/
=======
>>>>>>> Stashed changes
void brakeLights(float brightness) { 
  for(int i=BRAKE_BEGIN; i<BRAKE_END; i++) {
    strip.setPixelColor(i, strip.Color(255*brightness, 0, 0));
  }
  strip.show();
}

void brakeLightsHigh() {
  button.update();
  if (button.state() == LOW){
    noInterrupts();
    brakeLights(HIGHVAL);
  } else {
    return;
  }
<<<<<<< HEAD

  // Check if the signal went high during the noInterrupt period
  if (digitalRead(INTERRUPT_PIN)) {
    brakeLights(LOWVAL); // Edge was missed, so manually simulate it
    quickInterrupt(0); 
    return;
  }

  quickInterrupt(1); // Set interrupt to check for a rising edge
=======
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), brakeLightsLow, RISING);
  interrupts();
>>>>>>> parent of 114f1ae (Merge branch 'main' of https://github.com/smored/longboard_LEDs)
}

void brakeLightsLow() {
  button.update();
  if (button.state() == HIGH){
    noInterrupts();
    brakeLights(LOWVAL);
  } else {
    return;
  }
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), brakeLightsHigh, FALLING);
  interrupts();
}

void headLights(float brightness) {
  for (int i=HEADLIGHT_BEGIN; i<HEADLIGHT_END; i++) {
    strip.setPixelColor(i, strip.Color(255*brightness, 255*brightness, 255*brightness));
  }
  strip.show();
}

void underglowTracer(float brightness) {
  static int colour1Target = UNDERGLOW_START+UNDERGLOW_TRAIL_LENGTH;
  static int colour2Target = UNDERGLOW_START-UNDERGLOW_TRAIL_LENGTH;

  clearUnderglowLights();
  
  for (int i=0; i<UNDERGLOW_TRAIL_LENGTH; i++) {
    float brightnessAdjust = i/(float)UNDERGLOW_TRAIL_LENGTH;
  
    if (!isLightInPersistentLight(colour1Target-i) && colour1Target-i%LED_COUNT > UNDERGLOW_START-LED_COUNT/2-1 && colour1Target-i%LED_COUNT < UNDERGLOW_START+1)
      strip.setPixelColor((colour1Target-i)%LED_COUNT, strip.Color(0,0,255*brightness*brightnessAdjust));

    if (!isLightInPersistentLight(colour2Target+i) && colour2Target+i-LED_COUNT < UNDERGLOW_START-LED_COUNT/2+1 && colour2Target+i>UNDERGLOW_START-1)
      strip.setPixelColor((colour2Target+i)%LED_COUNT, strip.Color(0,0,255*brightness*brightnessAdjust));
  }
  colour1Target--;
  colour2Target++;

  if (colour1Target < 0)
    colour1Target += LED_COUNT;

  if (colour2Target < 0)
    colour2Target += LED_COUNT;

  if (colour1Target%LED_COUNT==UNDERGLOW_START-LED_COUNT/2-1)
    colour1Target = UNDERGLOW_START+UNDERGLOW_TRAIL_LENGTH;

  if (colour2Target%LED_COUNT==UNDERGLOW_START-LED_COUNT/2+1)
    colour2Target = UNDERGLOW_START-UNDERGLOW_TRAIL_LENGTH;
    
  strip.show();
  delay(WAIT_TIME);
}

//does not strip.show() so there's no flicker on effects, does not affect brake lights or headlights
void clearUnderglowLights() {
  for (int i=0; i<LED_COUNT; i++) {
    if (!isLightInPersistentLight(i))
      strip.setPixelColor(i, 0, 0, 0);
  }
}

// Returns true or false depending on if we are allowed to write to the LED -
bool isLightInPersistentLight(int led) {
  led %= LED_COUNT;
  return (led >= BRAKE_BEGIN && led < BRAKE_END) || (led >= HEADLIGHT_BEGIN && led < HEADLIGHT_END);
}

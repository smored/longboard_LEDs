#ifndef LED_EFFECTS_H
#define LED_EFFECTS_H

void colorWipe(uint32_t color, int wait);

void theaterChase(uint32_t color, int wait);

void rainbow(int wait);

void theaterChaseRainbow(int wait);

void underglowTracer(float brightness);

void clearUnderglowLights();

bool isLightInPersistentLight(int led);

#endif //LED_EFFECTS_H

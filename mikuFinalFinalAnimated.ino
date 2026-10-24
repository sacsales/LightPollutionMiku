#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> 
#endif

const int buttonPin = 11;  
//const int ledPin = 13;    
int buttonState = LOW;
int lastButtonState = LOW;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 40;  
int counter = 0;

//pigtail start/end led positions, for convenience 
int rightFirstStart = 0 ;
int rightFirstEnd = 55;
int rightSecondStart = 96;
int rightSecondEnd = 55;
int rightThirdStart = 100;
int rightThirdEnd = 150;
//0 - 55 end for trail 1 
//96 - 55 for trail 2 
// 100 - 150 for trail 3 and so on. 
//strips are in series, but are laid out (top to bottom, bottom to top, then top to bottom again)

int leftFirstStart = 145;
int leftFirstEnd = 185;
int leftSecondStart = 240;
int leftSecondEnd = 185;
int leftThirdStart = 230;
int leftThirdEnd = 300;



volatile bool stopAnimation = false;  

#define LED_PIN    3
#define LED_COUNT 300


Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
    pinMode(buttonPin, INPUT_PULLUP); 
    pinMode(LED_PIN, OUTPUT);
    Serial.begin(9600);
  strip.begin();           
  strip.show();            // turn everything off first 
  strip.setBrightness(100);
  
}

//animations
void colorFill(uint32_t c, uint8_t wait) {
  for(uint16_t i=100; i<150; i++) {
    strip.setPixelColor(i, c);
    strip.show();
    
  }
}

void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i = 0; i < strip.numPixels(); i++) {
    if (stopAnimation) {
      break;  
    }
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void rightPigtailFadeDown(uint32_t c, uint8_t wait) {

  uint16_t trailLength = rightFirstEnd - rightFirstStart;

  for (uint16_t step = 0; step <= trailLength; step++) { 
    // trail 1, 0 to 55 (forward)
    uint16_t trail1pos = rightFirstStart + step;
    if (trail1pos <= rightFirstEnd) {
       strip.setPixelColor(trail1pos, c);
    }
    // trail 2, 96 to 55 (backward, since this one is installed bottom->top)
    int16_t trail2pos = rightSecondStart - step;
    if (trail2pos >= rightSecondEnd) {
       strip.setPixelColor(trail2pos, c);
    }
    // trail 3, 100 to 150 (forward)
    uint16_t trail3pos = rightThirdStart + step;
    if (trail3pos <= rightThirdEnd) {
      strip.setPixelColor(trail3pos, c);
    }

    strip.show();
    delay(wait);
  }
}

void leftPigtailFadeDown(uint32_t c, uint8_t wait) {

  uint16_t trailLength = leftThirdEnd - leftThirdStart;

  for (uint16_t step = 0; step <= trailLength; step++) {
    // trail 1: 145 to 185
    uint16_t trail1pos = leftFirstStart + step;
    if (trail1pos <= leftFirstEnd) {
      strip.setPixelColor(trail1pos, c);
    }

    // trail 2: 185 to 240
    int16_t trail2pos = leftSecondStart - step;
    if (trail2pos >= leftSecondEnd) {
      strip.setPixelColor(trail2pos, c);
    }

    // trail 3: 240 to 300
    uint16_t trail3pos = leftThirdStart + step;
    if (trail3pos <= leftThirdEnd) {
      strip.setPixelColor(trail3pos, c);
    }

    strip.show();
    delay(wait);
  }
}

//flash to a beat
void pulse(uint32_t color, float bpm = 128.0, uint32_t durationMs = 30000) {
  const uint8_t fadeStep = 25;
  const uint8_t flashTime = 30;

  uint16_t beatDuration = (uint16_t)(60000.0 / bpm); 
  uint16_t fadeSteps = 255 / fadeStep;
  uint16_t fadeTime = beatDuration - flashTime;
  uint16_t delayPerStep = fadeTime / fadeSteps;

  uint32_t startTime = millis();

  while (millis() - startTime < durationMs) {
    // Flash ON
    for (uint16_t i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, color);
    }
    strip.show();
    delay(flashTime); // Quick flash

    // Fade OUT
    for (uint8_t b = 255; b > 0; b -= fadeStep) {
      uint8_t r = ((b * ((color >> 16) & 0xFF)) / 255);
      uint8_t g = ((b * ((color >> 8) & 0xFF)) / 255);
      uint8_t b_ = ((b * (color & 0xFF)) / 255);
      uint32_t fadedColor = strip.Color(r, g, b_);

      for (uint16_t i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, fadedColor);
      }
      strip.show();
      delay(delayPerStep);
    }

    
    uint16_t actualCycleTime = flashTime + fadeSteps * delayPerStep;
    if (actualCycleTime < beatDuration) {
      delay(beatDuration - actualCycleTime);
    }
  }

  // Turn off LEDs after pulse ends
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, 0);
  }
  strip.show();
}

//move lights downwards, both
void pigtailsFadeDown(uint32_t c, uint8_t wait) {

  uint16_t trailLength = leftThirdEnd - leftThirdStart;

  for (uint16_t step = 0; step <= trailLength; step++) {
    //right side first
    
    // trail 1, 0 to 55 (forward)
    uint16_t trail1pos = rightFirstStart + step;
    if (trail1pos <= rightFirstEnd) {
       strip.setPixelColor(trail1pos, c);
    }
    // trail 2, 96 to 55 (backward, since this one is installed bottom->top)
    int16_t trail2pos = rightSecondStart - step;
    if (trail2pos >= rightSecondEnd) {
       strip.setPixelColor(trail2pos, c);
    }
    // trail 3, 100 to 150 (forward)
    uint16_t trail3pos = rightThirdStart + step;
    if (trail3pos <= rightThirdEnd) {
      strip.setPixelColor(trail3pos, c);
    }

    // left
    uint16_t leftTrail1pos = leftFirstStart + step;
    if (leftTrail1pos <= leftFirstEnd) {
      strip.setPixelColor(leftTrail1pos, c);
    }

    // trail 2: 185 to 240
    int16_t leftTrail2pos = leftSecondStart - step;
    if (leftTrail2pos >= leftSecondEnd) {
      strip.setPixelColor(leftTrail2pos, c);
    }

    // trail 3: 240 to 300
    uint16_t leftTrail3pos = leftThirdStart + step;
    if (leftTrail3pos <= leftThirdEnd) {
      strip.setPixelColor(leftTrail3pos, c);
    }

    strip.show();
    delay(wait);
  }
}


// move lights upwards
void pigtailsFadeUp(uint32_t c, uint8_t wait) {

  uint16_t trailLength = leftThirdEnd - leftThirdStart;

  for (uint16_t step = 0; step <= trailLength; step++) {
    // right side first
    // trail 1, 55 to 0 (backward)
    int16_t trail1pos = rightFirstEnd - step;
    if (trail1pos >= rightFirstStart) {
      strip.setPixelColor(trail1pos, c);
    }
    // trail 2, 55 to 96 (forward, since this one is installed bottom->top)
    uint16_t trail2pos = rightSecondEnd + step;
    if (trail2pos <= rightSecondStart) {
      strip.setPixelColor(trail2pos, c);
    }
    // trail 3, 150 to 100 (backward)
    int16_t trail3pos = rightThirdEnd - step;
    if (trail3pos >= rightThirdStart) {
      strip.setPixelColor(trail3pos, c);
    }

    // left
    // trail 1: 185 to 145 (backward)
    int16_t leftTrail1pos = leftFirstEnd - step;
    if (leftTrail1pos >= leftFirstStart) {
      strip.setPixelColor(leftTrail1pos, c);
    }
    // trail 2: 185 to 240 (forward)
    uint16_t leftTrail2pos = leftSecondEnd + step;
    if (leftTrail2pos <= leftSecondStart) {
      strip.setPixelColor(leftTrail2pos, c);
    }
    // trail 3: 300 to 240 (backward)
    int16_t leftTrail3pos = leftThirdEnd - step;
    if (leftTrail3pos >= leftThirdStart) {
      strip.setPixelColor(leftTrail3pos, c);
    }

    strip.show();
    delay(wait);
  }
}




// rainbow!
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;
  for(j=0; j<256*2; j++) { 
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//theatre-style crawling lights, any color 
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    // every third pixel on
      }
      strip.show();
      delay(wait);
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        // every third pixel off
      }
    }
  }
}

// what color should the LED be, based on pos in a rainbow wheel
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

//some of the animations above were modified from an older versions of the adafruit neopixel strandtest and strandtest_wheel example code 
//namely the theatre chase/wheel/rainbow cycle!


void loop() {
    int reading = digitalRead(buttonPin);

    // check if the button state has changed
    if (reading != lastButtonState) {
        lastDebounceTime = millis(); // reset debounce timer
    }

    if ((millis() - lastDebounceTime) > debounceDelay) {
      
        // if the button state has changed and stabilized. different light 'modes' 
        if (reading != buttonState) {
          
            counter++;
            if (counter > 3) {
                counter = 0; // reset counter when it reaches 3
            }
            if (counter == 0) {
              colorFill(strip.Color(0, 128, 128), 50); // tealish static color
                strip.show();

            } else if (counter == 1) {
                pigtailsFadeDown(strip.Color(128, 128, 128), 50); //fade both pigtails downwards. white color looks slightly teal with the hair
                strip.show();

            } else if (counter == 2) {
                strip.clear(); //all lights off before performance. standby...
                strip.show();
               
            } else if (counter == 3) { //performance routine, timed to music!
                pulse(strip.Color(0, 0, 255), 170, 7800);
                pigtailsFadeDown(strip.Color(128, 128, 128), 50);
                strip.clear();
                pigtailsFadeUp(strip.Color(128, 128, 128), 50);
                theaterChase(strip.Color(127, 127, 127), 50); // White
                theaterChase(strip.Color(0, 127, 127), 50); // green
                rainbowCycle(2);
                strip.clear();
                rightPigtailFadeDown(strip.Color(51, 255, 255), 50); // green
                leftPigtailFadeDown(strip.Color(51, 255, 255), 50); // green
                pulse(strip.Color(0, 0, 255), 170, 7800); //pulse for last bit 

            }
                
        }
        buttonState = reading;
    }

    lastButtonState = reading;
}







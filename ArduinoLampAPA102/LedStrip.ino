
/**
 * handling setup for the led strip.
 * uses interrupt timer to attach color cycling outside of arduino loop.
 */
void setupLedStrip() {
  currentColor = 0;
  nextColor = 1;
  currentStep = 0;
  currentPixel = 0;
  updateColors(false);
  attachTimerInterrupt();
}

void attachTimerInterrupt() {
  Timer1.initialize(TIMER_INTERRUPT);

  // link timer with callback function
  Timer1.attachInterrupt(play);
}

/**
 * runs through color cycling as long as playerOn is true. 
 * manages currentPixel variable.
 */
void play() {
  logPlayStatus();
  
  if(playerOn) {
    updateLedStrip();
    currentPixel += PIXEL_STEPS;
    
    if(currentPixel > PIXELS) {
      currentPixel = 0;
      updateColors(true);
    }

    updatePlayerStatus();
  }
}

/**
 * sets new color values depending on current cycle iteration.
 * manages currentColor and currentStep variables.
 */
void updateColors(boolean cycleColors) {
  if(cycleColors) {
    if(currentStep < STEPS) {
      currentStep++;
    }
    else {
      currentStep = 0;
      currentColor = nextColor;
      nextColor = currentColor + 1;

      if(nextColor > colors() - 1) {
        nextColor = colors() - 1;
      }
    }
  }
  
  colorRed   = (s2r(color(currentColor)) * (STEPS - currentStep) + s2r(color(nextColor)) * currentStep) / STEPS;
  colorGreen = (s2g(color(currentColor)) * (STEPS - currentStep) + s2g(color(nextColor)) * currentStep) / STEPS;
  colorBlue  = (s2b(color(currentColor)) * (STEPS - currentStep) + s2b(color(nextColor)) * currentStep) / STEPS;  
}

/**
 * sets LED strip to fixed color code
 */
void setColorCode(const char* color) {
  playerOn = false;
  changeBrightness(BRIGHTNESS_MAX);
  colorRed = s2r(color);
  colorGreen = s2g(color);
  colorBlue = s2b(color);
  currentPixel = PIXELS;
}

/**
 * re-calculates rgb values and set led strip
 */
void updateLedStrip() {
  int r = (int)(colorRed * brightness / GLOBAL_DIM);
  int g = (int)(colorGreen * brightness / GLOBAL_DIM);
  int b = (int)(colorBlue * brightness / GLOBAL_DIM);
  
  setStripToColor(r,g,b,false);
}

/**
 * writes provided rgb value to led strip
 */
void setStripToColor(int r, int g, int b, boolean allPixels) {
  int final_pixel = allPixels ? PIXELS : currentPixel;
  int brightness = 1;
  
  for(int i = 0; i < final_pixel; i++) {
    stripColors[i] = rgb_color(r, g, b);
  }

  strip.write(stripColors, PIXELS, brightness);
}

void logPlayStatus() {
  if(LED_DEBUG) {
    Serial.print("play() Color/Step (Pixel): " + currentColor);
    Serial.print("/" + currentStep);
    Serial.print(" (" + currentPixel);
    Serial.print(")");
    
    if(playerOn == false) {
      Serial.print(" player is OFF");
    }
    
    Serial.println();
  }
}

void updatePlayerStatus() {
  if(currentColor == colors() - 1 && currentStep == STEPS - 1 && currentPixel >= PIXELS) {
    playerOn = false;
  }
}

void changeBrightness(float stepSize) {
  brightness = constrain(brightness + stepSize, BRIGHTNESS_MIN, BRIGHTNESS_MAX);
}

byte s2r(const char* s) { return s2v(s, 0); }
byte s2g(const char* s) { return s2v(s, 2); }
byte s2b(const char* s) { return s2v(s, 4); }

byte s2v(const char* s, int offset) {
  char cBuf[3];
  memcpy(cBuf, &s[offset], 2);
  cBuf[2] = '\0';
  return strtoul(cBuf, 0, 16);
}
